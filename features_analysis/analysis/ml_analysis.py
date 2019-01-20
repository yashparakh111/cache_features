import sys      # provides access to command-line arguments
import os       # used to access all files under given directory
import time
#import numpy as np

for program in os.listdir(sys.argv[1]):
    start = time.time()
    f = open(os.path.join(sys.argv[1], program), "r")

    # find Hawkeye predictor accuracy
    total_pred = 0
    num_pred_correct = 0
    unique_PCs = set()

    #for i in range(5):
        #f.readline()

    for line in f:
        read_instr = line.split(" ")
        
        total_pred = total_pred + 1
        num_pred_correct = num_pred_correct + int(read_instr[-1])

        unique_PCs.add(int(read_instr[2], 16))


    print "Program Name: \t\t", program
    print "# Predicted Correctly: \t", num_pred_correct
    print "Total #: \t\t", total_pred
    print "Accuracy: \t\t", float(num_pred_correct)/total_pred
    print "Dimensionality: \t", len(unique_PCs)
    print "Unique PC Ratio: \t", float(len(unique_PCs))/total_pred*100, "%"

    print "-------------------------------------->", time.time() - start, "\n"

    f.close()
