#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define NUM_INSTR_DESTINATIONS 2
#define NUM_INSTR_SOURCES 4

const uint8_t call_stack_max_size = 30;

typedef struct trace_call_stack {
    unsigned long long int ip;      // instruction pointer (PC) value 
    unsigned long long int call_stack[call_stack_max_size];    // call stack
    uint8_t call_stack_size;
} trace_call_stack_format_t;

int main(int argc, char** argv) {
    FILE *infile;
    trace_call_stack_format_t input;

    infile = fopen(argv[1], "r");
    if(infile == NULL) {
        fprintf(stderr, "\nError opening file\n");
        exit(1);
    }

    // read file contents till end of file
    while(fread(&input, sizeof(trace_call_stack_format_t), 1, infile)) {
        printf("0x%-15llX", input.ip);

        for(int i = 0; i < input.call_stack_size; i++) {
            printf("%-20llX", input.call_stack[i]);
        }

        printf("\n");
    } 
    fclose(infile);
    
    return 0;
}
