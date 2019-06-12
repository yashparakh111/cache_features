#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define NUM_INSTR_DESTINATIONS 2
#define NUM_INSTR_SOURCES 4

const uint8_t call_stack_max_size = 32;

typedef struct trace_features {
	unsigned long long int id;
    unsigned long long int ip;      // instruction pointer (PC) value 
    unsigned long long int call_stack[call_stack_max_size];    // call stack
    uint8_t call_stack_size;
	uint8_t loop_depth;
} trace_features_t;

int main(int argc, char** argv) {
    FILE *infile;
    trace_features_t input;

    infile = fopen(argv[1], "r");
    if(infile == NULL) {
        fprintf(stderr, "\nError opening file\n");
        exit(1);
    }

	int counter = 0;
    // read file contents till end of file
    while(fread(&input, sizeof(trace_features_t), 1, infile) && counter < 15) {
		// print instruction id
		printf("0x%-15llX", input.id);

       	// print intruction pointer 
		printf("0x%-15llX", input.ip);

		// print loop depth
		printf("%-5u", input.loop_depth);

		// print call stack
        for(int i = 0; i < input.call_stack_size; i++) {
            printf("%-20llX", input.call_stack[i]);
        }

        printf("\n");
		counter++;
    }
    fclose(infile);
    
    return 0;
}
