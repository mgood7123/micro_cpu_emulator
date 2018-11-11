// #include <gc.h>
#include "cpu_language.h"
void init_table_bank(char *** table_instructions, char *** table_types, char *** table_registers, char *** table_encoding, struct instruction ** instruction_bank) {
    *table_types = env__new();
    *table_registers = env__new();
    *table_encoding = env__new();
    *table_instructions = env__new();
    
    // instructions
	add_table_instructions(
        table_instructions, instruction_bank,
        "NOP", ET_BINARY, "0000",
		NULL,
		NULL,

        "JCN", ET_BINARY, "0001",
		NULL,
		NULL
	);
    add_table(
        table_types,
        "type_register", 0x1,
        "type_number", 0x2,
        "type_hex", 0x3
	);
   	add_table(
        table_registers,
        "r1", 0x1,
        "r2", 0x2,
        "r3", 0x3,
        "pc", 0x4
	);
    *table_encoding = env__append_env(*table_encoding, *table_instructions);
    *table_encoding = env__append_env(*table_encoding, *table_types);
    *table_encoding = env__append_env(*table_encoding, *table_registers);
}

int main()
{
    char ** table_instructions;
    char ** table_types;
    char ** table_registers;
    char ** table_encoding;
    instruction_new_bank(test);
	
    init_table_bank(&table_instructions, &table_types, &table_registers, &table_encoding, &test);
	
    parse(table_encoding, "NOP");
    parse(table_encoding, "JCN");
	
    opcode_structure_print_all();
	
    puts("writing from opcode_structure to testcpuwrite");
    opcode_structure_write("testcpuwrite", "w");
    puts("reading from testcpuwrite into opcode_structure");
    opcode_structure_read("testcpuwrite");
	system("xxd -b -c7 ./testcpuwrite");
    opcode_structure_print_all();
    cpu_decode(table_encoding, table_types, table_registers, test, 0);
    cpu_decode(table_encoding, table_types, table_registers, test, 1);
	
    opcode_structure_clear_all();
    denit_table_bank(&table_instructions, &table_types, &table_registers, &table_encoding, &test);
    return 0;
}
