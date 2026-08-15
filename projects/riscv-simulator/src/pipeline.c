#include <stdbool.h>
#include "cache.h"
#include "riscv.h"
#include "types.h"
#include "utils.h"
#include "pipeline.h"
#include "stage_helpers.h"

uint64_t total_cycle_counter = 0;
uint64_t mem_access_counter = 0;
uint64_t if_access_counter = 0; // MS4: L1-I/L2 stall cycles
uint64_t miss_count = 0;
uint64_t hit_count = 0;
uint64_t stall_counter = 0;
uint64_t branch_counter = 0;
uint64_t fwd_exex_counter = 0;
uint64_t fwd_exmem_counter = 0;

simulator_config_t sim_config = {0};

///////////////////////////////////////////////////////////////////////////////
/// Milestone 4: two-level cache hierarchy (TWO_LEVEL_CACHE_ENABLE)
///////////////////////////////////////////////////////////////////////////////
// L1-I and L2 are kept here so riscv.c can use its original MS3 interface

#ifdef TWO_LEVEL_CACHE_ENABLE
static Cache ms4_icache;               // L1-I: instruction fetches
static Cache ms4_l2cache;              // unified L2 behind L1-I and L1-D
static bool  ms4_caches_ready = false;

static void ms4_setup_caches(void)
{
    cacheSetUp(&ms4_icache, "L1-I");
    cacheSetUpL2(&ms4_l2cache, "L2");
    ms4_caches_ready = true;
}

// sopy L1-D counters before riscv.c's local cache goes out of scope
static int ms4_dcache_hits   = 0;
static int ms4_dcache_misses = 0;
#endif

///////////////////////////////////////////////////////////////////////////////
/// Milestone 4: branch predictor (BRANCH_PREDICT_1BIT / BRANCH_PREDICT_2BIT)
///////////////////////////////////////////////////////////////////////////////

uint64_t bp_lookup_counter     = 0;
uint64_t bp_correct_counter    = 0;
uint64_t bp_mispredict_counter = 0;

// 64-entry BHT indexed by PC[7:2]. Entries store the last outcome or a
// 2-bit saturating state, depending on the selected predictor
static uint8_t bht[BHT_SIZE];

void bp_init(void)
{
#ifdef BRANCH_PREDICT_1BIT
    // start each entry as not taken
    for (int i = 0; i < BHT_SIZE; i++)
        bht[i] = 0;
#else
    // start each entry as weakly not taken
    for (int i = 0; i < BHT_SIZE; i++)
        bht[i] = BP_WEAK_NOT_TAKEN;
#endif
}

static inline uint32_t bp_index(uint32_t pc)
{
    return (pc >> 2) & (BHT_SIZE - 1);
}

uint8_t bp_predict(uint32_t pc)
{
#ifdef BRANCH_PREDICT_1BIT
    return bht[bp_index(pc)];                          // predict last outcome
#else
    return (bht[bp_index(pc)] >= BP_WEAK_TAKEN) ? 1 : 0;
#endif
}

void bp_update(uint32_t pc, uint8_t taken)
{
#ifdef BRANCH_PREDICT_1BIT
    bht[bp_index(pc)] = taken ? 1 : 0;                 // remember last outcome
#else
    uint8_t* state = &bht[bp_index(pc)];
    if (taken)
    {
        if (*state < BP_STRONG_TAKEN) (*state)++;
    }
    else
    {
        if (*state > BP_STRONG_NOT_TAKEN) (*state)--;
    }
#endif
}

///////////////////////////////////////////////////////////////////////////////
/// Milestone 4: end-of-run statistics
///////////////////////////////////////////////////////////////////////////////
// print MS4 statistics here so riscv.c remains unchanged

