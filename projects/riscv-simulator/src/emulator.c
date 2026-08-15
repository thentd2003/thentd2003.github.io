#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "types.h"
#include "utils.h"
#include "riscv.h"

void execute_rtype(Instruction, Processor *);
void execute_itype_except_load(Instruction, Processor *);
void execute_branch(Instruction, Processor *);
void execute_jal(Instruction, Processor *);
void execute_load(Instruction, Processor *, Byte *);
void execute_store(Instruction, Processor *, Byte *);
void execute_ecall(Processor *, Byte *);
void execute_lui(Instruction, Processor *);

void execute_instruction(uint32_t instruction_bits, Processor *processor,Byte *memory) {
    Instruction instruction = parse_instruction(instruction_bits);
    switch(instruction.opcode) {
        case 0x33:
            execute_rtype(instruction, processor);
            break;
        case 0x13:
            execute_itype_except_load(instruction, processor);
            break;
        case 0x73:
            execute_ecall(processor, memory);
            break;
        case 0x63:
            execute_branch(instruction, processor);
            break;
        case 0x6F:
            execute_jal(instruction, processor);
            break;
        case 0x23:
            execute_store(instruction, processor, memory);
            break;
        case 0x03:
            execute_load(instruction, processor, memory);
            break;
        case 0x37:
            execute_lui(instruction, processor);
            break;
        default: // undefined opcode
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_rtype(Instruction instruction, Processor *processor) {
    switch (instruction.rtype.funct3){
        case 0x0:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    // Add
                    processor->R[instruction.rtype.rd] =
                        ((sWord)processor->R[instruction.rtype.rs1]) +
                        ((sWord)processor->R[instruction.rtype.rs2]);
                    break;
                case 0x1:
                    // Mul
                    processor->R[instruction.rtype.rd] =
                        ((sWord)processor->R[instruction.rtype.rs1]) *
                        ((sWord)processor->R[instruction.rtype.rs2]);
                    break;
                case 0x20:
                    // Sub
                    processor->R[instruction.rtype.rd] =
                        ((sWord)processor->R[instruction.rtype.rs1]) -
                        ((sWord)processor->R[instruction.rtype.rs2]);
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                    break;
            }
            break;
        /* YOUR CODE HERE */
    /* deal with other cases */
        case 0x1:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    // Sll
                    processor->R[instruction.rtype.rd] =
                        processor->R[instruction.rtype.rs1] << (processor->R[instruction.rtype.rs2] & 0x1F);
                    break;
                case 0x1:
                    // Mulh: upper 32 bits of the 64-bit signed product
                    processor->R[instruction.rtype.rd] =
                        (Word)(((sDouble)(sWord)processor->R[instruction.rtype.rs1] *
                                (sDouble)(sWord)processor->R[instruction.rtype.rs2]) >> 32);
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                    break;
            }
            break;
        case 0x2:
            // Slt
            processor->R[instruction.rtype.rd] =
                ((sWord)processor->R[instruction.rtype.rs1] < (sWord)processor->R[instruction.rtype.rs2]) ? 1 : 0;
            break;
        case 0x3:
            // Sltu
            processor->R[instruction.rtype.rd] =
                (processor->R[instruction.rtype.rs1] < processor->R[instruction.rtype.rs2]) ? 1 : 0;
            break;
        case 0x4:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    // Xor
                    processor->R[instruction.rtype.rd] =
                        processor->R[instruction.rtype.rs1] ^ processor->R[instruction.rtype.rs2];
                    break;
                case 0x1:
                    // Div
                    if (processor->R[instruction.rtype.rs2] == 0)
                        processor->R[instruction.rtype.rd] = 0xFFFFFFFF;
                    else if (processor->R[instruction.rtype.rs1] == 0x80000000 &&
                             processor->R[instruction.rtype.rs2] == 0xFFFFFFFF)
                        processor->R[instruction.rtype.rd] = 0x80000000;
                    else
                        processor->R[instruction.rtype.rd] =
                            ((sWord)processor->R[instruction.rtype.rs1]) /
                            ((sWord)processor->R[instruction.rtype.rs2]);
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                    break;
            }
            break;
        case 0x5:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    // Srl
                    processor->R[instruction.rtype.rd] =
                        processor->R[instruction.rtype.rs1] >> (processor->R[instruction.rtype.rs2] & 0x1F);
                    break;
                case 0x20:
                    // Sra
                    processor->R[instruction.rtype.rd] =
                        (sWord)processor->R[instruction.rtype.rs1] >> (processor->R[instruction.rtype.rs2] & 0x1F);
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                    break;
            }
            break;
        case 0x6:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    // Or
                    processor->R[instruction.rtype.rd] =
                        processor->R[instruction.rtype.rs1] | processor->R[instruction.rtype.rs2];
                    break;
                case 0x1:
                    // Rem
                    if (processor->R[instruction.rtype.rs2] == 0)
                        processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs1];
                    else if (processor->R[instruction.rtype.rs1] == 0x80000000 &&
                             processor->R[instruction.rtype.rs2] == 0xFFFFFFFF)
                        processor->R[instruction.rtype.rd] = 0;
                    else
                        processor->R[instruction.rtype.rd] =
                            ((sWord)processor->R[instruction.rtype.rs1]) %
                            ((sWord)processor->R[instruction.rtype.rs2]);
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                    break;
            }
            break;
        case 0x7:
            // And
            processor->R[instruction.rtype.rd] =
                processor->R[instruction.rtype.rs1] & processor->R[instruction.rtype.rs2];
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
    // update PC
    processor->PC += 4;
}

