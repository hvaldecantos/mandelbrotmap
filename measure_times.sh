#!/bin/bash

cores=$1
execution_times=$2
command=$3

for thread in $(eval echo {1..$1})
do
    echo "========== Time with $thread thread/s ($execution_times executions of command $command) =========="
    multitime -q -n $execution_times $command $thread
done

# Install dependency:
#     $ sudo apt install multitime
# Executes with 1, 2, 3, N cores, M times each, to return Mean, Std.Dev., Min, Median, Max:
#     $ ./speedup_efficiency_metrics.sh 4 10 ./command