#if defined(PRINT_STATS) && (defined(TWO_LEVEL_CACHE_ENABLE) || defined(BRANCH_PREDICT_ENABLED))
__attribute__((destructor))
static void ms4_print_stats(void)
{
#ifdef BRANCH_PREDICT_ENABLED
    printf("#BP lookups        = %5ld\n", bp_lookup_counter);
    printf("#BP correct        = %5ld\n", bp_correct_counter);
    printf("#BP mispredicts    = %5ld\n", bp_mispredict_counter);
    printf("#BP accuracy       = %.2f%%\n", bp_lookup_counter
            ? 100.0 * (double)bp_correct_counter / (double)bp_lookup_counter
            : 0.0);
#endif
#ifdef TWO_LEVEL_CACHE_ENABLE
    printf("#MEM   stalls      = %5ld\n", mem_access_counter + if_access_counter);
    printf("#L1-I accesses     = %5d\n", ms4_icache.hit_count + ms4_icache.miss_count);
    printf("#L1-I hits         = %5d\n", ms4_icache.hit_count);
    printf("#L1-I misses       = %5d\n", ms4_icache.miss_count);
    printf("#L1-D accesses     = %5d\n", ms4_dcache_hits + ms4_dcache_misses);
    printf("#L1-D hits         = %5d\n", ms4_dcache_hits);
    printf("#L1-D misses       = %5d\n", ms4_dcache_misses);
    printf("#L2    accesses    = %5d\n", ms4_l2cache.hit_count + ms4_l2cache.miss_count);
    printf("#L2    hits        = %5d\n", ms4_l2cache.hit_count);
    printf("#L2    misses      = %5d\n", ms4_l2cache.miss_count);
#endif
}
#endif

///////////////////////////////////////////////////////////////////////////////

void bootstrap(pipeline_wires_t* pwires_p, pipeline_regs_t* pregs_p, regfile_t* regfile_p)
{
    // PC src must get the same value as the default PC value
    pwires_p->pc_src0 = regfile_p->PC;

#ifdef BRANCH_PREDICT_ENABLED
    bp_init();
#endif
}

///////////////////////////
/// STAGE FUNCTIONALITY ///
///////////////////////////

/**
 * STAGE  : stage_fetch
 * output : ifid_reg_t
 **/
ifid_reg_t stage_fetch(pipeline_wires_t* pwires_p, regfile_t* regfile_p, Byte* memory_p)
{
    ifid_reg_t ifid_reg = {0};

    uint32_t pc = pwires_p->pcsrc ? pwires_p->pc_src1 : regfile_p->PC;
    uint32_t instruction_bits = load(memory_p, pc, LENGTH_WORD);

    ifid_reg.instr.bits = instruction_bits;
    ifid_reg.instr_addr = pc;
    ifid_reg.pc_plus4    = pc + 4;

    // MS4: L1-I checked first, falling through to L2 only on an L1-I miss, same latency 
    // accounting as stage_mem
#ifdef TWO_LEVEL_CACHE_ENABLE
    int if_latency = processTwoLevelCacheOperation((unsigned long)pc, &ms4_icache, &ms4_l2cache);
    uint64_t if_extra_cycles = if_latency - CACHE_HIT_LATENCY; // 0 on an L1-I hit, more on an L2 hit/miss
    total_cycle_counter += if_extra_cycles;
    if_access_counter    += if_extra_cycles;
#ifdef PRINT_CACHE_TRACES
    printf("[IF ]: Cache latency at addr: 0x%08x: %d cycles\n", pc, if_latency);
#endif
#endif

#ifdef DEBUG_CYCLE
    printf("[IF ]: Instruction [%08x]@[%08x]: ", instruction_bits, pc);
    decode_instruction(instruction_bits);
#endif

    // default next PC
    pwires_p->pc_src0 = pc + 4;

#ifdef BRANCH_PREDICT_ENABLED
    // MS4: Predict conditional branches in IF and redirect the next fetch when taken
    // Carry the prediction to MEM for verification
    if (ifid_reg.instr.opcode == 0x63 && bp_predict(pc))
    {
        ifid_reg.pred_taken = 1;
        pwires_p->pc_src0   = pc + get_branch_offset(ifid_reg.instr);
#ifdef DEBUG_CYCLE
        printf("[BP ]: Predict taken @[%08x] -> [%08x]\n", pc, pwires_p->pc_src0);
#endif
    }
#endif

    if (pwires_p->stall)
    {
        // Load-use hazard: hold the PC so the same instruction is fetched again
        // next cycle (IF/ID is frozen, the bubble is inserted in stage_decode)
#ifdef DEBUG_CYCLE
        printf("[HZD]: Stalling and rewriting PC: 0x%08x\n", pc);
#endif
        regfile_p->PC = pc;
    }
    else
    {
        regfile_p->PC = pwires_p->pc_src0;
    }

    return ifid_reg;
}

