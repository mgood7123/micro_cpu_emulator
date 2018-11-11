#include "varg.h"
#include "cpu_language.h"
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#define CPU_TYPE unsigned int
#define CPU_TYPE_ADD 1
#define CPU_POWER_ON true
#define CPU_POWER_OFF false
#define null_int(x) x = 0
#define null_custom(x, y) x = y

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
        "r0", 0x1,
        "r1", 0x2,
        "r2", 0x3,
        "r3", 0x4,
        "r4", 0x5,
        "r5", 0x6,
        "r6", 0x7,
        "r7", 0x8,
        "r8", 0x9,
        "r9", 0x10,
        "r10", 0x11,
        "r11", 0x12,
        "r12", 0x13,
        "r13", 0x14,
        "r14", 0x15,
        "r15", 0x16,
        "pc", 0x17
	);
    *table_encoding = env__append_env(*table_encoding, *table_instructions);
    *table_encoding = env__append_env(*table_encoding, *table_types);
    *table_encoding = env__append_env(*table_encoding, *table_registers);
}


struct virtual_cpu_register_bank {
    int r0;
    int r1;
    int r2;
    int r3;
    int r4;
    int r5;
    int r6;
    int r7;
    int r8;
    int r9;
    int r10;
    int r11;
    int r12;
    int r13;
    int r14;
    int r15;
	int accumulator;
	int carry;
	int8_t data_pointer;
	void * return_stack;
    int32_t pc;
    char * name;
    CPU_TYPE type;
};

struct virtual_cpu_sub_core {
    struct virtual_cpu_register_bank virtual_register;
    int op;
    int status;
    char * name;
};

struct virtual_cpu_core {
    struct virtual_cpu_sub_core previous;
    struct virtual_cpu_sub_core current;
    int status;
    int power;
    char * cpu_power_state[2];
    char * name;
    bool init;
};

struct virtual_cpu {
    int core_count;
    struct virtual_cpu_core *core;
    struct Queue *q;
    char ** table_instructions;
    char ** table_types;
    char ** table_registers;
    char ** table_encoding;
    struct instruction * instruction_bank;
    char * name;
} cpu_default;

void virtual_cpu_check(struct virtual_cpu *cpu);
void cpu_register_info(struct virtual_cpu *cpu, struct virtual_cpu_core *core, struct virtual_cpu_sub_core *sub_core, struct virtual_cpu_register_bank *virtual_register);
void cpu_sub_core_info(struct virtual_cpu *cpu, struct virtual_cpu_core *core, struct virtual_cpu_sub_core *sub_core);
void cpu_core_info(struct virtual_cpu *cpu, struct virtual_cpu_core *core);
void cpu_info(struct virtual_cpu *cpu);
void cpu_core_start(struct virtual_cpu_core *core);
void cpu_start(struct virtual_cpu *cpu);
void cpu_core_shutdown(struct virtual_cpu_core *core);
void cpu_shutdown(struct virtual_cpu *cpu);
struct QNode* newNode(char * string);
struct Queue *createQueue();
void store_asm(struct Queue *q, char * string);
struct QNode * load_asm(struct Queue *q);
int cpu_queue_add(char * string);
void virtual_cpu_register_check(struct virtual_cpu_register_bank *reg, char * name);
void virtual_cpu_sub_core_check(struct virtual_cpu_sub_core * core, char * name);
void virtual_cpu_core_check(struct virtual_cpu_core *core, char * name);
void cpu_execute(struct virtual_cpu *cpu);
void cpu_deinit(struct virtual_cpu *cpu);
void cpu_core_init(struct virtual_cpu *cpu, int max_cores);


