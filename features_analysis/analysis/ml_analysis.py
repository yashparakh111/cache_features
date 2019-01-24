# Helpful links:
    # Counting Frequencies: https://stackoverflow.com/questions/2600191/how-can-i-count-the-occurrences-of-a-list-item
    # Printing Data in tabular format: https://stackoverflow.com/questions/9535954/printing-lists-as-tabular-data

import sys      # provides access to command-line arguments
import os       # used to access all files under given directory
import time
import numpy as np
from collections import Counter
import matplotlib.pyplot as plt
from prettytable import PrettyTable

plot_num = 1

for program in os.listdir(sys.argv[1]):
    start = time.time()
    f = open(os.path.join(sys.argv[1], program), "r")

    # find Hawkeye predictor accuracy
    total_pred = 0
    num_pred_correct = 0
    unique_PCs = {}     # create an empty dict

    for line in f:
        read_instr = line.split(" ")

        total_pred = total_pred + 1
        num_pred_correct = num_pred_correct + int(read_instr[-1])
        PC = int(read_instr[1], 16)

        if PC not in unique_PCs:
            unique_PCs[PC] = 0
        else:
            unique_PCs[PC] = unique_PCs[PC] + 1

        #PC_list.append(PC)

    #PC_freq = np.bincount(PC_list)

    #print unique_PCs[0], unique_PCs[1000], unique_PCs[100000], unique_PCs[-1000], unique_PCs[-1]
    data = PrettyTable(['Property', 'Value'])
    data.add_row(["Program Name", program])
    data.add_row(["# Predicted Correctly", num_pred_correct])
    data.add_row(["Total PCs", total_pred])
    data.add_row(["Hawkeye Accuracy", str(num_pred_correct) + " (" + str(round(float(num_pred_correct)/total_pred*100, 2)) + "%)"])
    data.add_row(["Dimensionality", len(unique_PCs)])
    data.add_row(["Unique PC Ratio", str(round(float(len(unique_PCs))/total_pred*100, 3)) + "%"])
    print data
    
    print "Execute Time: ", time.time() - start, "ms\n"

    plt.subplot(3, 4, plot_num)
    plt.title(program)
    plt.plot(unique_PCs.values())
    plot_num = plot_num + 1
    f.close()

plt.show()
