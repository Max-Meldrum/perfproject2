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

#
# Bench Fractal
#
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

#
# Qsort
#
printf "Qsort sequential: "
run ./qsort_seq
printf "\n"

THREAD_COUNT=1 make qsort_par > /dev/null
printf "Qsort parallell(1): "
run ./qsort_par
printf "\n"

THREAD_COUNT=8 make qsort_par > /dev/null
printf "Qsort parallell(8): "
run ./qsort_par
printf "\n"

THREAD_COUNT=128 make qsort_par > /dev/null
printf "Qsort parallell(128): "
run ./qsort_par
printf "\n"

#
# Bench Matmul
#
printf "Matmul sequential: "
run ./matmul_seq
printf "\n"

THREAD_COUNT=1 make matmul_par > /dev/null
printf "Matmul sequential block: "
run ./matmul_par
printf "\n"

THREAD_COUNT=8 make matmul_par > /dev/null
printf "Matmul parallell block(8): "
run ./matmul_par
printf "\n"

#
# Bench Matmul lab3
#
printf "Matmul lab3 (8): "
run ./matmul_lab3_par
printf "\n"
