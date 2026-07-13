#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

if [ $# != 1 ]; then
    echo "Usage: ${0} euclid|random|pertUM"
    exit 1
fi
if [ "$1" != "euclid" -a "$1" != "random" -a "$1" != "pertUM" ]; then
	  echo "Usage: ${0} euclid|random|pertUM"
    exit 1
fi

DATA_DIR="${PROJECT_ROOT}/data/${1}"

if [ ! -d "$DATA_DIR" ]; then
	mkdir -p "$DATA_DIR"
fi



if [ "$1" = "euclid" ]; then
	CODE="make_euclidean"
	RHO=""
elif [ "$1" = "random" ]; then
	CODE="make_random"
	RHO=""
elif [ "$1" = "pertUM" ]; then
	CODE="perturbedUM"
	RHO="0.25"
fi

for n in 5 10 15 20 25 30 35 40 100 150 200 250 300 350 400 450 500
do
	a=1
	while [ $a -le 20 ]
	do
		${PROJECT_ROOT}/experiments/instance_generation/${CODE} ${n} ${RHO} ${a} ${DATA_DIR}/${1}${n}_${a}.phylip
		a=$((a+1))
	done
done

