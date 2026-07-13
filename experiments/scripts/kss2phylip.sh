#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

if [ $# != 2 ]; then
    echo "Usage: ${0} 1|2 euclid|random|pertUM"
    exit 1
fi
	
if [ "$1" != "1" -a "$1" != "2" ]; then
	  echo "Usage: ${0} 1|2 euclid|random|pertUM"
    exit 1
fi

if [ "$2" != "euclid" -a "$2" != "random" -a "$2" != "pertUM" ]; then
	  echo "Usage: ${0} 1|2 euclid|random|pertUM"
    exit 1
fi

for n in 500
do
	for k in 1 2 3 4 5 8 10 12 20
	do
		a=1	
		while [ $a -le 20 ]
		do
			echo "doing: ${PROJECT_ROOT}/app/min_inc p${1} ${PROJECT_ROOT}/data/${2}/${2}${n}_${a}.phylip $k" 1>&2
			"$PROJECT_ROOT/app/min_inc" "p${1}" "$PROJECT_ROOT/data/${2}/${2}${n}_${a}.phylip" "$k"
			a=$((a+1))
		done
	done
done