void cpu_register_info(struct virtual_cpu *cpu, struct virtual_cpu_core *core, struct virtual_cpu_sub_core *sub_core, struct virtual_cpu_register_bank *virtual_register) {
    printf("VIRTUAL CPU: INFO : %s : %s : %s : register bank name: %s\n", cpu->name, core->name, sub_core->name, virtual_register->name);
    printf("VIRTUAL CPU: INFO : %s : %s : %s : size of %s: %zu\n", cpu->name, core->name, sub_core->name, virtual_register->name, sizeof(*virtual_register));
    printf("VIRTUAL CPU: INFO : %s : %s : %s : %s : r0: %d\n", cpu->name, core->name, sub_core->name, virtual_register->name, virtual_register->r0);
    printf("VIRTUAL CPU: INFO : %s : %s : %s : %s : r1: %d\n", cpu->name, core->name, sub_core->name, virtual_register->name, virtual_register->r1);
    printf("VIRTUAL CPU: INFO : %s : %s : %s : %s : r2: %d\n", cpu->name, core->name, sub_core->name, virtual_register->name, virtual_register->r2);
    printf("VIRTUAL CPU: INFO : %s : %s : %s : %s : r3: %d\n", cpu->name, core->name, sub_core->name, virtual_register->name, virtual_register->r3);
    printf("VIRTUAL CPU: INFO : %s : %s : %s : %s : r4: %d\n", cpu->name, core->name, sub_core->name, virtual_register->name, virtual_register->r4);
    printf("VIRTUAL CPU: INFO : %s : %s : %s : %s : r5: %d\n", cpu->name, core->name, sub_core->name, virtual_register->name, virtual_register->r5);
    printf("VIRTUAL CPU: INFO : %s : %s : %s : %s : r6: %d\n", cpu->name, core->name, sub_core->name, virtual_register->name, virtual_register->r6);
    printf("VIRTUAL CPU: INFO : %s : %s : %s : %s : r7: %d\n", cpu->name, core->name, sub_core->name, virtual_register->name, virtual_register->r7);
    printf("VIRTUAL CPU: INFO : %s : %s : %s : %s : r8: %d\n", cpu->name, core->name, sub_core->name, virtual_register->name, virtual_register->r8);
    printf("VIRTUAL CPU: INFO : %s : %s : %s : %s : r9: %d\n", cpu->name, core->name, sub_core->name, virtual_register->name, virtual_register->r9);
    printf("VIRTUAL CPU: INFO : %s : %s : %s : %s : r10: %d\n", cpu->name, core->name, sub_core->name, virtual_register->name, virtual_register->r10);
    printf("VIRTUAL CPU: INFO : %s : %s : %s : %s : r11: %d\n", cpu->name, core->name, sub_core->name, virtual_register->name, virtual_register->r11);
    printf("VIRTUAL CPU: INFO : %s : %s : %s : %s : r12: %d\n", cpu->name, core->name, sub_core->name, virtual_register->name, virtual_register->r12);
    printf("VIRTUAL CPU: INFO : %s : %s : %s : %s : r13: %d\n", cpu->name, core->name, sub_core->name, virtual_register->name, virtual_register->r13);
    printf("VIRTUAL CPU: INFO : %s : %s : %s : %s : r14: %d\n", cpu->name, core->name, sub_core->name, virtual_register->name, virtual_register->r14);
    printf("VIRTUAL CPU: INFO : %s : %s : %s : %s : r15: %d\n", cpu->name, core->name, sub_core->name, virtual_register->name, virtual_register->r15);
    printf("VIRTUAL CPU: INFO : %s : %s : %s : %s : accumulator: %d\n", cpu->name, core->name, sub_core->name, virtual_register->name, virtual_register->accumulator);
    printf("VIRTUAL CPU: INFO : %s : %s : %s : %s : carry: %d\n", cpu->name, core->name, sub_core->name, virtual_register->name, virtual_register->carry);
    printf("VIRTUAL CPU: INFO : %s : %s : %s : %s : data_pointer: %d\n", cpu->name, core->name, sub_core->name, virtual_register->name, virtual_register->data_pointer);
    printf("VIRTUAL CPU: INFO : %s : %s : %s : %s : pc: %d\n", cpu->name, core->name, sub_core->name, virtual_register->name, virtual_register->pc);
}

void cpu_sub_core_info(struct virtual_cpu *cpu, struct virtual_cpu_core *core, struct virtual_cpu_sub_core *sub_core) {
    printf("VIRTUAL CPU: INFO : %s : %s : sub core name: %s\n", cpu->name, core->name, sub_core->name);
    printf("VIRTUAL CPU: INFO : %s : %s : size of %s: %zu\n", cpu->name, core->name, sub_core->name, sizeof(*sub_core));
    cpu_register_info(cpu, core, sub_core, &sub_core->virtual_register);
}