/**
 * STAGE  : stage_decode
 * output : idex_reg_t
 **/
idex_reg_t stage_decode(ifid_reg_t ifid_reg, pipeline_wires_t* pwires_p, regfile_t* regfile_p)
{
    Instruction instr = ifid_reg.instr;

#ifdef DEBUG_CYCLE
    printf("[ID ]: Instruction [%08x]@[%08x]: ", instr.bits, ifid_reg.instr_addr);
    decode_instruction(instr.bits);
#endif

    // Control unit + immediate generator (both fully implemented in stage_helpers.h)
    idex_reg_t idex_reg = gen_control(instr);

    idex_reg.instr      = instr;
    idex_reg.instr_addr = ifid_reg.instr_addr;
    idex_reg.pc_plus4    = ifid_reg.pc_plus4;
    idex_reg.imm         = gen_imm(instr);
    idex_reg.pred_taken  = ifid_reg.pred_taken; // MS4: carry prediction to resolution

    idex_reg.rs1_val = regfile_p->R[instr.rtype.rs1];
    idex_reg.rs2_val = regfile_p->R[instr.rtype.rs2];

    // LUI's encoding has no rs1 field (bits 15-19 are part of the 20-bit imm)
    // force operand A to 0 so ALU_ADD(0, imm) == imm
    if (instr.opcode == 0x37)
    {
        idex_reg.rs1_val = 0;
    }

    // Clear control signals to insert a bubble, IF/ID is decoded again next cycle
    if (pwires_p->stall)
    {
        idex_reg_t bubble = {0};
        bubble.instr      = instr;
        bubble.instr_addr = ifid_reg.instr_addr;
        bubble.pc_plus4   = ifid_reg.pc_plus4;
        return bubble;
    }

    return idex_reg;
}

/**
 * STAGE  : stage_execute
 * output : exmem_reg_t
 **/
exmem_reg_t stage_execute(idex_reg_t idex_reg, pipeline_wires_t* pwires_p)
{
    exmem_reg_t exmem_reg = {0};

    exmem_reg.instr         = idex_reg.instr;
    exmem_reg.instr_addr    = idex_reg.instr_addr;
    exmem_reg.rs1_val       = idex_reg.rs1_val; // fix from header file, added for branch comparison
    exmem_reg.rs2_val       = idex_reg.rs2_val;   // store data, passed through
    exmem_reg.funct3        = idex_reg.instr.rtype.funct3;
    exmem_reg.reg_write     = idex_reg.reg_write;
    exmem_reg.mem_to_reg    = idex_reg.mem_to_reg;
    exmem_reg.mem_read      = idex_reg.mem_read;
    exmem_reg.mem_write     = idex_reg.mem_write;
    exmem_reg.branch        = idex_reg.branch;
    exmem_reg.branch_target = idex_reg.instr_addr + idex_reg.imm;
    exmem_reg.pred_taken    = idex_reg.pred_taken; // MS4: carry prediction to resolution

    // Note: idex_reg.rs1_val / rs2_val here already reflect any forwarding
    // applied by the caller (cycle_pipeline), based on pwires_p->fwd_a/fwd_b
    uint32_t alu_inp1 = idex_reg.rs1_val;
    uint32_t alu_inp2 = idex_reg.alu_src ? (uint32_t)idex_reg.imm : idex_reg.rs2_val;

    uint32_t alu_control = gen_alu_control(idex_reg);
    uint32_t alu_result  = execute_alu(alu_inp1, alu_inp2, alu_control);

    // JAL writes pc+4 to rd, not an ALU result
    if (idex_reg.is_jal)
    {
        alu_result = idex_reg.pc_plus4;
    }

    exmem_reg.alu_result = alu_result;
    exmem_reg.zero       = (alu_result == 0) ? 1 : 0;

#ifdef DEBUG_CYCLE
    printf("[EX ]: Instruction [%08x]@[%08x]: ", idex_reg.instr.bits, idex_reg.instr_addr);
    decode_instruction(idex_reg.instr.bits);
#endif

    return exmem_reg;
}

