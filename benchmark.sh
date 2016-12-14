#!/bin/bash

PROD=0

function run(){
    prog=$1
    iters=$2
    t=$({ time $(echo $iters | $prog >/dev/null); } 2>&1 | grep real | sed -e 's/.*m\([0-9]\+\.[0-9]\+\)s/\1/')
    printf "%s" $t
}

# Warmup processor
#echo "Ramping up CPU scheduler for stress test"
#./fractal_seq > /dev/null

# GO!
echo "Starting benchmark"
printf "Fractal sequential: "
run ./fractal_seq
printf "\n"

THREAD_COUNT=1 make fractal_par > /dev/null
printf "Fractal parallell(1): "
run ./fractal_par
printf "\n"

THREAD_COUNT=8 make fractal_par > /dev/null
printf "Fractal parallell(8): "
run ./fractal_par
printf "\n"
