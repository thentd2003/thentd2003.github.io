#ifndef __STAGE_HELPERS_H__
#define __STAGE_HELPERS_H__

#include <stdio.h>
#include "utils.h"
#include "pipeline.h"

#define ALU_ADD   0x0
#define ALU_SUB   0x1
#define ALU_SLL   0x2
#define ALU_SLT   0x3
#define ALU_XOR   0x4
#define ALU_SRL   0x5
#define ALU_SRA   0x6
#define ALU_OR    0x7
#define ALU_AND   0x8
#define ALU_MUL   0x9
#define ALU_MULH  0xA
#define ALU_DIV   0xB
#define ALU_REM   0xC

/// EXECUTE STAGE HELPERS ///

// ALU control unit: maps alu_op (0=add, 1=sub, 2=R-type funct decode,
// 3=I-type funct decode) + funct3/funct7 to a specific ALU_* operation
uint32_t gen_alu_control(idex_reg_t idex_reg)
{
  uint32_t alu_control = 0;
  switch(idex_reg.alu_op)
  {
    case 0x0: alu_control = ALU_ADD; break;
    case 0x1: alu_control = ALU_SUB; break;
    case 0x2:
      switch(idex_reg.instr.rtype.funct3)
      {
        case 0x0:
          switch(idex_reg.instr.rtype.funct7)
          {
            case 0x00: alu_control = ALU_ADD; break;
            case 0x01: alu_control = ALU_MUL; break;
            case 0x20: alu_control = ALU_SUB; break;
          }
          break;
        case 0x1:
          switch(idex_reg.instr.rtype.funct7)
          {
            case 0x00: alu_control = ALU_SLL;  break;
            case 0x01: alu_control = ALU_MULH; break;
          }
          break;
        case 0x2: alu_control = ALU_SLT; break;
        case 0x4:
          switch(idex_reg.instr.rtype.funct7)
          {
            case 0x00: alu_control = ALU_XOR; break;
            case 0x01: alu_control = ALU_DIV; break;
          }
          break;
        case 0x5:
          switch(idex_reg.instr.rtype.funct7)
          {
            case 0x00: alu_control = ALU_SRL; break;
            case 0x20: alu_control = ALU_SRA; break;
          }
          break;
        case 0x6:
          switch(idex_reg.instr.rtype.funct7)
          {
            case 0x00: alu_control = ALU_OR;  break;
            case 0x01: alu_control = ALU_REM; break;
          }
          break;
        case 0x7: alu_control = ALU_AND; break;
      }
      break;
    case 0x3:
      switch(idex_reg.instr.itype.funct3)
      {
        case 0x0: alu_control = ALU_ADD; break;
        case 0x1: alu_control = ALU_SLL; break;
        case 0x2: alu_control = ALU_SLT; break;
        case 0x4: alu_control = ALU_XOR; break;
        case 0x5:
          alu_control = (((idex_reg.instr.itype.imm >> 5) & 0x7F) == 0x20) ? ALU_SRA : ALU_SRL;
          break;
        case 0x6: alu_control = ALU_OR;  break;
        case 0x7: alu_control = ALU_AND; break;
      }
      break;
  }
  return alu_control;
}

// The ALU: computes alu_inp1 (op) alu_inp2 per alu_control
// Signed casts and div/rem edge cases follow the lab 3 emulator semantics
uint32_t execute_alu(uint32_t alu_inp1, uint32_t alu_inp2, uint32_t alu_control)
{
  uint32_t result;
  switch(alu_control){
    case ALU_ADD: result = alu_inp1 + alu_inp2; break;
    case ALU_SUB: result = alu_inp1 - alu_inp2; break;
    case ALU_SLL: result = alu_inp1 << (alu_inp2 & 0x1F); break;
    case ALU_SLT: result = ((sWord)alu_inp1 < (sWord)alu_inp2) ? 1 : 0; break;
    case ALU_XOR: result = alu_inp1 ^ alu_inp2; break;
    case ALU_SRL: result = alu_inp1 >> (alu_inp2 & 0x1F); break;
    case ALU_SRA: result = (uint32_t)(((sWord)alu_inp1) >> (alu_inp2 & 0x1F)); break;
    case ALU_OR:  result = alu_inp1 | alu_inp2; break;
    case ALU_AND: result = alu_inp1 & alu_inp2; break;
    case ALU_MUL: result = (uint32_t)(((sWord)alu_inp1) * ((sWord)alu_inp2)); break;
    case ALU_MULH:
      result = (uint32_t)(((Double)(((sDouble)(sWord)alu_inp1) * ((sDouble)(sWord)alu_inp2))) >> 32);
      break;
    case ALU_DIV:
      if      (alu_inp2 == 0)                                       result = 0xFFFFFFFF;
      else if (alu_inp1 == 0x80000000 && alu_inp2 == 0xFFFFFFFF)    result = 0x80000000;
      else    result = (uint32_t)(((sWord)alu_inp1) / ((sWord)alu_inp2));
      break;
    case ALU_REM:
      if      (alu_inp2 == 0)                                       result = alu_inp1;
      else if (alu_inp1 == 0x80000000 && alu_inp2 == 0xFFFFFFFF)    result = 0;
      else    result = (uint32_t)(((sWord)alu_inp1) % ((sWord)alu_inp2));
      break;
    default: result = 0xBADCAFFE; break;
  };
  return result;
}

