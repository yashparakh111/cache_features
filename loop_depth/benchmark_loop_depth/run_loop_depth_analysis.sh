#!/bin/bash

for file in ./benchmarks/*
do
	echo $file
	../loop_depth $file
done
