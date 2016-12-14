#!/bin/bash

PROD=0

function run(){
    prog=$1
    iters=$2
    t=$({ time $(echo $iters | $prog >/dev/null); } 2>&1 | grep real | sed -e 's/.*m\([0-9]\+\.[0-9]\+\)s/\1/')
    echo -e "$prog\t$t"
}

# Warmup processor
#echo "Ramping up CPU scheduler for stress test"
#./fractal_seq > /dev/null

# GO!
echo "Starting benchmark"
run ./fractal_seq
run ./fractal_par
