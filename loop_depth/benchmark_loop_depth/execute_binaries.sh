# executes all the benchmark binaries
#!/bin/bash

for file in ./benchmarks/*-nn
do
	echo $file
	timeout 15 ./$file
	printf "\n\n---------------------------------------\n\n"
done
