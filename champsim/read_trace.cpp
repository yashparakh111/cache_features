#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

#define NUM_INSTR_DESTINATIONS 2
#define NUM_INSTR_SOURCES 4

typedef struct trace_instr_format {
    unsigned long long int ip;      // instruction pointer (PC) value
    
    unsigned char is_branch;        // is this branch
    unsigned char branch_taken;     // if so, is this taken

    unsigned char destination_registers[NUM_INSTR_DESTINATIONS];    // output registers
    unsigned char source_registers[NUM_INSTR_SOURCES];               // input registers

    unsigned long long int destination_memory[NUM_INSTR_DESTINATIONS];  // output memory
    unsigned long long int source_memory[NUM_INSTR_SOURCES];            // input memory
} trace_inst_format_t;


int main(int argc, char** argv) {
    FILE *infile;
    trace_inst_format_t input;

    infile = fopen(argv[1], "r");
    if(infile == NULL) {
        fprintf(stderr, "\nError opening file\n");
        exit(1);
    }

    // read file contents till end of file
    while(fread(&input, sizeof(trace_inst_format_t), 1, infile)) {
        printf("0x%-25llX", input.ip);
        /*for(int i = 0; i < NUM_INSTR_DESTINATIONS; i++) {
            printf("%-25llu", (input.destination_memory)[i]);
        }*/
        for(int i = 0; i < NUM_INSTR_SOURCES; i++) {
            printf("%-25llu", (input.source_memory)[i]);
        }
        printf("\n");
    } 
    fclose(infile);
    
    return 0;
}
