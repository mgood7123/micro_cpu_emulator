#pragma once
#include "../../Shell/builtins/env.h"
#include "../disassembler.h"

char * bit8null = "00000000";

struct opcode {
    uint8_t ins;
    uint8_t ty1;
    uint8_t op1;
    uint8_t ty2;
    uint8_t op2;
    uint8_t ty3;
    uint8_t op3;
    uint8_t nu1;
} * opcode_structure;
int internal_parser_index = 0;

void opcode_structure_write(char * file, char * mode) {
    FILE * t = fopen(file, mode);
    fwrite(opcode_structure, (sizeof(opcode_structure)*internal_parser_index), 1, t);
    fclose(t);
}

void opcode_structure_read(char * file) {
    // shall update the structure and index
    FILE * t = fopen(file, "r");
    fseek(t, 0, SEEK_END);
    size_t len = ftell(t);
    rewind(t);
    internal_parser_index = len/sizeof(opcode_structure);
    if (len % sizeof(opcode_structure) != 0) {
        printf("error: file sector %d is damaged, structure may be incomplete\n", internal_parser_index+1);
    }
    fread(opcode_structure, len, 1, t);
    fclose(t);
}

void opcode_structure_clear(int index) {
    printf("clearing opcode_structure index %d\n", index);
    opcode_structure[index].ins = 0;
    opcode_structure[index].ty1 = 0;
    opcode_structure[index].op1 = 0;
    opcode_structure[index].ty2 = 0;
    opcode_structure[index].op2 = 0;
    opcode_structure[index].ty3 = 0;
    opcode_structure[index].op3 = 0;
}

void opcode_structure_print(int index) {
    printf("printing opcode_structure index %d\n", index);
    printBinLen(opcode_structure[index].ins,8); puts("");
    printBinLen(opcode_structure[index].ty1,8); puts("");
    printBinLen(opcode_structure[index].op1,8); puts("");
    printBinLen(opcode_structure[index].ty2,8); puts("");
    printBinLen(opcode_structure[index].op2,8); puts("");
    printBinLen(opcode_structure[index].ty3,8); puts("");
    printBinLen(opcode_structure[index].op3,8); puts("");
}

void opcode_structure_clear_all(void) {
    for (int i = 0; i < internal_parser_index; i++) opcode_structure_clear(i);
    free(opcode_structure);
}

void opcode_structure_print_all(void) {
    for (int i = 0; i < internal_parser_index; i++) opcode_structure_print(i);
}

char ** table_instructions; char ** table_types; char ** table_registers; char ** table_encoding;

char * type_register = "type_register";
char * type_number = "type_number";
char * type_hex = "type_hex";

void denit_table(char *** table_instructions, char *** table_types, char *** table_registers, char *** table_encoding) {
    env__free(*table_instructions);
    env__free(*table_types);
    env__free(*table_registers);
    env__free(*table_encoding);
}

void init_table(char *** table_instructions, char *** table_types, char *** table_registers, char *** table_encoding) {
    *table_types = env__new();
    *table_registers = env__new();
    *table_encoding = env__new();
    *table_instructions = env__new();
    
    // instructions
    *table_instructions = env__add(*table_instructions, "instruction_start=0");
    *table_instructions = env__add(*table_instructions, "instruction_end=3");
    *table_instructions = env__add(*table_instructions, "instruction_add=\x1");
    *table_instructions = env__add(*table_instructions, "instruction_add3=\x2");
    *table_instructions = env__add(*table_instructions, "instruction_mov=\x3");
    // types
    *table_types = env__add(*table_types, "type_register=\x1");
    *table_types = env__add(*table_types, "type_number=\x2");
    *table_types = env__add(*table_types, "type_hex=\x3");
    
    // registers
    *table_registers = env__add(*table_registers, "register_r1=\x1");
    *table_registers = env__add(*table_registers, "register_r2=\x2");
    *table_registers = env__add(*table_registers, "register_r3=\x3");
    *table_registers = env__add(*table_registers, "register_pc=\x4");

    *table_encoding = env__append_env(*table_encoding, *table_instructions);
    *table_encoding = env__append_env(*table_encoding, *table_types);
    *table_encoding = env__append_env(*table_encoding, *table_registers);
}
