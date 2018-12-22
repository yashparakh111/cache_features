//
// This tool counts the number of times a routine is executed and 
// the number of instructions executed in a routine
//

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string.h>
#include "pin.H"
#include <vector>
#include <unordered_map>

ofstream outFile;

// maintains the call stack for the current instruction
vector<string> call_stack;
uint8_t call_stack_size;

// maintains instruction instruction disassembly
static std::unordered_map<ADDRINT, std::string> inst_disassembly;

VOID RecordMemRead(ADDRINT *inst_ptr, ADDRINT *addr) {
    // record instruction disassembly
    outFile << setw(40) << inst_disassembly[(unsigned long long)inst_ptr] << " : ";

    // capture value read by read_instruction
    ADDRINT value;
    PIN_SafeCopy(&value, addr, sizeof(ADDRINT));
    outFile << "0x" << hex << (unsigned long long) inst_ptr << " reads 0x" << (unsigned long long) addr << " as " << (int)value << endl;

    outFile << setw(40) << " " << " : ";
    // print call stack beginning from most recent routine call
    int i = 0;
    for(std::vector<string>::reverse_iterator rtn_it = call_stack.rbegin(); rtn_it != call_stack.rend() && i < call_stack_size; rtn_it++) {
        outFile << *rtn_it << " ";
        i++;
    }

    outFile << endl << endl;
}

// push routine on call stack
VOID PushRoutine(ADDRINT rtn) {
    call_stack.push_back(RTN_FindNameByAddress(rtn));
}

// pop routine from call stack
VOID PopRoutine() {
    call_stack.pop_back();
}

// Pin calls this function every time a new rtn is executed
VOID Routine(RTN rtn, VOID *v) {
    // only consider c functions (c functions begin with "_Z")
    if (!RTN_Name(rtn).compare(0, 2, "_Z") || !RTN_Name(rtn).compare("main")) {
        RTN_Open(rtn);

        // push routine at start of the function
        RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)PushRoutine, IARG_ADDRINT, RTN_Address(rtn), IARG_END);

        for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins)) {
            // memory read encountered, record call stack
            if(INS_IsMemoryRead(ins)) {
                //outFile << OPCODE_StringShort(INS_Opcode(ins)) << "\t";
                //outFile << INS_Disassemble(ins) << endl;
                inst_disassembly[INS_Address(ins)] = INS_Disassemble(ins);

                INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead, IARG_INST_PTR, IARG_MEMORYREAD_EA, IARG_END);
            }
            /*UINT32 memOperands = INS_MemoryOperandCount(ins);

            for(UINT32 memOp = 0; memOp < memOperands; memOp++) {
                if(INS_MemoryOperandIsRead(ins, memOp))
                    INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead, IARG_INST_PTR, IARG_MEMORYREAD_EA, IARG_END);
            }*/
        }

        // pop routine at the end of function call
        RTN_InsertCall(rtn, IPOINT_AFTER, (AFUNPTR)PopRoutine, IARG_END);

        RTN_Close(rtn);
    }
}

// This function is called when the application exits.
VOID Fini(INT32 code, VOID *v) { 
    outFile.close();
}

INT32 Usage()
{
    cerr << "This Pintool records the call stack every time a read instruction is encountered" << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

int main(int argc, char * argv[])
{
    // declare the size of the call stack
    call_stack_size = 30;

    // Initialize symbol table code, needed for rtn instrumentation
    PIN_InitSymbols();

    outFile.open("callStack.out");

    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();

    // Register Routine to be called to instrument rtn
    RTN_AddInstrumentFunction(Routine, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);

    // Start the program, never returns
    PIN_StartProgram();

    return 0;
}
