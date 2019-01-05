#include <iostream>
#include <fstream>
#include "pin.H"

ofstream outFile;

VOID ReadContent(ADDRINT *addr) {
    ADDRINT value;
    PIN_SafeCopy(&value, addr, sizeof(ADDRINT));
    outFile << "Address 0x" << hex << (unsigned long long) addr << dec << ":\t" << value << endl;
    return;
}

VOID Routine(INS ins, VOID *v) {
        //RTN_Open(rtn);

        //for(INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins)) {
            if(INS_IsMov(ins) && INS_OperandIsMemory(ins, 1) && INS_MemoryBaseReg(ins) == REG_RIP) {
                INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(ReadContent), IARG_MEMORYREAD_EA, IARG_END);
            }
        //}
        //RTN_Close(rtn);
    //}
}

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "global_dump.out", "specify output file name");

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v) {
    outFile.close();
}

INT32 Usage()
{
    cerr << "This tool dump heap memory information (global variable) ..." << endl;
    cerr << endl
        << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

int main(int argc, char *argv[]) {
    // Initialize pin
    if (PIN_Init(argc, argv))
        return Usage();
    outFile.open(KnobOutputFile.Value().c_str());

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Routine, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);

    // Start the program, never returns
    PIN_StartProgram();

    return 0;
} 