void cpu_core_info(struct virtual_cpu *cpu, struct virtual_cpu_core *core) {
    printf("VIRTUAL CPU: INFO : %s : core name:  %s\n", cpu->name, core->name);
    printf("VIRTUAL CPU: INFO : %s : size of %s: %zu\n", cpu->name, core->name, 
           sizeof(*core)-
           sizeof(core->cpu_power_state)+
           (
               sizeof(core->cpu_power_state)*env__size(core->cpu_power_state)
           )
    );
    printf("VIRTUAL CPU: INFO : %s : %s : cpu power status: %s\n", cpu->name, core->name, core->cpu_power_state[core->power]);
    printf("VIRTUAL CPU: INFO : %s : %s : status:           %d\n", cpu->name, core->name, core->status);
    cpu_sub_core_info(cpu, core, &core->previous);
    cpu_sub_core_info(cpu, core, &core->current);
}

void cpu_info(struct virtual_cpu *cpu) {
    virtual_cpu_check(cpu);
    printf("VIRTUAL CPU: INFO : cpu name:         %s\n", cpu->name);
    printf("VIRTUAL CPU: INFO : cpu cores:        %d\n", cpu->core_count);
    printf
    (
        "VIRTUAL CPU: INFO : size of %s: %zu\n", cpu->name,
        (
            sizeof(*cpu)-
            (
                sizeof(cpu->q)+
                sizeof(cpu->core)+
                sizeof(cpu->table_instructions)+
                sizeof(cpu->table_types)+
                sizeof(cpu->table_registers)+
                sizeof(cpu->table_encoding)+
                sizeof(cpu->instruction_bank)
            )+
            (
                (
                    sizeof(cpu->core[0])-
                    sizeof(cpu->core[0].cpu_power_state)+
                    (
                        sizeof(cpu->core[0].cpu_power_state)*env__size(cpu->core[0].cpu_power_state)
                    )
                ) * cpu->core_count
            )
        ) +
        (
            sizeof(cpu->table_instructions) * env__size(cpu->table_instructions)+
            sizeof(cpu->table_types) *env__size(cpu->table_types)+
            sizeof(cpu->table_registers) * env__size(cpu->table_registers)+
            sizeof(cpu->table_encoding) * env__size(cpu->table_encoding)+
            sizeof(*cpu->instruction_bank) * env__size(&cpu->instruction_bank)
        )
    );
    printf("VIRTUAL CPU: INFO : size of instruction table: %zu\n", sizeof(cpu->table_instructions)  * env__size(cpu->table_instructions));
    printf("VIRTUAL CPU: INFO : size of types table:       %zu\n", sizeof(cpu->table_types)         * env__size(cpu->table_types));
    printf("VIRTUAL CPU: INFO : size of registers table:   %zu\n", sizeof(cpu->table_registers)     * env__size(cpu->table_registers));
    printf("VIRTUAL CPU: INFO : size of encoding table:    %zu\n", sizeof(cpu->table_encoding)      * env__size(cpu->table_encoding));
    printf("VIRTUAL CPU: INFO : size of instruction bank:  %zu\n", sizeof(*cpu->instruction_bank)   * env__size(&cpu->instruction_bank));
    for (int i = 0; i < 1; i++) cpu_core_info(cpu, &cpu->core[i]);
}

void cpu_core_start(struct virtual_cpu_core *core) {
    printf("VIRTUAL CPU: starting virtual cpu core:      %s\n", core->name);
    core->power = CPU_POWER_ON;
}

