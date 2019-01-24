# this program analyzes PC history

import sys
import os
from collections import deque
from prettytable import PrettyTable
import time
from collections import defaultdict

pc_history_len_list = [2, 4, 8, 16]

history_list_ordered = set()
history_list_unordered = set()

data = PrettyTable()

for program in os.listdir(sys.argv[1]):
    start = time.time()

    unique_PCs = {}     # create an empty dict
    accurate_PCs = {}     

    f = open(os.path.join(sys.argv[1], program), "r")
    total_pred = 0
    num_pred_correct = 0

    for line in f:
        read_instr = line.split(" ")

        total_pred = total_pred + 1
        num_pred_correct = num_pred_correct + int(read_instr[-1])
        PC = int(read_instr[2], 32)

        if PC not in unique_PCs:
            unique_PCs[PC] = 1
            accurate_PCs[PC] = int(read_instr[-1])
        else:
            unique_PCs[PC] = unique_PCs[PC] + 1
            accurate_PCs[PC] = accurate_PCs[PC] + int(read_instr[-1])

    accuracy_PCs = {}
    for k, v in unique_PCs.items():
        
        accuracy = float(accurate_PCs[k])/float(v)
        proportion = float(v)/float(total_pred)

        if accuracy >= 0.95 or proportion < 0.01:
            del unique_PCs[k]
            del accurate_PCs[k]
        else:
            accuracy_PCs[k] = accuracy
    #    else: 
    #        print(str(hex(k)) + " " + str(v) + " " + str(proportion) + " " + str(accuracy))

    data.add_column("PC", unique_PCs.keys())
    data.add_column("Count", unique_PCs.values())
    data.add_column("Hawkeye", accuracy_PCs.values())
   # print data

    for pc_history_index in range(len(pc_history_len_list)):
        pc_history_len = pc_history_len_list[pc_history_index]

        history = deque(maxlen=pc_history_len)
        pcs_history_dimensionality = {}
        history_list_unordered = defaultdict(set)

        f = open(os.path.join(sys.argv[1], program), "r")
        for pc in unique_PCs:
            history_list_unordered[pc].clear()

        for line in f:
            read_instr = line.split(" ")
            curr_pc = int(read_instr[2], 32)
            if(curr_pc in unique_PCs):
                history_temp = list(history)
                history_temp.sort()
                history_list_unordered[curr_pc].add(tuple(history_temp))

            history.append(curr_pc)

        f.close()
        #print (str(pc) + " " + str(len(history_list_unordered)/float(count)))
    
        for pc in unique_PCs:
            pcs_history_dimensionality[pc] = len(history_list_unordered[pc])

        
        data.add_column(str(pc_history_len), pcs_history_dimensionality.values())


    print "Program Name:", program
    #print "PC-History Count:", count
    print data
    print "Execute Time:", time.time() - start
    data.clear()
    
    print