/**
 * STAGE  : stage_mem
 * output : memwb_reg_t
 **/
memwb_reg_t stage_mem(exmem_reg_t exmem_reg, pipeline_wires_t* pwires_p, Byte* memory_p, Cache* cache_p)
{
    memwb_reg_t memwb_reg = {0};

    memwb_reg.instr      = exmem_reg.instr;
    memwb_reg.instr_addr = exmem_reg.instr_addr;
    memwb_reg.alu_result = exmem_reg.alu_result;
    memwb_reg.reg_write  = exmem_reg.reg_write;
    memwb_reg.mem_to_reg = exmem_reg.mem_to_reg;

    Alignment align;
    switch (exmem_reg.funct3 & 0x3)
    {
    case 0:
        align = LENGTH_BYTE;
        break;
    case 1:
        align = LENGTH_HALF_WORD;
        break;
    default:
        align = LENGTH_WORD;
        break;
    }

    if (exmem_reg.mem_read)
    {
        Word raw = load(memory_p, exmem_reg.alu_result, align);
        // sign/zero-extend per funct3: lb=0x0, lh=0x1, lw=0x2, lbu=0x4, lhu=0x5
        switch (exmem_reg.funct3)
        {
        case 0x0:
            memwb_reg.mem_data = (uint32_t)sign_extend_number(raw, 8);
            break;
        case 0x1:
            memwb_reg.mem_data = (uint32_t)sign_extend_number(raw, 16);
            break;
        case 0x2:
            memwb_reg.mem_data = raw;
            break;
        case 0x4:
            memwb_reg.mem_data = raw & 0xFF;
            break;
        case 0x5:
            memwb_reg.mem_data = raw & 0xFFFF;
            break;
        default:
            memwb_reg.mem_data = raw;
            break;
        }
    }
    else if (exmem_reg.mem_write)
    {
        store(memory_p, exmem_reg.alu_result, align, exmem_reg.rs2_val);
    }

#ifdef DEBUG_CYCLE
    printf("[MEM]: Instruction [%08x]@[%08x]: ", exmem_reg.instr.bits, exmem_reg.instr_addr);
    decode_instruction(exmem_reg.instr.bits);
#endif

    // MS3: cache / memory access latency
    if (exmem_reg.mem_read || exmem_reg.mem_write)
    {
#ifdef TWO_LEVEL_CACHE_ENABLE
        // MS4: L1-D checked first, falling through to L2 only on an L1-D miss
        int latency = processTwoLevelCacheOperation((unsigned long)exmem_reg.alu_result, cache_p, &ms4_l2cache);
        uint64_t extra_cycles = latency - CACHE_HIT_LATENCY; // 0 on an L1-D hit, more on an L2 hit/miss
        total_cycle_counter += extra_cycles;
        mem_access_counter  += extra_cycles;
#ifdef PRINT_CACHE_TRACES
        printf("[MEM]: Cache latency at addr: 0x%08x: %d cycles\n", exmem_reg.alu_result, latency);
#endif
#elif defined(CACHE_ENABLE)
        int latency = processCacheOperation((unsigned long)exmem_reg.alu_result, cache_p);
        uint64_t extra_cycles;
        if (latency == CACHE_HIT_LATENCY)
        {
            hit_count++;
            extra_cycles = CACHE_HIT_LATENCY - 1;
        }
        else
        {
            miss_count++;
            extra_cycles = CACHE_MISS_LATENCY - 1;
        }
        total_cycle_counter += extra_cycles;
        mem_access_counter  += extra_cycles;
#ifdef PRINT_CACHE_TRACES
        printf("[MEM]: Cache latency at addr: 0x%08x: %d cycles\n", exmem_reg.alu_result, latency);
#endif
#else
        const uint64_t mem_stall = (MEM_LATENCY > 1) ? (MEM_LATENCY - 1) : 0;
        total_cycle_counter += mem_stall;
        mem_access_counter++;   // counts accesses; riscv.c multiplies by (MEM_LATENCY-1)
#endif
    }

    // Branch / jump resolution (decided in MEM per MS2 spec, JAL is always "taken")
#ifdef BRANCH_PREDICT_ENABLED
    // MS4: Flush only on a conditional-branch misprediction
    // JAL remains unconditional and always redirects
    pwires_p->pcsrc = false;

    if (exmem_reg.branch && exmem_reg.instr.opcode == 0x63)
    {
        bool taken = gen_branch(exmem_reg);
        bp_lookup_counter++;

        if (taken != (bool)exmem_reg.pred_taken)
        {
            bp_mispredict_counter++;
            pwires_p->pcsrc   = true;
            pwires_p->pc_src1 = taken ? exmem_reg.branch_target
                                      : exmem_reg.instr_addr + 4;
#ifdef DEBUG_CYCLE
            printf("[BP ]: Mispredict @[%08x] (pred %s, actual %s), redirect to [%08x]\n",
                   exmem_reg.instr_addr,
                   exmem_reg.pred_taken ? "T" : "NT",
                   taken ? "T" : "NT",
                   pwires_p->pc_src1);
#endif
        }
        else
        {
            bp_correct_counter++;
#ifdef DEBUG_CYCLE
            printf("[BP ]: Correct prediction @[%08x] (%s), no flush\n",
                   exmem_reg.instr_addr, taken ? "T" : "NT");
#endif
        }

        if (taken)
            branch_counter++; // keep "#Branches taken" counting actual outcomes

        bp_update(exmem_reg.instr_addr, taken ? 1 : 0);
    }
    else if (exmem_reg.instr.opcode == 0x6F) // JAL: unconditional, always redirect
    {
        pwires_p->pcsrc   = true;
        pwires_p->pc_src1 = exmem_reg.branch_target;
        branch_counter++; // jal counted as "taken", matching the old stat's meaning
    }
#else
    bool taken = gen_branch(exmem_reg) || (exmem_reg.instr.opcode == 0x6F);
    if (taken)
    {
        pwires_p->pcsrc   = true;
        pwires_p->pc_src1 = exmem_reg.branch_target;
    }
    else
    {
        pwires_p->pcsrc = false;
    }
#endif

    return memwb_reg;
}

