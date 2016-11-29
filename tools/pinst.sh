#!/bin/sh

set -e

root=$(git rev-parse --show-toplevel)
tools=$root/tools

python3 $tools/pinst.py > pinst.log
gnuplot -persist $tools/pinst.gpl
