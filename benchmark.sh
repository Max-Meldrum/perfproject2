#!/bin/bash

PROD=0

function run(){
    prog=$1
    t=$(/usr/bin/time $1 2>&1 | tail -n 2 | awk '{ print $1 $2 $3 $4 }' | head -n 1)
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
