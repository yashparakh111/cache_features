# Helpful links:
    # Counting Frequencies: https://stackoverflow.com/questions/2600191/how-can-i-count-the-occurrences-of-a-list-item

import sys      # provides access to command-line arguments
import os       # used to access all files under given directory
import time
import numpy as np
from collections import Counter
import matplotlib.pyplot as plt

i = 0

for program in os.listdir(sys.argv[1]):
    i = i + 1
    start = time.time()
    f = open(os.path.join(sys.argv[1], program), "r")

    # find Hawkeye predictor accuracy
    total_pred = 0
    num_pred_correct = 0
    unique_PCs = set()
    PC_list = []

    #for i in range(5):
        #f.readline()

    for line in f:
        read_instr = line.split(" ")
        
        total_pred = total_pred + 1
        num_pred_correct = num_pred_correct + int(read_instr[-1])
        PC = int(read_instr[2], 16)
        unique_PCs.add(PC)
        PC_list.append(PC % 10000000)

    PC_freq = np.bincount(PC_list)

    #print unique_PCs[0], unique_PCs[1000], unique_PCs[100000], unique_PCs[-1000], unique_PCs[-1]
    print "Program Name: \t\t", program
    print "# Predicted Correctly: \t", num_pred_correct
    print "Total #: \t\t", total_pred
    print "Accuracy: \t\t", float(num_pred_correct)/total_pred
    print "Dimensionality: \t", len(unique_PCs)
    print "Unique PC Ratio: \t", float(len(unique_PCs))/total_pred*100, "%"

    print "-------------------------------------->", time.time() - start, "\n"

    plt.subplot(3, 4, i)
    plt.title(program)
    plt.plot(PC_freq)
    f.close()

plt.show()
