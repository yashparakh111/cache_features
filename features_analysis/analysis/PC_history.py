# this program prints PC history

import sys
import copy
import os


for program in os.listdir(sys.argv[1]):
    history = [None] * 5
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

        i = (i + 1) % 5
        count = count + 1

    
    print "Program Name: ", program
    print "Unique PC History Ratio (ordered): ", len(history_list_ordered), "/", count, "=", float(len(history_list_ordered)) / count
    print "Unique PC History Ratio (unordered): ", len(history_list_unordered), "/", count, "=", float(len(history_list_unordered)) / count
    print "--------------------------------------"
f.close()
