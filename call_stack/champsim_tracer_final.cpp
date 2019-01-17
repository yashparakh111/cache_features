
/*! @file
 *  This is an example of the PIN tool that demonstrates some basic PIN APIs 
 *  and could serve as the starting point for developing your first PIN tool
 */

#include "pin.H"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <map>

#define NUM_INSTR_DESTINATIONS 2
#define NUM_INSTR_SOURCES 4

// maintains the call stack for the current instruction
vector<ADDRINT> call_stack_address;
const uint8_t call_stack_max_size = 30;

typedef struct trace_instr_format {
	unsigned long long int ip;  // instruction pointer (program counter) value

	unsigned char is_branch;    // is this branch
	unsigned char branch_taken; // if so, is this taken

	unsigned char destination_registers[NUM_INSTR_DESTINATIONS]; // output registers
	unsigned char source_registers[NUM_INSTR_SOURCES];           // input registers

	unsigned long long int destination_memory[NUM_INSTR_DESTINATIONS]; // output memory
	unsigned long long int source_memory[NUM_INSTR_SOURCES];           // input memory
} trace_instr_format_t;

// maintains call stack trace for read instructions
typedef struct trace_call_stack {
	unsigned long long int ip;  // instruction pointer
	unsigned long long int call_stack[call_stack_max_size];    // call stack
	uint8_t call_stack_size;
	uint8_t loop_depth;
} trace_call_stack_format_t;


/* ================================================================== */
// Global variables 
/* ================================================================== */

UINT64 instrCount = 0;

FILE* output_file;
FILE* call_stack_output_file;

bool output_file_closed = false;
bool tracing_on = false;
bool is_read = false;

trace_instr_format_t curr_instr;
trace_call_stack_format_t curr_call_stack;

// allows for a generic mapping from function_name to any value defined under this struct
typedef struct feature_map {
	std::map<int, uint8_t> loop_depth_map;  // maps instruction offset to a loop depth
} feature_map_t;

typedef std::map<std::string, feature_map_t> function_map;
function_map func_map;

/* ===================================================================== */
// Command line switches
/* ===================================================================== */
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE,  "pintool", "o", "champsim.trace", 
		"specify file name for Champsim tracer output");

KNOB<string> KnobCallStackOutputFile(KNOB_MODE_WRITEONCE,  "pintool", "O", "champsim_call_stack.trace", 
		"specify file name for Champsim tracer call stack output");

KNOB<string> KnobLoopDepthTrace(KNOB_MODE_WRITEONCE,  "pintool", "l", "loop_depth.trace", 
		"specify path to loop_depth_info file");

KNOB<UINT64> KnobSkipInstructions(KNOB_MODE_WRITEONCE, "pintool", "s", "0", 
		"How many instructions to skip before tracing begins");

KNOB<UINT64> KnobTraceInstructions(KNOB_MODE_WRITEONCE, "pintool", "t", "1000000", 
		"How many instructions to trace");

/* ===================================================================== */
// Utilities
/* ===================================================================== */

/*!
 *  Print out help message.
 */
INT32 Usage()
{
	cerr << "This tool creates a register and memory access trace" << endl 
		<< "Specify the output trace file with -o" << endl 
		<< "Specify the call stack output trace file with -O" << endl
		<< "Specify the number of instructions to skip before tracing with -s" << endl
		<< "Specify the number of instructions to trace with -t" << endl << endl;

	cerr << KNOB_BASE::StringKnobSummary() << endl;

	return -1;
}

/* ===================================================================== */
// Analysis routines
/* ===================================================================== */

void BeginInstruction(VOID *ip, UINT32 op_code, VOID *opstring)
{
	instrCount++;
	//printf("[%p %u %s ", ip, op_code, (char*)opstring);

	if(instrCount > KnobSkipInstructions.Value()) 
	{
		tracing_on = true;

		if(instrCount > (KnobTraceInstructions.Value()+KnobSkipInstructions.Value()))
			tracing_on = false;
	}

	if(!tracing_on) 
		return;

	// reset the current instruction
	curr_instr.ip = (unsigned long long int)ip;
	/*if(curr_instr.ip != 0)
	  cout << instrCount << ", ";*/

	curr_instr.is_branch = 0;
	curr_instr.branch_taken = 0;

	for(int i=0; i<NUM_INSTR_DESTINATIONS; i++) 
	{
		curr_instr.destination_registers[i] = 0;
		curr_instr.destination_memory[i] = 0;
	}

	for(int i=0; i<NUM_INSTR_SOURCES; i++) 
	{
		curr_instr.source_registers[i] = 0;
		curr_instr.source_memory[i] = 0;
	}


	// initialize call stack info for read instr	
	curr_call_stack.ip = (unsigned long long int)ip;
	curr_call_stack.call_stack_size = 0;
	curr_call_stack.loop_depth = 0;

	for(int i = 0; i < call_stack_max_size; i++) {
		curr_call_stack.call_stack[i] = 0;
	}
}

