# this program analyzes PC history

import sys
import os
from collections import deque
from prettytable import PrettyTable
import time

pc_history_len_list = [2, 4, 8, 16]

history_list_ordered = set()
history_list_unordered = set()

data = PrettyTable()

for program in os.listdir(sys.argv[1]):
    start = time.time()
    data.add_column("Unique PC-History Ratio", ["Ordered", "Unordered"])

    for pc_history_index in range(len(pc_history_len_list)):
        pc_history_len = pc_history_len_list[pc_history_index]

        history = deque(maxlen=pc_history_len)
        history_list_ordered.clear()
        history_list_unordered.clear()
        count = 0

        f = open(os.path.join(sys.argv[1], program), "r")

        for line in f:
            history.append((line.split(" "))[1])
            history_list_ordered.add(tuple(history))

            history_temp = list(history)
            history_temp.sort()
            history_list_unordered.add(tuple(history_temp))

            count = count + 1

        data.add_column(str(pc_history_len), [float(len(history_list_ordered))/count, float(len(history_list_unordered))/count])

        f.close()

    print "Program Name:", program
    print "PC-History Count:", count
    print data
    print "Execute Time:", time.time() - start
    data.clear()
    
    print
