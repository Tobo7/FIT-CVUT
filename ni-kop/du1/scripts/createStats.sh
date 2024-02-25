#!/usr/bin/env bash

python scripts/stat_util.py "output/$1/gsat2/data/$2/" "output/$1/probsat/data/$2/" "results/$1_$2_results.csv"
