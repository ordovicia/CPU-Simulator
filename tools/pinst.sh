#!/bin/sh

set -e

root=$(git rev-parse --show-toplevel)
tools=$root/tools

if [ $# -ge 1 ]; then
    f=$1
else
    f="instruction.log"
fi

python3 $tools/pinst.py $f > pinst.log
gnuplot -persist $tools/pinst.gpl
