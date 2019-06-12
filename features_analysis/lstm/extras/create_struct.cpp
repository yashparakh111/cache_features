#include <iostream>
#include <fstream>
#include <string>

using namespace std;

FILE* features_output_file;
const int call_stack_max_size = 32;

// maintains call stack trace for read instructions
typedef struct trace_features {
	unsigned long long int id;	// unique id
    unsigned long long int ip;  // instruction pointer
    unsigned long long int call_stack[call_stack_max_size];    // call stack
    uint8_t call_stack_size;
    uint8_t loop_depth;
} trace_features_format_t;

int main(int argc, char** argv) {
	string fileName = "test.out";
	string command = "cat > ";
    command.append(fileName);
    features_output_file = popen(command.c_str(), "w");
    if (!features_output_file) {
        cout << "Couldn't open output trace file. Exiting." << endl;
        exit(1);
    }

	trace_features_format_t features;
	features.id = 999;
	features.ip = 1234;
	(features.call_stack)[0] = 1;
	(features.call_stack)[1] = 2;
	(features.call_stack)[2] = 3;
	features.call_stack_size = 10;
	features.loop_depth = 10;

	fwrite(&features, sizeof(trace_features_format_t), 1, features_output_file);
	fclose(features_output_file);
}
