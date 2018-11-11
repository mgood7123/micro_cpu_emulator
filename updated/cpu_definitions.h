#pragma once
int CPU_Endian = 0;
#define E_LITTLE 0
#define E_BIG 1
#define RIGHT_TO_LEFT E_LITTLE
#define LEFT_TO_RIGHT E_BIG

#include "../Shell/builtins/env.h"
#include "../Shell/builtins/printfmacro.h"
#include "../Shell/builtins/clock.h"
#include "../Shell/builtins/regex_str.h"
// #include "../disassembler/disassembler.h"
#include "../disassemblertmp/binary.h"
#include <assert.h>
#ifndef MCASSEMBLER
#define printBinLen(num, len) { \
	for (int i = len-1; i != -1; i--) printf("%d", (num >> i) & 0x1); \
	printf(" = %s", #num); \
}

int swap_(void) {
	unsigned int x = 1;
	return (int) (((char *)&x)[0]);
}

int swap = 0;


int getBit(unsigned char byte, int position) // position in range 0-7
{
    return (byte >> position) & 0x1;
}

int offset = 0;
int bitcount = 0;

unsigned char * readbin_(unsigned char * encoding, int len) {
	unsigned char * bits = malloc(len);
	int ii = 7;
	int was_set = 0;
	
	for (int i = 0; i < len; i++)
	{
		bits[i]=!getBit(*encoding, ii)?'0':'1';
		ii--;
		bitcount++;
		if (bitcount>7) {
			encoding = (encoding)+0x1;
			bitcount = 0;
			was_set = 1;
			ii = 7;
		}
	}
	bits[len] = 0;
	if (was_set == 0 && bitcount<=7) for(int i = 0; i < len; i++) *encoding = *encoding << 0x1;
	was_set = 0;
	return bits;
}

int32_t string_to_bin(int len, char * s) {
	int32_t x = 0;
	for(int i = 0; i < len; i++) x = (x << 1) | (s[i] == '1' ? 1 : 0);
	return x;
}

// not needed
uint64_t string_to_bin64(int len, char * s) {
	uint64_t x = 0;
	for(int i = 0; i < len; i++) x = (x << 1) | (s[i] == '1' ? 1 : 0);
	return x;
}

#endif

char * bit8null = "00000000";
/*
[20:32] <[[]]> could a format be something like this:  instruction, how_many_opcodes_follow, opcodes
[20:34] <[[]]> for example: 00000011 00000010 00010011 00000001 00000001 00000000 00000000  could be changed to 00000011 00000010 00000010 00010011 00000001 00000001
[20:36] <[[]]> where it would be read as instruction = 00000011, opcode_count = 00000010, op1 = 00000010 00010011, op2 = 00000001 00000001
*/

struct op {
	uint8_t type;
	uint8_t opcode;
};

struct opcode {
	uint8_t ins;
	uint8_t opcode_count;
	struct op * opcode;
};

int internal_parser_index = 0;

void initopcode(struct opcode * opcode_structure) {
	if (opcode_structure == NULL) {
		puts("initializing opcode structure");
		opcode_structure = malloc(50*sizeof(struct opcode));
		memset(opcode_structure, 0, 50*sizeof(struct opcode));
	}
	for (int i = 0; i < 50; i++) if (opcode_structure[i].opcode == NULL) {
		printf("initializing opcode sub structure %d/%d\n", i, 50);
		opcode_structure[i].opcode = malloc(50*sizeof(struct op));
		memset(opcode_structure[i].opcode, 0, 50*sizeof(struct op));
	}
}

struct ISA {
	struct opcode * opcode_structure;
	int word_size;
	int max_words_per_cycle;
	int word_split;
	int max_instruction_size;
};

struct ISA * init_ISA_Intel_4004(void) {
	puts("initializing ISA structure");
	struct ISA Intel4004 = {0};
	memset(&Intel4004, 0, sizeof(struct ISA));
	Intel4004.word_size = 4;
	Intel4004.max_words_per_cycle = 2;
	Intel4004.word_split = 4;
	Intel4004.max_instruction_size = 8;
	initopcode(Intel4004.opcode_structure);
	return &Intel4004;
}

void opcode_structure_opcode_clear(int index, int sub_index) {
	printf("clearing opcode_structure[%d].opcode index %d\n", index, sub_index);
	opcode_structure[index].opcode[sub_index].type = 0x0;
	opcode_structure[index].opcode[sub_index].opcode = 0x0;
	if (sub_index == opcode_structure[index].opcode_count-1 && sub_index != 0) opcode_structure[index].opcode_count--;
}

