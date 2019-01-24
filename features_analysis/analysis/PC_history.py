# this program analyzes PC history

import sys
import copy
import os
from prettytable import PrettyTable

pc_history_len_list = [2, 4, 8, 16]

data = PrettyTable(['Property', 'Value'])
for program in os.listdir(sys.argv[1]):
    data.add_row(["Program Name", program])

    for pc_history_index in range(len(pc_history_len_list)):
        pc_history_len = pc_history_len_list[pc_history_index]

        history = [None] * pc_history_len
        history_list_ordered = set()
        history_list_unordered = set()
        i = 0
        count = 0

        f = open(os.path.join(sys.argv[1], program), "r")

        for line in f:
            history[i] = (line.split(" "))[1]
            history_list_ordered.add(tuple(history))

            history_temp = copy.copy(history)   # performs a shallow copy
            history_temp.sort()
            history_list_unordered.add(tuple(history_temp))

            i = (i + 1) % pc_history_len
            count = count + 1

        data.add_row(["Total PC History Count", count])
        data.add_row(["Unique PC History Ratio (ordered):" + str(pc_history_len), float(len(history_list_ordered))/count)])
        data.add_row(["Unique PC History Ratio (unordered):" + str(pc_history_len), float(len(history_list_unordered))/count])

    f.close()

    data.clear_rows()
    print