void EndInstruction() {
	//printf("%d]\n", (int)instrCount);

	//printf("\n");

	if(instrCount > KnobSkipInstructions.Value())
	{
		tracing_on = true;

		if(instrCount <= (KnobTraceInstructions.Value()+KnobSkipInstructions.Value()))
		{
			// keep tracing
			fwrite(&curr_instr, sizeof(trace_instr_format_t), 1, output_file);
			if(is_read) {
				fwrite(&curr_call_stack, sizeof(trace_call_stack_format_t), 1, call_stack_output_file);
			}
		}
		else
		{
			tracing_on = false;
			// close down the file, we're done tracing
			if(!output_file_closed)
			{
				fclose(output_file);
				fclose(call_stack_output_file);
				output_file_closed = true;
			}

			exit(0);
		}
	}
}

static VOID BranchOrNot(BOOL taken)
{
	//printf("[%d] ", taken);

	curr_instr.is_branch = 1;
	if(taken != 0)
	{
		curr_instr.branch_taken = 1;
	}
}

void RegRead(UINT32 i, UINT32 index)
{
	if(!tracing_on) return;

	REG r = (REG)i;

	/*
	   if(r == 26)
	   {
	// 26 is the IP, which is read and written by branches
	return;
	}
	*/

	//cout << r << " " << REG_StringShort((REG)r) << " " ;
	//cout << REG_StringShort((REG)r) << " " ;

	//printf("%d ", (int)r);

	// check to see if this register is already in the list
	int already_found = 0;
	for(int i=0; i<NUM_INSTR_SOURCES; i++)
	{
		if(curr_instr.source_registers[i] == ((unsigned char)r))
		{
			already_found = 1;
			break;
		}
	}
	if(already_found == 0)
	{
		for(int i=0; i<NUM_INSTR_SOURCES; i++)
		{
			if(curr_instr.source_registers[i] == 0)
			{
				curr_instr.source_registers[i] = (unsigned char)r;
				break;
			}
		}
	}
}

void RegWrite(REG i, UINT32 index)
{
	if(!tracing_on) return;

	REG r = (REG)i;

	/*
	   if(r == 26)
	   {
	// 26 is the IP, which is read and written by branches
	return;
	}
	*/

	//cout << "<" << r << " " << REG_StringShort((REG)r) << "> ";
	//cout << "<" << REG_StringShort((REG)r) << "> ";

	//printf("<%d> ", (int)r);

	int already_found = 0;
	for(int i=0; i<NUM_INSTR_DESTINATIONS; i++)
	{
		if(curr_instr.destination_registers[i] == ((unsigned char)r))
		{
			already_found = 1;
			break;
		}
	}
	if(already_found == 0)
	{
		for(int i=0; i<NUM_INSTR_DESTINATIONS; i++)
		{
			if(curr_instr.destination_registers[i] == 0)
			{
				curr_instr.destination_registers[i] = (unsigned char)r;
				break;
			}
		}
	}
	/*
	   if(index==0)
	   {
	   curr_instr.destination_register = (unsigned long long int)r;
	   }
	   */
}

void MemoryRead(INS instr, VOID* addr, UINT32 index, UINT32 read_size) {
	is_read = true;

	if(!tracing_on) return;

	//printf("0x%llx,%u ", (unsigned long long int)addr, read_size);

	// check to see if this memory read location is already in the list
	int already_found = 0;
	for(int i=0; i<NUM_INSTR_SOURCES; i++)
	{
		if(curr_instr.source_memory[i] == ((unsigned long long int)addr))
		{
			already_found = 1;
			break;
		}
	}
	if(already_found == 0)
	{
		for(int i=0; i<NUM_INSTR_SOURCES; i++)
		{
			if(curr_instr.source_memory[i] == 0)
			{
				curr_instr.source_memory[i] = (unsigned long long int)addr;
				break;
			}
		}
	}

	// capture loop depth for routines with valid symbols
	PIN_LockClient();
	if(!call_stack_address.empty()) {
		ADDRINT rtn_addr = call_stack_address.back();
		RTN curr_rtn = RTN_FindByAddress(rtn_addr);
		if(RTN_Valid(curr_rtn) && SYM_Valid(RTN_Sym(curr_rtn))) {
			string function_name = PIN_UndecorateSymbolName(SYM_Name(RTN_Sym(curr_rtn)), UNDECORATION_NAME_ONLY);
			int offset = curr_call_stack.ip - (unsigned long long int)rtn_addr;

			// record loop depth
			curr_call_stack.loop_depth = (func_map[function_name]).loop_depth_map[offset];

			cout << hex << unsigned(func_map[function_name].loop_depth_map[offset])
			  << setw(15) << function_name << ": "
			  << curr_call_stack.ip << " - " << (unsigned long long int)rtn_addr << " = " << offset
			  << endl;
		}
	}
	PIN_UnlockClient();

	// record call stack for this read instruction
	curr_call_stack.call_stack_size = 
		call_stack_address.size() >= call_stack_max_size ? call_stack_max_size : call_stack_address.size();

	std::vector<ADDRINT>::reverse_iterator rtn_addr_it = call_stack_address.rbegin();
	for(int i = 0; i < curr_call_stack.call_stack_size; i++) {
		curr_call_stack.call_stack[i] = *rtn_addr_it;
		rtn_addr_it++;
	}
}