void opcode_structure_clear(int index) {
	printf("clearing opcode_structure index %d\n", index);
	opcode_structure[index].ins = 0;
	for (int i = 0; i < opcode_structure[index].opcode_count; i++) opcode_structure_opcode_clear(index, i);
	if (index == internal_parser_index-1 && index != 0) internal_parser_index--;
}

void opcode_structure_clear_all(void) {
	for (int i = 0; i < internal_parser_index; i++) opcode_structure_clear(i);
	free(opcode_structure);
	opcode_structure = NULL;
	internal_parser_index = 0;
}

void opcode_structure_opcode_print(int index, int sub_index) {
	printf("printing opcode_structure[%d].opcode index %d\n", index, sub_index);
	printBinLen(opcode_structure[index].opcode[sub_index].type,8); puts("");
	printBinLen(opcode_structure[index].opcode[sub_index].opcode,8); puts("");
}

void opcode_structure_print(int index) {
	printf("printing opcode_structure index %d\n", index);
	printBinLen(opcode_structure[index].ins,8); puts("");
	for (int i = 0; i < opcode_structure[index].opcode_count; i++) opcode_structure_opcode_print(index, i);
}

void opcode_structure_print_all(void) {
	for (int i = 0; i < internal_parser_index; i++) opcode_structure_print(i);
}

void opcode_structure_opcode_write(FILE * t, int index, int sub_index) {
	printf("writing opcode_structure[%d].opcode index %d\n", index, sub_index);
	fwrite(&opcode_structure[index].opcode[sub_index].type, 1, 1, t);
	fwrite(&opcode_structure[index].opcode[sub_index].opcode, 1, 1, t);
}

void opcode_structure_write_one(FILE * t, int index) {
	printf("writing opcode_structure index %d\n", index);
	fwrite(&opcode_structure[index].ins, 1, 1, t);
	fwrite(&opcode_structure[index].opcode_count, 1, 1, t);
	for (int i = 0; i < opcode_structure[index].opcode_count; i++) opcode_structure_opcode_write(t, index, i);
}

void opcode_structure_write_all(FILE * t) {
	for (int i = 0; i < internal_parser_index; i++) opcode_structure_write_one(t, i);
}

void opcode_structure_write(char * file, char * mode) {
	FILE * t = fopen(file, mode);
// 	fwrite(opcode_structure, sizeof(*opcode_structure)*internal_parser_index, 1, t);
	opcode_structure_write_all(t);
	fclose(t);
}

void invalid_sector(int sector) {
	printf("error: file sector %d is damaged, structure may be incomplete\n", sector);
}

int opcode_structure_opcode_read(FILE * t, int index, int sub_index, int * sector) {
	printf("reading opcode_structure[%d].opcode index %d\n", index, sub_index);
	fread(&opcode_structure[index].opcode[sub_index].type, 1, 1, t); if (feof(t) || ferror(t)) return -2;
	(*sector)++;
	fread(&opcode_structure[index].opcode[sub_index].opcode, 1, 1, t); if (feof(t) || ferror(t)) return -2;
	(*sector)++;
	return 0;
}

int opcode_structure_read_one(FILE * t, int index, int * sector) {
	printf("attempting to read opcode_structure index %d\n", index);
	fread(&opcode_structure[index].ins, 1, 1, t); if (feof(t) || ferror(t)) return -1;
	(*sector)++;
	fread(&opcode_structure[index].opcode_count, 1, 1, t); if (feof(t) || ferror(t)) return -2;
	(*sector)++;
	for (int i = 0; i < opcode_structure[index].opcode_count; i++) {
		int r = opcode_structure_opcode_read(t, index, i, sector);
		if (r) return r;
	}
	return 0;
}

int opcode_structure_read_all(FILE * t) {
	int stable = 0;
	int sector = 0;
	while(!stable) {
		stable = opcode_structure_read_one(t, internal_parser_index, &sector);
		if (!stable) internal_parser_index++;
	}
	if (stable == -1) puts("reached End Of File");
	if (stable == -2) invalid_sector(sector);
	return stable;
}

void opcode_structure_read(char * file) {
	// shall update the structure and index
	if (opcode_structure) opcode_structure_clear_all();
	initopcode();
	FILE * t = fopen(file, "r");
	fseek(t, 0, SEEK_END);
	size_t len = ftell(t);
	assert(len!=0);
	rewind(t);
	opcode_structure_read_all(t);
	fclose(t);
}