void execute_itype_except_load(Instruction instruction, Processor *processor) {
    int imm = sign_extend_number(instruction.itype.imm, 12);
    int shamt = instruction.itype.imm & 0x1F;
    int funct7 = (instruction.itype.imm >> 5) & 0x7F;
    switch (instruction.itype.funct3) {
        /* YOUR CODE HERE */
        case 0x0:
            // Addi
            processor->R[instruction.itype.rd] = (sWord)processor->R[instruction.itype.rs1] + imm;
            break;
        case 0x1:
            // Slli
            processor->R[instruction.itype.rd] = processor->R[instruction.itype.rs1] << shamt;
            break;
        case 0x2:
            // Slti
            processor->R[instruction.itype.rd] =
                ((sWord)processor->R[instruction.itype.rs1] < imm) ? 1 : 0;
            break;
        case 0x3:
            // Sltiu
            processor->R[instruction.itype.rd] =
                (processor->R[instruction.itype.rs1] < (Word)imm) ? 1 : 0;
            break;
        case 0x4:
            // Xori
            processor->R[instruction.itype.rd] = processor->R[instruction.itype.rs1] ^ imm;
            break;
        case 0x5:
            if (funct7 == 0x20) {
                // Srai
                processor->R[instruction.itype.rd] = (sWord)processor->R[instruction.itype.rs1] >> shamt;
            } else {
                // Srli
                processor->R[instruction.itype.rd] = processor->R[instruction.itype.rs1] >> shamt;
            }
            break;
        case 0x6:
            // Ori
            processor->R[instruction.itype.rd] = processor->R[instruction.itype.rs1] | imm;
            break;
        case 0x7:
            // Andi
            processor->R[instruction.itype.rd] = processor->R[instruction.itype.rs1] & imm;
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
    processor->PC += 4;
}

void execute_ecall(Processor *p, Byte *memory) {
    Register i;
    
    // syscall number is given by a0 (x10)
    // argument is given by a1
    switch(p->R[10]) {
        case 1: // print an integer
            printf("%d",p->R[11]);
            p->PC += 4;
            break;
        case 4: // print a string
            for(i=p->R[11];i<MEMORY_SPACE && load(memory,i,LENGTH_BYTE);i++) {
                printf("%c",load(memory,i,LENGTH_BYTE));
            }
            p->PC += 4;
            break;
        case 10: // exit
            printf("exiting the simulator\n");
            exit(0);
            break;
        case 11: // print a character
            printf("%c",p->R[11]);
            p->PC += 4;
            break;
        default: // undefined ecall
            printf("Illegal ecall number %d\n", p->R[10]);
            exit(-1);
            break;
    }
}

void execute_branch(Instruction instruction, Processor *processor) {
    int taken = 0;
    sWord rs1_signed = (sWord)processor->R[instruction.sbtype.rs1];
    sWord rs2_signed = (sWord)processor->R[instruction.sbtype.rs2];
    Word rs1_unsigned = processor->R[instruction.sbtype.rs1];
    Word rs2_unsigned = processor->R[instruction.sbtype.rs2];
    switch (instruction.sbtype.funct3) {
        /* YOUR CODE HERE */
        case 0x0:
            // Beq
            taken = (rs1_signed == rs2_signed);
            break;
        case 0x1:
            // Bne
            taken = (rs1_signed != rs2_signed);
            break;
        case 0x4:
            // Blt
            taken = (rs1_signed < rs2_signed);
            break;
        case 0x5:
            // Bge
            taken = (rs1_signed >= rs2_signed);
            break;
        case 0x6:
            // Bltu
            taken = (rs1_unsigned < rs2_unsigned);
            break;
        case 0x7:
            // Bgeu
            taken = (rs1_unsigned >= rs2_unsigned);
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
    if (taken) {
        processor->PC += get_branch_offset(instruction);
    } else {
        processor->PC += 4;
    }
}

void execute_load(Instruction instruction, Processor *processor, Byte *memory) {
    int offset = sign_extend_number(instruction.itype.imm, 12);
    Address addr = processor->R[instruction.itype.rs1] + offset;
    switch (instruction.itype.funct3) {
        /* YOUR CODE HERE */
        case 0x0:
            // Lb
            processor->R[instruction.itype.rd] =
                sign_extend_number(load(memory, addr, LENGTH_BYTE), 8);
            break;
        case 0x1:
            // Lh
            processor->R[instruction.itype.rd] =
                sign_extend_number(load(memory, addr, LENGTH_HALF_WORD), 16);
            break;
        case 0x2:
            // Lw
            processor->R[instruction.itype.rd] = load(memory, addr, LENGTH_WORD);
            break;
        case 0x4:
            // Lbu
            processor->R[instruction.itype.rd] = load(memory, addr, LENGTH_BYTE);
            break;
        case 0x5:
            // Lhu
            processor->R[instruction.itype.rd] = load(memory, addr, LENGTH_HALF_WORD);
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
    processor->PC += 4;
}

void execute_store(Instruction instruction, Processor *processor, Byte *memory) {
    int offset = get_store_offset(instruction);
    Address addr = processor->R[instruction.stype.rs1] + offset;
    Word value = processor->R[instruction.stype.rs2];
    switch (instruction.stype.funct3) {
        /* YOUR CODE HERE */
        case 0x0:
            // Sb
            store(memory, addr, LENGTH_BYTE, value);
            break;
        case 0x1:
            // Sh
            store(memory, addr, LENGTH_HALF_WORD, value);
            break;
        case 0x2:
            // Sw
            store(memory, addr, LENGTH_WORD, value);
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
    processor->PC += 4;
}

void execute_jal(Instruction instruction, Processor *processor) {
    /* YOUR CODE HERE */
    Address return_addr = processor->PC + 4;
    int offset = get_jump_offset(instruction);
    processor->R[instruction.ujtype.rd] = return_addr;
    processor->PC += offset;
}

void execute_lui(Instruction instruction, Processor *processor) {
    /* YOUR CODE HERE */
    processor->R[instruction.utype.rd] = instruction.utype.imm << 12;
    processor->PC += 4;
}

void store(Byte *memory, Address address, Alignment alignment, Word value) {
    /* YOUR CODE HERE */
    if (alignment == LENGTH_BYTE) {
        memory[address] = value & 0xFF;
    } else if (alignment == LENGTH_HALF_WORD) {
        memory[address] = value & 0xFF;
        memory[address + 1] = (value >> 8) & 0xFF;
    } else if (alignment == LENGTH_WORD) {
        memory[address] = value & 0xFF;
        memory[address + 1] = (value >> 8) & 0xFF;
        memory[address + 2] = (value >> 16) & 0xFF;
        memory[address + 3] = (value >> 24) & 0xFF;
    } else {
        printf("Error: Unrecognized alignment %d\n", alignment);
        exit(-1);
    }
}

Word load(Byte *memory, Address address, Alignment alignment) {
    if(alignment == LENGTH_BYTE) {
        return memory[address];
    } else if(alignment == LENGTH_HALF_WORD) {
        return (memory[address+1] << 8) + memory[address];
    } else if(alignment == LENGTH_WORD) {
        return (memory[address+3] << 24) + (memory[address+2] << 16)
               + (memory[address+1] << 8) + memory[address];
    } else {
        printf("Error: Unrecognized alignment %d\n", alignment);
        exit(-1);
    }
}