/**
 * STAGE  : stage_writeback
 * output : nothing - The state of the register file may be changed
 **/
void stage_writeback(memwb_reg_t memwb_reg, pipeline_wires_t* pwires_p, regfile_t* regfile_p)
{
#ifdef DEBUG_CYCLE
    printf("[WB ]: Instruction [%08x]@[%08x]: ", memwb_reg.instr.bits, memwb_reg.instr_addr);
    decode_instruction(memwb_reg.instr.bits);
#endif

    if (memwb_reg.reg_write)
    {
        uint32_t rd = memwb_reg.instr.rtype.rd;
        if (rd != 0) // never write x0
        {
            regfile_p->R[rd] = memwb_reg.mem_to_reg ? memwb_reg.mem_data : memwb_reg.alu_result;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

/**
 * excite the pipeline with one clock cycle
 **/
void cycle_pipeline(regfile_t* regfile_p, Byte* memory_p, Cache* cache_p, pipeline_regs_t* pregs_p, pipeline_wires_t* pwires_p, bool* ecall_exit)
{
#ifdef TWO_LEVEL_CACHE_ENABLE
    // Milestone 4: lazy one-time setup of the file-static L1-I and L2
    // (riscv.c only owns the L1-D it passes in as cache_p).
    if (!ms4_caches_ready)
        ms4_setup_caches();
#endif

#ifdef DEBUG_CYCLE
    printf("v==============");
    printf("Cycle Counter = %5ld", total_cycle_counter);
    printf("==============v\n\n");
#endif

    // process each stage

    // Hazard detection unit
    if (sim_config.fwd_en)
        detect_hazard(pregs_p, pwires_p, regfile_p);
    else
        pwires_p->stall = false;

    /* Output               |    Stage      |       Inputs  */
    pregs_p->ifid_preg.inp  = stage_fetch     (pwires_p, regfile_p, memory_p);

    pregs_p->idex_preg.inp  = stage_decode    (pregs_p->ifid_preg.out, pwires_p, regfile_p);

#if defined(BRANCH_PREDICT_ENABLED) && !defined(DISABLE_WB_BYPASS)
    // MS4: Bypass a same-cycle WB result into ID when prediction removes branch bubbles
    {
        memwb_reg_t wb = pregs_p->memwb_preg.out;
        if (wb.reg_write && wb.instr.rtype.rd != 0)
        {
            uint32_t wb_val = wb.mem_to_reg ? wb.mem_data : wb.alu_result;
            uint32_t op     = pregs_p->idex_preg.inp.instr.opcode;
            // LUI/AUIPC/JAL have no rs1 field (those bits are immediate)
            bool has_rs1 = (op != 0x37) && (op != 0x17) && (op != 0x6F);
            if (has_rs1 && pregs_p->idex_preg.inp.instr.rtype.rs1 == wb.instr.rtype.rd)
                pregs_p->idex_preg.inp.rs1_val = wb_val;
            if (has_rs1 && pregs_p->idex_preg.inp.instr.rtype.rs2 == wb.instr.rtype.rd)
                pregs_p->idex_preg.inp.rs2_val = wb_val;
        }
    }
#endif

    // Forwarding unit: Select forwarding sources for the instruction entering EX
    pwires_p->fwd_a = 0;
    pwires_p->fwd_b = 0;
    if (sim_config.fwd_en)
        gen_forward(pregs_p, pwires_p);

    idex_reg_t idex_fwd = pregs_p->idex_preg.out;

    // for a load in EX/MEM, read the loaded value for same-cycle forwarding
    uint32_t ex_fwd_val = pregs_p->exmem_preg.out.alu_result;
    if (pregs_p->exmem_preg.out.mem_read)
    {
        Address addr = pregs_p->exmem_preg.out.alu_result;
        switch (pregs_p->exmem_preg.out.funct3)
        {
            case 0x0: ex_fwd_val = sign_extend_number(load(memory_p, addr, LENGTH_BYTE), 8);      break; // lb
            case 0x1: ex_fwd_val = sign_extend_number(load(memory_p, addr, LENGTH_HALF_WORD), 16); break; // lh
            case 0x2: ex_fwd_val = load(memory_p, addr, LENGTH_WORD);      break; // lw
            case 0x4: ex_fwd_val = load(memory_p, addr, LENGTH_BYTE);      break; // lbu
            case 0x5: ex_fwd_val = load(memory_p, addr, LENGTH_HALF_WORD); break; // lhu
        }
    }

    if      (pwires_p->fwd_a == 1) idex_fwd.rs1_val = ex_fwd_val;
    else if (pwires_p->fwd_a == 2) idex_fwd.rs1_val = pregs_p->memwb_preg.out.mem_to_reg
                ? pregs_p->memwb_preg.out.mem_data
                : pregs_p->memwb_preg.out.alu_result;
    if      (pwires_p->fwd_b == 1) idex_fwd.rs2_val = ex_fwd_val;
    else if (pwires_p->fwd_b == 2) idex_fwd.rs2_val = pregs_p->memwb_preg.out.mem_to_reg
                ? pregs_p->memwb_preg.out.mem_data
                : pregs_p->memwb_preg.out.alu_result;

    pregs_p->exmem_preg.inp = stage_execute   (idex_fwd, pwires_p);

    pregs_p->memwb_preg.inp = stage_mem       (pregs_p->exmem_preg.out, pwires_p, memory_p, cache_p);

    stage_writeback (pregs_p->memwb_preg.out, pwires_p, regfile_p);

    // Load-use hazard resolution: Hold IF/ID for one cycle
    // Decode has already inserted the bubble
    if (sim_config.fwd_en && pwires_p->stall)
    {
        pregs_p->ifid_preg.inp = pregs_p->ifid_preg.out;
    }

    // flush younger instructions after a branch or jump redirect
    if (sim_config.fwd_en && pwires_p->pcsrc)
    {
        uint32_t ifid_addr  = pregs_p->ifid_preg.inp.instr_addr;   
        uint32_t idex_addr  = pregs_p->idex_preg.inp.instr_addr;   
        uint32_t exmem_addr = pregs_p->exmem_preg.inp.instr_addr;  

        ifid_reg_t  ifid_bubble  = {0};
        ifid_bubble.instr.bits  = 0x00000013;
        ifid_bubble.instr_addr  = ifid_addr;  
        idex_reg_t  idex_bubble  = {0};
        idex_bubble.instr.bits  = 0x00000013;
        idex_bubble.instr_addr  = idex_addr;       
        exmem_reg_t exmem_bubble = {0};
        exmem_bubble.instr.bits = 0x00000013;
        exmem_bubble.instr_addr = exmem_addr; 

        pregs_p->ifid_preg.inp  = ifid_bubble;
        pregs_p->idex_preg.inp  = idex_bubble;
        pregs_p->exmem_preg.inp = exmem_bubble;

#ifndef BRANCH_PREDICT_ENABLED
        // with prediction, taken branches are counted at resolution
        branch_counter++;
#endif

#ifdef DEBUG_CYCLE
        printf("[CPL]: Pipeline Flushed\n");
#endif
    }

    // update all the output registers for the next cycle from the input registers in the current cycle
    pregs_p->ifid_preg.out  = pregs_p->ifid_preg.inp;
    pregs_p->idex_preg.out  = pregs_p->idex_preg.inp;
    pregs_p->exmem_preg.out = pregs_p->exmem_preg.inp;
    pregs_p->memwb_preg.out = pregs_p->memwb_preg.inp;

    /////////////////// NO CHANGES BELOW THIS ARE REQUIRED //////////////////////

    // increment the cycle
    total_cycle_counter++;

#ifdef DEBUG_REG_TRACE
    print_register_trace(regfile_p);
#endif

    /**
     * check ecall condition
     * To do this, the value stored in R[10] (a0 or x10) should be 10.
     * Hence, the ecall condition is checked by the existence of following
     * two instructions in sequence:
     * 1. <instr>  x10, <val1>, <val2>
     * 2. ecall
     *
     * The first instruction must write the value 10 to x10.
     * The second instruction is the ecall (opcode: 0x73)
     *
     * The condition checks whether the R[10] value is 10 when the
     * `memwb_reg.instr.opcode` == 0x73 (to propagate the ecall)
     *
     * If more functionality on ecall needs to be added, it can be done
     * by adding more conditions on the value of R[10]
     */
    if( (pregs_p->memwb_preg.out.instr.bits == 0x00000073) &&
            (regfile_p->R[10] == 10) )
    {
        *(ecall_exit) = true;
    }

#ifdef TWO_LEVEL_CACHE_ENABLE
    // save L1-D counters before riscv.c's local cache goes out of scope
    ms4_dcache_hits   = cache_p->hit_count;
    ms4_dcache_misses = cache_p->miss_count;
#endif
}