void cpu_start(struct virtual_cpu *cpu) {
    puts("VIRTUAL CPU: performing startup check");
    virtual_cpu_check(cpu);
    puts("VIRTUAL CPU: startup check complete");
    printf("VIRTUAL CPU: starting virtual cpu:      %s\n", cpu->name);
    Expression_Assign("r0", &cpu->core[0].current.virtual_register.r0);
    Expression_Assign("r1", &cpu->core[0].current.virtual_register.r1);
    Expression_Assign("r2", &cpu->core[0].current.virtual_register.r2);
    Expression_Assign("r3", &cpu->core[0].current.virtual_register.r3);
    Expression_Assign("r4", &cpu->core[0].current.virtual_register.r4);
    Expression_Assign("r5", &cpu->core[0].current.virtual_register.r5);
    Expression_Assign("r6", &cpu->core[0].current.virtual_register.r6);
    Expression_Assign("r7", &cpu->core[0].current.virtual_register.r7);
    Expression_Assign("r8", &cpu->core[0].current.virtual_register.r8);
    Expression_Assign("r9", &cpu->core[0].current.virtual_register.r9);
    Expression_Assign("r10", &cpu->core[0].current.virtual_register.r10);
    Expression_Assign("r11", &cpu->core[0].current.virtual_register.r11);
    Expression_Assign("r12", &cpu->core[0].current.virtual_register.r12);
    Expression_Assign("r13", &cpu->core[0].current.virtual_register.r13);
    Expression_Assign("r14", &cpu->core[0].current.virtual_register.r14);
    Expression_Assign("r15", &cpu->core[0].current.virtual_register.r15);
    Expression_Assign("accumulator", &cpu->core[0].current.virtual_register.accumulator);
    Expression_Assign("carry", &cpu->core[0].current.virtual_register.carry);
    Expression_Assign("data_pointer", &cpu->core[0].current.virtual_register.data_pointer);
    Expression_Assign("pc", &cpu->core[0].current.virtual_register.pc);
    init_table_bank(&cpu->table_instructions, &cpu->table_types, &cpu->table_registers, &cpu->table_encoding, &cpu->instruction_bank);
    cpu_core_start(&cpu->core[0]);
}

void cpu_core_shutdown(struct virtual_cpu_core *core) {
    printf("VIRTUAL CPU: shutting down virtual cpu core: %s\n", core->name);
    core->power = CPU_POWER_OFF;
}

void cpu_shutdown(struct virtual_cpu *cpu) {
    puts("VIRTUAL CPU: performing shutdown check");
    virtual_cpu_check(cpu);
    puts("VIRTUAL CPU: shutdown check complete");
    printf("VIRTUAL CPU: shutting down virtual cpu:      %s\n", cpu->name);
    denit_table_bank(&cpu->table_instructions, &cpu->table_types, &cpu->table_registers, &cpu->table_encoding, &cpu->instruction_bank);
    opcode_structure_clear_all();
    internal_parser_index = 0;
    Expression_Assign_free();
    cpu_core_shutdown(&cpu->core[0]);
    cpu_deinit(cpu);
}

// A C program to demonstrate linked list based implementation of queue 
// A linked list (LL) node to store a queue entry
struct QNode
{
    char * assembly;
    struct QNode *next;
};
 
// The queue, front stores the front node of LL and rear stores ths
// last node of LL
struct Queue
{
    struct QNode *front, *rear;
};
 
// A utility function to create a new linked list node.
struct QNode* newNode(char * string)
{
    struct QNode *temp = (struct QNode*)malloc(sizeof(struct QNode));
    temp->assembly = string;
    temp->next = NULL;
    return temp; 
}
 
// A utility function to create an empty queue
struct Queue *createQueue()
{
    struct Queue *q = (struct Queue*)malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    return q;
}

void store_asm(struct Queue *q, char * string)
{
    // Create a new LL node
    struct QNode *temp = newNode(string);
 
    // If queue is empty, then new node is front and rear both
    if (q->rear == NULL)
    {
       q->front = q->rear = temp;
       return;
    }
 
    // Add the new node at the end of queue and change rear
    q->rear->next = temp;
    q->rear = temp;
}
 
struct QNode * load_asm(struct Queue *q)
{
    // If queue is empty, return NULL.
    if (q == NULL)
       return NULL;
    if (q->front == NULL)
       return NULL;
 
    // Store previous front and move front one node ahead
    struct QNode *temp = q->front;
    q->front = q->front->next;
 
