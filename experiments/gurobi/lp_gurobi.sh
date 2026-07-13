#!/bin/bash

if [ $# -ne 0 ]; then
	echo "$0 "
else
	
#	for n in 20
#	do
#		a=12
#		while [ $a -le 15 ]
#		do
#			python3 lp_gurobi3_phylip.py ../../../data/euclid/euclid${n}_${a}.phylip
#			a=$((a+1))
#		done
#	done

	for n in 20
	do
		a=11
		while [ $a -le 15 ]
		do
			python3 lp_gurobi3_phylip.py ../../../data/euclid/euclid${n}_${a}.phylip
			a=$((a+1))
		done
	done
fi

#if [ $# -ne 0 ]; then
#	echo "$0 "
#else
#	
#	for n in 15 20 25
#	do
#		a=1
#		while [ $a -le 15 ]
#		do
#			python3 lp_gurobi_phylip.py ../../../data/pertUM/pertUM${n}_${a}.phylip
#			a=$((a+1))
#		done
#	done
#fi
