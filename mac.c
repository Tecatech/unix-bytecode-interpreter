#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define STACK_SIZE 256
static int stack[STACK_SIZE];

/** Instructions */
typedef enum {
    HLT, // 0  -- hlt           :: halts program
    PSH, // 1  -- psh val       :: pushes <val> to the stack
    POP, // 2  -- pop rm        :: pops value off the stack
    ADD, // 3  -- add rm, rn    :: adds two values from the stack
    MUL, // 4  -- mul rm, rn    :: multiplies two values from the stack
    DIV, // 5  -- div rm, rn    :: divides two values from the stack
    SUB, // 6  -- sub rm, rn    :: subtracts two values from the stack
    SLT, // 7  -- slt rm, rn    :: pushes register with the smallest value to the stack
    MOV, // 8  -- mov rm, rn    :: moves value from source register to destination register
    SET, // 9  -- set rm, val   :: sets register to <val>
    LOG, // 10 -- log val       :: prints out <val>
    IF,  // 11 -- if  rm val ip :: branches to specified instruction if register value equals <val>
    IFN, // 12 -- ifn rm val ip :: branches to specified instruction if register value doesn't equal <val>
    GLD, // 13 -- gld rm        :: loads register to the stack
    GPT, // 14 -- gpt rm        :: pushes the top of the stack to register
    NOP  // 15 -- nop           :: nothing
} Instructions;

/** Registers */
typedef enum {
    A, B, C, D, E, F, G, H, // GENERAL PURPOSE
    EX, EXA,                // EXCESS
    IP,                     // INSTRUCTION POINTER
    SP,                     // STACK POINTER
    REGISTER_SIZE
} Registers;

static int registers[REGISTER_SIZE];

// instruction array
int *instructions;

// instruction counter
int instruction_count = 0;

// allocation space
int instruction_space = 4;

/** program running status */
static bool running = true;

/** program counter assignment status */
bool is_jmp = false;

/** special register access */
#define SP (registers[SP])
#define IP (registers[IP])

/** current instruction set fetching */
#define FETCH (instructions[IP])

/** stack output operation */
void print_stack() {
    for (int i = 0; i < SP; i++) {
        printf("0x%04d ", stack[i]);
        if ((i + 1) % 4 == 0) {
            printf("\n");
        }
    }
    if (SP != 0) {
        printf("\n");
    }
}

void print_registers() {
    printf("Register dump:\n");
    for (int i = 0; i < REGISTER_SIZE; i++) {
        printf("%04d ", registers[i]);
        if ((i + 1) % 4 == 0) {
            printf("\n");
        }
    }
}

int find_empty_register() {
    for (int i = 0; i < REGISTER_SIZE; i++) {
        if (i != registers[EX] && i != registers[EXA]) {
            return i;
        }
    }
    return EX;
}

void eval(int instr) {
    is_jmp = false;
    switch (instr) {
        case HLT: {
            running = false;
            printf("Flow execution finished\n");
            break;
        }
        case PSH: {
            SP = SP + 1;
            IP = IP + 1;
            stack[SP] = instructions[IP];
            break;
        }
        case POP: {
            SP = SP - 1;
            break;
        }
        case ADD: {
            registers[A] = stack[SP];
            SP = SP - 1;
            
            registers[B] = stack[SP];
            
            registers[C] = registers[B] + registers[A];
            
            stack[SP] = registers[C];
            printf("%d + %d = %d\n", registers[B], registers[A], registers[C]);
            break;
        }
        case MUL: {
            registers[A] = stack[SP];
            SP = SP - 1;
            
            registers[B] = stack[SP];
            
            registers[C] = registers[B] * registers[A];
            
            stack[SP] = registers[C];
            printf("%d * %d = %d\n", registers[B], registers[A], registers[C]);
            break;
        }
        case DIV: {
            registers[A] = stack[SP];
            SP = SP - 1;
            
            registers[B] = stack[SP];
            
            registers[C] = registers[B] / registers[A];
            
            stack[SP] = registers[C];
            printf("%d / %d = %d\n", registers[B], registers[A], registers[C]);
            break;
        }
        case SUB: {
            registers[A] = stack[SP];
            SP = SP - 1;
            
            registers[B] = stack[SP];
            
            registers[C] = registers[B] - registers[A];
            
            stack[SP] = registers[C];
            printf("%d - %d = %d\n", registers[B], registers[A], registers[C]);
            break;
        }
        case SLT: {
            SP = SP - 1;
            stack[SP] = stack[SP + 1] < stack[SP];
            break;
        }
        case MOV: {
            registers[instructions[IP + 2]] = registers[instructions[IP + 1]];
            IP = IP + 2;
            break;
        }
        case SET: {
            registers[instructions[IP + 1]] = instructions[IP + 2];
            IP = IP + 2;
            break;
        }
        case LOG: {
            printf("%d\n", registers[instructions[IP + 1]]);
            IP = IP + 1;
            break;
        }
        case IF: {
            if (registers[instructions[IP + 1]] == instructions[IP + 2]) {
                IP = instructions[IP + 3];
                is_jmp = true;
            }
            else {
                IP = IP + 3;
            }
            break;
        }
        case IFN: {
            if (registers[instructions[IP + 1]] != instructions[IP + 2]) {
                IP = instructions[IP + 3];
                is_jmp = true;
            }
            else {
                IP = IP + 3;
            }
            break;
        }
        case GLD: {
            SP = SP + 1;
            IP = IP + 1;
            stack[SP] = registers[instructions[IP]];
            break;
        }
        case GPT: {
            registers[instructions[IP + 1]] = stack[SP];
            IP = IP + 1;
            break;
        }
        case NOP: {
            printf("No operation\n");
            break;
        }
        default: {
            printf("Unknown instruction: %d\n", instr);
            break;
        }
    }
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        printf("Usage: %s <input_binary>\n", argv[0]);
        return -1;
    }
    
    char *filename = argv[1];
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error occurred when reading input file!\n");
        return -1;
    }
    
    // allocating space for instructions
    instructions = malloc(sizeof(*instructions) * instruction_space);
    
    // reading input file
    int num;
    int i = 0;
    while (fscanf(file, "%d", &num) > 0) {
        instructions[i] = num;
        printf("%d\n", instructions[i]);
        i++;
        if (i >= instruction_space) {
            instruction_space *= 2;
            instructions = realloc(instructions, sizeof(*instructions) * instruction_space);
        }
    }
    
    // setting instruction counter to number of processed instructions
    instruction_count = i;
    
    // closing input file
    fclose(file);
    
    // initializing stack pointer
    SP = -1;
    
    // looping through program
    while (running && IP < instruction_count) {
        eval(FETCH);
        if (!is_jmp){
            IP = IP + 1;
        }
    }
    
    // cleaning up program
    free(instructions);
    
    return 0;
}