void MemoryWrite(VOID* addr, UINT32 index) {
	if(!tracing_on) return;

	//printf("(0x%llx) ", (unsigned long long int) addr);

	// check to see if this memory write location is already in the list
	int already_found = 0;
	for(int i=0; i<NUM_INSTR_DESTINATIONS; i++)
	{
		if(curr_instr.destination_memory[i] == ((unsigned long long int)addr))
		{
			already_found = 1;
			break;
		}
	}
	if(already_found == 0)
	{
		for(int i=0; i<NUM_INSTR_DESTINATIONS; i++)
		{
			if(curr_instr.destination_memory[i] == 0)
			{
				curr_instr.destination_memory[i] = (unsigned long long int)addr;
				break;
			}
		}
	}
	/*
	   if(index==0)
	   {
	   curr_instr.destination_memory = (long long int)addr;
	   }
	   */
}

/* ===================================================================== */
// Instrumentation callbacks
/* ===================================================================== */

// Is called for every instruction and instruments reads and writes
VOID Instruction(INS ins, VOID *v) {
	//cerr << INS_Disassemble(ins) << " " << INS_IsBranchOrCall(ins) << " " << INS_IsXend(ins) << endl;
	// begin each instruction with this function
	UINT32 opcode = INS_Opcode(ins);
	INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)BeginInstruction, IARG_INST_PTR, IARG_UINT32, opcode, IARG_END);

	// instrument branch instructions
	if (INS_IsBranch(ins) && !INS_IsXend(ins))
		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)BranchOrNot, IARG_BRANCH_TAKEN, IARG_END);

	// instrument register reads
	UINT32 readRegCount = INS_MaxNumRRegs(ins);
	for(UINT32 i=0; i<readRegCount; i++) 
	{
		UINT32 regNum = INS_RegR(ins, i);

		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)RegRead,
				IARG_UINT32, regNum, IARG_UINT32, i,
				IARG_END);
	}

	// instrument register writes
	UINT32 writeRegCount = INS_MaxNumWRegs(ins);
	for(UINT32 i=0; i<writeRegCount; i++) 
	{
		UINT32 regNum = INS_RegW(ins, i);

		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)RegWrite,
				IARG_UINT32, regNum, IARG_UINT32, i,
				IARG_END);
	}

	// instrument memory reads and writes
	UINT32 memOperands = INS_MemoryOperandCount(ins);

	// Iterate over each memory operand of the instruction.
	for (UINT32 memOp = 0; memOp < memOperands; memOp++) 
	{
		if (INS_MemoryOperandIsRead(ins, memOp)) 
		{
			UINT32 read_size = INS_MemoryReadSize(ins);

			INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)MemoryRead,
					IARG_MEMORYOP_EA, memOp, IARG_UINT32, memOp, IARG_UINT32, read_size,
					IARG_END);
		}
		if (INS_MemoryOperandIsWritten(ins, memOp)) 
		{
			INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)MemoryWrite,
					IARG_MEMORYOP_EA, memOp, IARG_UINT32, memOp,
					IARG_END);
		}
	}

	// finalize each instruction with this function
	INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)EndInstruction, IARG_END);
}

// push routine on call stack
VOID PushRoutine(ADDRINT rtn) {
	call_stack_address.push_back(rtn);
}

// pop routine from call stack
VOID PopRoutine() {
	call_stack_address.pop_back();
}

// Pin calls this function every time a new rtn is executed
VOID Routine(RTN rtn, VOID *v) {
	//cout << setw(50) << RTN_Name(rtn) << ": " << RTN_IsDynamic(rtn) << endl;
	if(!RTN_Name(rtn).compare(0, 2, "_Z") || !RTN_Name(rtn).compare("main")) {
		RTN_Open(rtn);

		// push routine on call stack
		RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)PushRoutine, IARG_ADDRINT, RTN_Address(rtn), IARG_END);

		// pop routine off call stack
		RTN_InsertCall(rtn, IPOINT_AFTER, (AFUNPTR)PopRoutine, IARG_END);

		RTN_Close(rtn);
	}
}

