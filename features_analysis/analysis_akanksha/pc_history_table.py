# this program analyzes PC history

import sys
import os
from collections import deque
from prettytable import PrettyTable
import time
from collections import defaultdict
import random

pc_history_len_list = [1, 2, 4, 8, 16]

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
    count_sum = 0
    proportion_sum = 0.0
    hawkeye_average = 0.0
    for k, v in unique_PCs.items():
        
        accuracy = float(accurate_PCs[k])/float(v)
        proportion = float(v)/float(total_pred)

        if accuracy >= 0.95 or proportion < 0.01:
            del unique_PCs[k]
            del accurate_PCs[k]
        else:
            accuracy_PCs[k] = accuracy
            count_sum = count_sum + v
            proportion_sum = proportion_sum + proportion
            hawkeye_average = hawkeye_average + accurate_PCs[k]

    hawkeye_average = hawkeye_average/float(count_sum)

    data.add_column("PC", unique_PCs.keys())
    data.add_column("Count", unique_PCs.values())
    data.add_column("Hawkeye", accuracy_PCs.values())
    average_row = ["Average", proportion_sum, hawkeye_average]
   # print data

    for pc_history_index in range(len(pc_history_len_list)):
        pc_history_len = pc_history_len_list[pc_history_index]

        history = deque(maxlen=pc_history_len)
        history_cache_friendly = {}
        history_count = {}
        per_pc_accurate = {}
        pcs_history_accuracy = {}
        pc_cache_friendly = {}
        pc_count = {}

        f = open(os.path.join(sys.argv[1], program), "r")
        average = 0.0

        for pc in unique_PCs:
            history_cache_friendly[pc] = {}
            history_count[pc] = {}
            per_pc_accurate[pc] = 0 
            pcs_history_accuracy[pc] = 0

        for line in f:
            read_instr = line.split(" ")
            curr_pc = int(read_instr[2], 32)
            opt_decision = int(read_instr[-2])

            if(curr_pc in unique_PCs):
                history_temp = list(history)
                history_temp.sort()

                #generate prediction
                prediction = random.randint(0,1)
                if curr_pc in pc_count:
                    probability = float(pc_cache_friendly[curr_pc])/float(pc_count[curr_pc])
                    if probability < 0.5:
                        prediction = 0
                    else:
                        prediction = 1

                if(tuple(history_temp) in history_count[curr_pc]):
                    probability = float(history_cache_friendly[curr_pc][tuple(history_temp)])/float(history_count[curr_pc][tuple(history_temp)])
                    if probability < 0.5:
                        prediction = 0
                    else:
                        prediction = 1
                
                #check prediction
                if(prediction == opt_decision):
                    per_pc_accurate[curr_pc] += 1               
                
                #update table 
                if curr_pc in pc_count:
                    pc_count[curr_pc] += 1
                    pc_cache_friendly[curr_pc] += opt_decision;
                else:
                    pc_count[curr_pc] = 1
                    pc_cache_friendly[curr_pc] = opt_decision;

                if(tuple(history_temp) in history_count[curr_pc]):
                    history_count[curr_pc][tuple(history_temp)] += 1
                    history_cache_friendly[curr_pc][tuple(history_temp)] += opt_decision
                else:
                    history_count[curr_pc][tuple(history_temp)] = 1
                    history_cache_friendly[curr_pc][tuple(history_temp)] = opt_decision

            history.append(curr_pc)

        f.close()
    
        for pc in unique_PCs:
            pcs_history_accuracy[pc] = float(per_pc_accurate[pc])/float(unique_PCs[pc])
            average = average + per_pc_accurate[pc]

        average = average/float(count_sum) 
        data.add_column(str(pc_history_len), pcs_history_accuracy.values())
        average_row.append(average)

    data.add_row(average_row)

    print "Program Name:", program
    #print "PC-History Count:", count
    print data
    print "Execute Time:", time.time() - start
    data.clear()
    
    print