    // If front becomes NULL, then change rear also as NULL
    if (q->front == NULL)
       q->rear = NULL;
    return temp;
}

void virtual_cpu_register_check(struct virtual_cpu_register_bank *reg, char * name) {
    null_int(reg->r0);
    null_int(reg->r1);
    null_int(reg->r2);
    null_int(reg->r3);
    null_int(reg->r4);
    null_int(reg->r5);
    null_int(reg->r6);
    null_int(reg->r7);
    null_int(reg->r8);
    null_int(reg->r9);
    null_int(reg->r10);
    null_int(reg->r11);
    null_int(reg->r12);
    null_int(reg->r13);
    null_int(reg->r14);
    null_int(reg->r15);
    null_int(reg->accumulator);
    null_int(reg->carry);
    null_int(reg->data_pointer);
    null_int(reg->pc);
    null_custom(reg->name, name);
    null_int(reg->type);
}

void virtual_cpu_sub_core_check(struct virtual_cpu_sub_core * core, char * name) {
    virtual_cpu_register_check(&core->virtual_register, "register");
    null_int(core->op);
    null_int(core->status);
    null_custom(core->name, name);
}

// a cpu part shall be considered uninitialized if has a value of zero [ 0 ]
void virtual_cpu_core_check(struct virtual_cpu_core *core, char * name) {
    if (core->init == true) return;
    virtual_cpu_sub_core_check(&core->previous, "PREVIOUS");
    virtual_cpu_sub_core_check(&core->current, "CURRENT");
    null_int(core->status);
    null_int(core->power);
    null_custom(core->cpu_power_state[0], "OFF");
    null_custom(core->cpu_power_state[1], "ON");
    null_custom(core->name, name);
    core->init = true;
}

void cpu_core_init(struct virtual_cpu *cpu, int max_cores) {
    if (cpu->core) return;
    cpu->core = malloc(max_cores*sizeof(struct virtual_cpu_core));
    for (int i = 0; i < max_cores; i++) {
        char * name = malloc(100);
        snprintf(name, 100, "Core %d", i);
        virtual_cpu_core_check(&cpu->core[i], name);
    }
    cpu->core_count = max_cores;
}

void cpu_deinit(struct virtual_cpu *cpu) {
    if (!cpu->core) return;
    for (int i = 0; i < cpu->core_count; i++) {
        free(cpu->core[i].name);
    }
    free(cpu->core);
    free(cpu->q);
    cpu->core_count = 0;
}

void virtual_cpu_check(struct virtual_cpu *cpu) {
    cpu_core_init(cpu, 32);
    null_custom(cpu->name, "Intel 4004");
}

int cpu_queue_add(char * string) {
    if (!cpu_default.q) cpu_default.q = createQueue();
    store_asm(cpu_default.q, string);
    return 0;
}

// cpu shall backup current to previous in order to have a restore point, then take changes from que and commit them to current
void cpu_execute(struct virtual_cpu *cpu) {
    puts("attempting to execute instruction list");
    // obtain the first added instruction
    struct QNode * node = malloc(1); // this gets freed anyway
    int instructions = 0;
    while (node != NULL) {
        // drain the list until empty
        free(node);
        node = load_asm(cpu->q);
        if (node == NULL) break;
        parse(cpu->table_encoding, node->assembly);
        cpu_decode(cpu->table_instructions, cpu->table_types, cpu->table_registers, cpu->instruction_bank, cpu->core[0].current.virtual_register.pc);
        cpu->core[0].current.virtual_register.pc++;
        instructions++;
    }
    if (instructions == 0) puts("i have no instruction to execute");
    else puts("i have no instructions left to execute");
}

int main()
{
    cpu_info(&cpu_default);
    cpu_start(&cpu_default);
//     cpu_queue_add("mov 19, %r1");
//     cpu_queue_add("mov 1, %r2");
//     cpu_queue_add("add %r3, %r1, %r2");
    cpu_execute(&cpu_default);
//     cpu_queue_add("mov 1, %r2");
    cpu_execute(&cpu_default);
    cpu_execute(&cpu_default);
//     cpu_info(&cpu_default);
    cpu_shutdown(&cpu_default);
    return 0;
}
