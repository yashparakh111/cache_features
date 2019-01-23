# this program prints PC history

import sys

f = open(sys.argv[1])

history = [None] * 5
history_list = set()
i = 0
count = 0

for line in f:
    history[i] = (line.split(" "))[1]
    history_list.add(tuple(history))

    history_temp = history
    history_temp.sort()

    i = (i + 1) % 5
    count = count + 1

print "Unique PC History Ratio (considering ordering): ", len(history_list), "/", count, "=", float(len(history_list)) / count

print "Unique PC History Ratio (NOT considering ordering): ", 
f.close()