/*!
 * Print out analysis results.
 * This function is called when the application exits.
 * @param[in]   code            exit code of the application
 * @param[in]   v               value specified by the tool in the 
 *                              PIN_AddFiniFunction function call
 */
VOID Fini(INT32 code, VOID *v)
{
	//cout << instrCount << "\n";
	// close output files if it hasn't already been closed
	if(!output_file_closed) 
	{
		fclose(output_file);
		fclose(call_stack_output_file);
		output_file_closed = true;
	}

	// test to see if fileToMap is implemented properly
	/*
	   cout << endl << "Map" << endl;
	   for(map<string, map<int, uint8_t>>::const_iterator iter = loop_depth_map.begin(); iter != loop_depth_map.end(); ++iter) {
	   cout << iter->first << endl; // print method name
	   for(std::map<int, uint8_t>::const_iterator iter2 = (iter->second).begin(); iter2 != (iter->second).end(); ++iter2) {
	   cout << iter2->first << "|" << unsigned(iter2->second) << endl;       // write offset|loop_depth pairs
	   }
	   cout << "-" << endl;
	   }
	   */
}

void splitString(vector<string> &v_str, const string &str, const char ch) {
	string sub;
	string::size_type pos = 0;
	string::size_type old_pos = 0;
	bool flag=true;

	while(flag)
	{
		pos=str.find_first_of(ch,pos);
		if(pos == string::npos)
		{
			flag = false;
			pos = str.size();
		}
		sub = str.substr(old_pos,pos-old_pos);  // Disregard the '.'
		v_str.push_back(sub);
		old_pos = ++pos;
	}
}

// read loop depth map from loop_depth.trace
bool fileToMap(const string& filename) {
	ifstream ifile;
	ifile.open(filename.c_str());
	if(!ifile)
		return false;		// could not read the file.

	string line;
	string key;
	map<int, uint8_t> l_depth_map;		// maps offsets to loop depth
	bool is_func_name = true;	

	while(ifile>>line) {
		vector<string> v_str;
		
		// encountered function name
		if(is_func_name) {
			key = line; // obtain function name
			is_func_name = false;
			continue;
		}

		// end of nested loop_depth map
		if(!line.compare("-")) {
			feature_map_t f_map = {l_depth_map};
			func_map[key] = f_map;

			is_func_name = true;
			l_depth_map.clear();
			continue;
		}
		splitString(v_str, line, '|');
		l_depth_map[atoi(v_str[0].c_str())] = atoi(v_str[1].c_str());
	}

	return true;
}

bool InitLoopDepth() {
	string filename = KnobLoopDepthTrace.Value();
	return fileToMap(filename);
}

/*!
 * The main procedure of the tool.
 * This function is called when the application image is loaded but not yet started.
 * @param[in]   argc            total number of elements in the argv array
 * @param[in]   argv            array of command line arguments, 
 *                              including pin -t <toolname> -- ...
 */
int main(int argc, char *argv[])
{
	// Initialize symbol table code, needed for rtn instrumentation
	PIN_InitSymbols();

	// Initialize PIN library. Print help message if -h(elp) is specified
	// in the command line or the command line is invalid 
	if(PIN_Init(argc,argv) )
		return Usage();

	//const char* fileName = KnobOutputFile.Value().c_str();
	string fileName = KnobOutputFile.Value();
	string callStackFileName = KnobCallStackOutputFile.Value();

	// This is the most horrible hack I've ever been party to. Kill me now.
	// We actually just pipe the log through the external 'gzip' program...
	//string command = "gzip -c9 > ";
	//string command = "xz > ";
	string command = "cat > ";
	command.append(fileName);
	output_file = popen(command.c_str(), "w");
	if (!output_file) 
	{
		cout << "Couldn't open output trace file. Exiting." << endl;
		exit(1);
	}

	//command = "xz > ";
	command = "cat > ";
	command.append(callStackFileName);
	call_stack_output_file = popen(command.c_str(), "w");
	if (!call_stack_output_file) 
	{
		cout << "Couldn't open call stack output trace file. Exiting." << endl;
		exit(1);
	}

	// Initialize loop depth map
	InitLoopDepth();

	// Register Routine to be called to instrument rtn
	RTN_AddInstrumentFunction(Routine, 0);

	// Register function to be called to instrument instructions
	INS_AddInstrumentFunction(Instruction, 0);

	// Register function to be called when the application exits
	PIN_AddFiniFunction(Fini, 0);

	//cerr <<  "===============================================" << endl;
	//cerr <<  "This application is instrumented by the Champsim Trace Generator" << endl;
	//cerr <<  "Trace saved in " << KnobOutputFile.Value() << endl;
	//cerr <<  "===============================================" << endl;

	// Start the program, never returns
	PIN_StartProgram();

	return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