/// DECODE STAGE HELPERS ///

// ImmGen: produces the sign-extended 32-bit immediate for each format
// (I/S/SB/U/UJ); R-type and ecall have none

uint32_t gen_imm(Instruction instruction)
{
  int imm_val = 0;
  switch(instruction.opcode) {
    case 0x63: imm_val = get_branch_offset(instruction); break;
    case 0x03:
    case 0x13: imm_val = sign_extend_number(instruction.itype.imm, 12); break;
    case 0x23: imm_val = get_store_offset(instruction); break;
    case 0x37: imm_val = (int)(instruction.utype.imm << 12); break;
    case 0x6F: imm_val = get_jump_offset(instruction); break;
  };
  return imm_val;
}

// Main control unit (ID stage): sets reg_write, mem_read/write/to_reg,
// alu_src, alu_op, branch, is_jal per opcode
idex_reg_t gen_control(Instruction instruction)
{
  idex_reg_t idex_reg = {0};
  switch(instruction.opcode) {
    case 0x33:
      idex_reg.reg_write = 1; idex_reg.alu_src = 0; idex_reg.alu_op = 2;
      break;
    case 0x13:
      idex_reg.reg_write = 1; idex_reg.alu_src = 1; idex_reg.alu_op = 3;
      break;
    case 0x03:
      idex_reg.reg_write = 1; idex_reg.mem_to_reg = 1; idex_reg.mem_read = 1;
      idex_reg.alu_src = 1; idex_reg.alu_op = 0;
      break;
    case 0x23:
      idex_reg.mem_write = 1; idex_reg.alu_src = 1; idex_reg.alu_op = 0;
      break;
    case 0x63:
      idex_reg.branch = 1; idex_reg.alu_src = 0; idex_reg.alu_op = 1;
      break;
    case 0x37:
      idex_reg.reg_write = 1; idex_reg.alu_src = 1; idex_reg.alu_op = 0;
      break;
    case 0x6F:
      idex_reg.reg_write = 1; idex_reg.is_jal = 1;
      break;
  }
  return idex_reg;
}

/// MEMORY STAGE HELPERS ///

/**
 * evaluates whether a branch must be taken
 * input  : <open to implementation>
 * output : bool
 **/
bool gen_branch(exmem_reg_t exmem_reg)
{
  if (!exmem_reg.branch) return false;

  // The EX stage computed rs1 - rs2, so `zero` already answers beq/bne.
  // The ordered comparisons cannot be derived from `zero` (and a SUB's sign
  // bit is wrong on overflow), so compare the forwarded operands directly.
  sWord rs1_s = (sWord)exmem_reg.rs1_val;
  sWord rs2_s = (sWord)exmem_reg.rs2_val;
  Word  rs1_u = exmem_reg.rs1_val;
  Word  rs2_u = exmem_reg.rs2_val;

  switch (exmem_reg.funct3)
  {
    case 0x0: return  exmem_reg.zero;      // beq
    case 0x1: return !exmem_reg.zero;      // bne
    case 0x4: return (rs1_s <  rs2_s);     // blt
    case 0x5: return (rs1_s >= rs2_s);     // bge
    case 0x6: return (rs1_u <  rs2_u);     // bltu
    case 0x7: return (rs1_u >= rs2_u);     // bgeu
    default:  return false;
  }
}

/// PIPELINE FEATURES ///