// void opcode_structure_read(char * file) {
// 	// shall update the structure and index
// 	if (opcode_structure) opcode_structure_clear_all();
// 	initopcode();
// 	FILE * t = fopen(file, "r");
// 	fseek(t, 0, SEEK_END);
// 	size_t len = ftell(t);
// 	assert(len!=0);
// 	rewind(t);
// 	internal_parser_index = len/sizeof(*opcode_structure);
// 	if (len % sizeof(*opcode_structure) != 0) {
// 		printf("error: file sector %d is damaged, structure may be incomplete\n", internal_parser_index+1);
// 	}
// 	fread(opcode_structure, sizeof(*opcode_structure), internal_parser_index, t);
// 	fclose(t);
// }

char ** table_instructions; char ** table_types; char ** table_registers; char ** table_encoding;

char * type_register = "type_register";
char * type_number = "type_number";
char * type_hex = "type_hex";

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
/* C99-style: anonymous argument referenced by __VA_ARGS__, empty arg not OK */
# define add_table(...) add_table_h(N_ARGS(__VA_ARGS__), __VA_ARGS__)
# define add_table_instructions(...) add_table_instructions_h(N_ARGS(__VA_ARGS__), __VA_ARGS__)
#elif defined(__GNUC__)
/* GCC-style: named argument, empty arg is OK */
# define add_table(args...) add_table_h(N_ARGS(args), args)
# define add_table_instructions(args...) add_table_instructions_h(N_ARGS(args), args)
#endif

void add_table_h(unsigned int n_args, char *** table, ...) {
    pi(n_args);
    va_list ap2;
    va_start(ap2, table);
    for (int i = 0; i < n_args-1; i++) {
        char * inst = va_arg(ap2, char*);
        int hex_encoding = va_arg(ap2, int);
        int len = strlen(inst)+sizeof(hex_encoding)+1;
        char * val = malloc(len);
        memset(val, 0, len);
        sprintf(val, "%s=", inst);
        val[strlen(val)] = hex_encoding;
        *table = env__add(*table, val);
        i++;
    }
    va_end(ap2);
}

#define ET_INT 1
#define ET_CHAR 2
#define ET_STRING 3
#define ET_BINARY 4

void add_table_instructions_h(unsigned int n_args, char *** table, struct instruction ** instruction_bank, ...) {
	CPU_Endian=E_LITTLE;
    pi(n_args);
    va_list ap2;
    va_start(ap2, instruction_bank);
    for (int i = 0; i < n_args-2; i++) {
        char * inst = va_arg(ap2, char*);
        int encoding_type = va_arg(ap2, int);
        i++;
		if (encoding_type == ET_INT) {
			int hex_encoding = va_arg(ap2, int);
			i++;
			int len = strlen(inst)+sizeof(hex_encoding)+1;
			char * val = malloc(len);
			memset(val, 0, len);
			sprintf(val, "%s=", inst);
			val[strlen(val)] = hex_encoding;
			*table = env__add(*table, val);
		}
		else if (encoding_type == ET_BINARY) {
			char * binary = va_arg(ap2, char*);
			i++;
			ps(binary);
			str_new(binary_);
			str_new(binary_tmp);
			str_insert_string(binary_tmp, binary);
			str_insert_string(binary_, binary_tmp.no_spaces.string);
			str_info(binary_);
			struct binbuf binstr = binbuf_init(BINMODE_APPEND);
			swap = swap_();
			pi(swap)
			printf("Endian: %s\n", swap==1?"Big":"Little");
			pi(CPU_Endian)
			printf("CPU_Endian: %s\n", CPU_Endian==1?"Big":"Little");
			
			if (swap == E_LITTLE && CPU_Endian == E_LITTLE) read__binary(&binstr, binary_.string);
			if (swap == E_LITTLE && CPU_Endian == E_BIG) read__binary(&binstr, binary_.reverse.string);
			if (swap == E_BIG && CPU_Endian == E_LITTLE) read__binary(&binstr, binary_.reverse.string);
			if (swap == E_BIG && CPU_Endian == E_BIG) read__binary(&binstr, binary_.string);
			
			pi(*binstr.data->s)
			
			int len = strlen(inst)+sizeof(int)+1;
			char * val = malloc(len);
			memset(val, 0, len);
			sprintf(val, "%s=", inst);
			val[strlen(val)] = *binstr.data->s;
			*table = env__add(*table, val);
			
			str_free(binary_);
			str_free(binary_tmp);
// 			exit(0);
		}
        instruction_add(instruction_bank, strdup(inst));
		char * operand = va_arg(ap2, char*); i++;
		char * operation = va_arg(ap2, char*); i++;
        if (operand) instruction_add_operands(instruction_bank, strdup(inst), strdup(operand)); // if this step is omitted all registers in the operations shall become IMPLICIT
        if (operation) instruction_add_operations(instruction_bank, strdup(inst), strdup(operation));
        instruction_finalize(instruction_bank);
    }
    va_end(ap2);
}
