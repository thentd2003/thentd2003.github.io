#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include "config.h"
#include "types.h"
#include "cache.h"
#include <stdbool.h>

///////////////////////////////////////////////////////////////////////////////
/// Functionality
///////////////////////////////////////////////////////////////////////////////

extern simulator_config_t sim_config;
extern uint64_t miss_count;
extern uint64_t hit_count;
extern uint64_t total_cycle_counter;
extern uint64_t stall_counter;
extern uint64_t branch_counter;
extern uint64_t fwd_exex_counter;
extern uint64_t fwd_exmem_counter;
extern uint64_t mem_access_counter;
extern uint64_t if_access_counter; // MS4: instruction-cache stall cycles

// MS4: Branch predictor statistics
extern uint64_t bp_lookup_counter;     // total conditional branches resolved
extern uint64_t bp_correct_counter;    // predictions that matched the outcome
extern uint64_t bp_mispredict_counter; // predictions that caused a flush

///////////////////////////////////////////////////////////////////////////////
/// Milestone 4 branch prediction
///////////////////////////////////////////////////////////////////////////////
// uses a 64-entry BHT indexed by PC[7:2]
// the 1-bit predictor stores the last outcome
// the 2-bit predictor uses saturating counters and predicts taken in states 2 or 3
// enable only one predictor in config.h

#if defined(BRANCH_PREDICT_1BIT) && defined(BRANCH_PREDICT_2BIT)
#error "Enable only one of BRANCH_PREDICT_1BIT / BRANCH_PREDICT_2BIT in config.h"
#endif
#if defined(BRANCH_PREDICT_1BIT) || defined(BRANCH_PREDICT_2BIT)
#define BRANCH_PREDICT_ENABLED
#endif

#define BHT_SIZE            64   // entries; index = (pc >> 2) & (BHT_SIZE-1)
#define BP_STRONG_NOT_TAKEN 0
#define BP_WEAK_NOT_TAKEN   1
#define BP_WEAK_TAKEN       2
#define BP_STRONG_TAKEN     3

void    bp_init(void);                          // reset all entries to WEAK_NOT_TAKEN
uint8_t bp_predict(uint32_t pc);                // 1 = predict taken
void    bp_update(uint32_t pc, uint8_t taken);  // saturating inc/dec after resolution

///////////////////////////////////////////////////////////////////////////////
/// NOP encoding and forwarding selectors
///////////////////////////////////////////////////////////////////////////////

#define NOP_INSTR   0x00000013u  // NOP instruction

#define FWD_NONE    0  // no forwarding
#define FWD_EX_MEM  1  // forward from EX/MEM reg
#define FWD_MEM_WB  2  // forward from MEM/WB reg

///////////////////////////////////////////////////////////////////////////////
/// RISC-V Pipeline Register Types
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
  Instruction instr;
  uint32_t    instr_addr;
  uint32_t    pc_plus4;   // pc+4, passed to decode
  uint8_t     pred_taken; // MS4: 1 = fetch predicted this branch taken (0 when predictor disabled)
}ifid_reg_t;

typedef struct
{
  Instruction instr;
  uint32_t    instr_addr;
  uint32_t    pc_plus4;   // pc+4, used by jal for return address
  uint32_t    rs1_val;    // register source 1 value
  uint32_t    rs2_val;    // register source 2 value
  int32_t     imm;        // sign-extended immediate
  uint8_t     reg_write;  // 1 = write result to rd
  uint8_t     mem_to_reg; // 1 = writeback from memory, 0 = from alu
  uint8_t     mem_read;   // 1 = load
  uint8_t     mem_write;  // 1 = store
  uint8_t     branch;     // 1 = branch instruction
  uint8_t     alu_src;    // 0 = rs2, 1 = imm
  uint8_t     alu_op;     // 2-bit aluop: 00=add, 01=sub, 10=decode funct
  uint8_t     is_jal;     // 1 = jal instruction
  uint8_t     pred_taken; // MS4: prediction made at fetch, carried to resolution
}idex_reg_t;

