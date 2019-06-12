'''
C struct for read instructions info

const uint8_t call_stack_max_size = 32;
typedef struct trace_call_stack {
    unsigned long long int id;
    unsigned long long int ip;  // instruction pointer
    unsigned long long int call_stack[call_stack_max_size];    // call stack
    uint8_t call_stack_size;
    uint8_t loop_depth;
} trace_call_stack_format_t;
'''

import sys, os
from ctypes import *
import lzma
from multiprocessing import Pool

fields = []
fields.append(('id', c_ulonglong))
fields.append(('ip', c_ulonglong))
for x in range(32):
    fields.append(('call_stack' + str(x), c_ulonglong)) 
fields.append(('call_stack_size', c_ubyte))
fields.append(('loop_depth', c_ubyte))

class TraceEntry(Structure):
    _fields_ = fields


f = lzma.open(sys.argv[1], "rb")
program = os.path.splitext(os.path.basename(sys.argv[1]))[0]
print(program)

# 473.astar_107000000000B.call_stack.xz: 321954952 entries (aka read instructions)
# 473.astar_11500000000B.call_stack.xz:  268597038 entries
# 473.astar_355500000000B.call_stack.xz: 286395200 entries
# 473.astar_282000000000B.call_stack.xz: 321461795 entries
def count_read_instructions(file_name):
    f = lzma.open(file_name, "rb")
    count = 0
    read_instr_entry = TraceEntry()
    while (f.readinto(read_instr_entry)):
        count = count + 1
        if count % 1000000 == 0:
            print('process id:', os.getpid(), ' | count: ', count)

    program = os.path.splitext(os.path.basename(file_name))[0]
    print(program, count)

pool = Pool()
pool.map(count_read_instructions, [sys.argv[1]])

'''
count = 0
read_instr_entry = TraceEntry()
while (f.readinto(read_instr_entry)):
    count = count + 1
    if count % 1000000 == 0:
        print(count)
    if(read_instr_entry.loop_depth != 0):
        print(read_instr_entry.loop_depth)
print(count)
'''

'''
print("ID: {}".format(read_instr_entry.id))
print("IP: {}".format(read_instr_entry.ip))
print("Call Stack: {}".format(read_instr_entry.call_stack0))
print(read_instr_entry.call_stack1)
print(read_instr_entry.call_stack2)
print(read_instr_entry.call_stack3)
print(read_instr_entry.call_stack4)
print(read_instr_entry.call_stack_size)
print(read_instr_entry.loop_depth)
print("-------------------------")
count = count + 1
'''