/**
 * Task   : Sets the pipeline wires for the forwarding unit's control signals
 *           based on the pipeline register values.
 * input  : pipeline_regs_t*, pipeline_wires_t*
 * output : None
*/
void gen_forward(pipeline_regs_t* pregs_p, pipeline_wires_t* pwires_p)
{
  pwires_p->fwd_a = 0;
  pwires_p->fwd_b = 0;
 
  idex_reg_t  idex  = pregs_p->idex_preg.out;
  exmem_reg_t exmem = pregs_p->exmem_preg.out;
  memwb_reg_t memwb = pregs_p->memwb_preg.out;
 
  if(idex.instr.bits == 0)
    return;
 
  uint32_t opcode = idex.instr.opcode;
  bool reads_rs1 = (opcode==0x33 || opcode==0x13 || opcode==0x03 || opcode==0x23 || opcode==0x63);
  bool reads_rs2 = (opcode==0x33 || opcode==0x23 || opcode==0x63);
 
  uint32_t rs1 = idex.instr.rtype.rs1;
  uint32_t rs2 = idex.instr.rtype.rs2;
 
  bool ex_hazard_rs1  = reads_rs1 && exmem.reg_write && exmem.instr.rtype.rd != 0 && exmem.instr.rtype.rd == rs1;
  bool ex_hazard_rs2  = reads_rs2 && exmem.reg_write && exmem.instr.rtype.rd != 0 && exmem.instr.rtype.rd == rs2;
  bool mem_hazard_rs1 = !ex_hazard_rs1 && reads_rs1 && memwb.reg_write && memwb.instr.rtype.rd != 0 && memwb.instr.rtype.rd == rs1;
  bool mem_hazard_rs2 = !ex_hazard_rs2 && reads_rs2 && memwb.reg_write && memwb.instr.rtype.rd != 0 && memwb.instr.rtype.rd == rs2;
 
  // EX hazards (fresher value, closer stage) print before MEM hazards,
  // regardless of which operand (rs1/rs2) each is on
  if(ex_hazard_rs1)
  {
    pwires_p->fwd_a = 1;
    fwd_exex_counter++;
    #ifdef DEBUG_CYCLE
    printf("[FWD]: Resolving EX hazard on rs1: x%d\n", rs1);
    #endif
  }
  if(ex_hazard_rs2)
  {
    pwires_p->fwd_b = 1;
    fwd_exex_counter++;
    #ifdef DEBUG_CYCLE
    printf("[FWD]: Resolving EX hazard on rs2: x%d\n", rs2);
    #endif
  }
  if(mem_hazard_rs1)
  {
    pwires_p->fwd_a = 2;
    fwd_exmem_counter++;
    #ifdef DEBUG_CYCLE
    printf("[FWD]: Resolving MEM hazard on rs1: x%d\n", rs1);
    #endif
  }
  if(mem_hazard_rs2)
  {
    pwires_p->fwd_b = 2;
    fwd_exmem_counter++;
    #ifdef DEBUG_CYCLE
    printf("[FWD]: Resolving MEM hazard on rs2: x%d\n", rs2);
    #endif
  }
}

/**
 * Task   : Sets the pipeline wires for the hazard unit's control signals
 *           based on the pipeline register values.
 * input  : pipeline_regs_t*, pipeline_wires_t*
 * output : None
*/
void detect_hazard(pipeline_regs_t* pregs_p, pipeline_wires_t* pwires_p, regfile_t* regfile_p)
{
  pwires_p->stall = false;
 
  ifid_reg_t ifid = pregs_p->ifid_preg.out;
  idex_reg_t idex = pregs_p->idex_preg.out;
 
  if(ifid.instr.bits == 0 || !idex.mem_read)
    return;
 
  uint32_t load_rd = idex.instr.rtype.rd;
  if(load_rd == 0)
    return;
 
  Instruction use_instr = parse_instruction(ifid.instr.bits);
  uint32_t opcode = use_instr.opcode;
  bool reads_rs1 = (opcode==0x33 || opcode==0x13 || opcode==0x03 || opcode==0x23 || opcode==0x63);
  bool reads_rs2 = (opcode==0x33 || opcode==0x23 || opcode==0x63);
 
  if((reads_rs1 && use_instr.rtype.rs1 == load_rd) ||
     (reads_rs2 && use_instr.rtype.rs2 == load_rd))
  {
    pwires_p->stall = true;
    stall_counter++;
  }
}
///////////////////////////////////////////////////////////////////////////////

/// RESERVED FOR PRINTING REGISTER TRACE AFTER EACH CLOCK CYCLE ///
void print_register_trace(regfile_t* regfile_p)
{
  // print
  for (uint8_t i = 0; i < 8; i++)       // 8 columns
  {
    for (uint8_t j = 0; j < 4; j++)     // of 4 registers each
    {
      printf("r%2d=%08x ", i * 4 + j, regfile_p->R[i * 4 + j]);
    }
    printf("\n");
  }
  printf("\n");
}

#endif // __STAGE_HELPERS_H__