typedef struct
{
  Instruction instr;
  uint32_t    instr_addr;
  uint32_t    alu_result;     // alu output, also used as memory address
  uint32_t    rs1_val;        // forwarded rs1, used by gen_branch's comparator
  uint32_t    rs2_val;        // passed through for stores
  uint32_t    branch_target;  // instr_addr + imm
  uint8_t     zero;           // 1 if alu_result == 0
  uint8_t     funct3;         // passed through for load/store width
  uint8_t     reg_write;
  uint8_t     mem_to_reg;
  uint8_t     mem_read;
  uint8_t     mem_write;
  uint8_t     branch;
  uint8_t     pred_taken; // MS4: prediction made at fetch, compared vs actual outcome here
}exmem_reg_t;

typedef struct
{
  Instruction instr;
  uint32_t    instr_addr;
  uint32_t    alu_result; // passed through for non-load writeback
  uint32_t    mem_data;   // data read from memory for loads
  uint8_t     reg_write;
  uint8_t     mem_to_reg;
}memwb_reg_t;


///////////////////////////////////////////////////////////////////////////////
/// Register types with input and output variants for simulator
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
  ifid_reg_t inp;
  ifid_reg_t out;
}ifid_reg_pair_t;

typedef struct
{
  idex_reg_t inp;
  idex_reg_t out;
}idex_reg_pair_t;

typedef struct
{
  exmem_reg_t inp;
  exmem_reg_t out;
}exmem_reg_pair_t;

typedef struct
{
  memwb_reg_t inp;
  memwb_reg_t out;
}memwb_reg_pair_t;

///////////////////////////////////////////////////////////////////////////////
/// Functional pipeline requirements
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
  ifid_reg_pair_t  ifid_preg;
  idex_reg_pair_t  idex_preg;
  exmem_reg_pair_t exmem_preg;
  memwb_reg_pair_t memwb_preg;
}pipeline_regs_t;

typedef struct
{
  bool      pcsrc;
  uint32_t  pc_src0;
  uint32_t  pc_src1;
  uint8_t   fwd_a;  // forwarding select for alu input a
  uint8_t   fwd_b;  // forwarding select for alu input b
  bool      stall;  // 1 = stall the pipeline
  bool      bubble_pending;  // 1 = bubble IDEX next cycle (the redundant
                             // re-decode of the frozen use instruction),
                             // not the stall cycle itself
  bool      flush;  // 1 = flush the pipeline
}pipeline_wires_t;


///////////////////////////////////////////////////////////////////////////////
/// Function definitions for different stages
///////////////////////////////////////////////////////////////////////////////

/**
 * output : ifid_reg_t
 **/ 
ifid_reg_t stage_fetch(pipeline_wires_t* pwires_p, regfile_t* regfile_p, Byte* memory_p);

/**
 * output : idex_reg_t
 **/ 
idex_reg_t stage_decode(ifid_reg_t ifid_reg, pipeline_wires_t* pwires_p, regfile_t* regfile_p);

/**
 * output : exmem_reg_t
 **/ 
exmem_reg_t stage_execute(idex_reg_t idex_reg, pipeline_wires_t* pwires_p);

/**
 * output : memwb_reg_t
 **/ 
memwb_reg_t stage_mem(exmem_reg_t exmem_reg, pipeline_wires_t* pwires_p, Byte* memory, Cache* cache_p);

/**
 * output : write_data
 **/ 
void stage_writeback(memwb_reg_t memwb_reg, pipeline_wires_t* pwires_p, regfile_t* regfile_p);

// keeps the original framework interface; MS4 caches are managed internally
void cycle_pipeline(regfile_t* regfile_p, Byte* memory_p, Cache* cache_p, pipeline_regs_t* pregs_p, pipeline_wires_t* pwires_p, bool* ecall_exit);

// Milestone 4 two-level cache helpers
void cacheSetUpCustom(Cache *cache, char *name, int setBits, int linesPerSet, int blockBits);
void cacheSetUpL2(Cache *cache, char *name);
int  processTwoLevelCacheOperation(unsigned long address, Cache *l1, Cache *l2);
// MS4 statistics are printed from pipeline.c at program exit

void bootstrap(pipeline_wires_t* pwires_p, pipeline_regs_t* pregs_p, regfile_t* regfile_p);

#endif  // __PIPELINE_H__
