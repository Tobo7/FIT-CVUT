#!/usr/bin/env bash

echo $( dirname -- "$0"; )
# program; dataset directory;


mkdir -p "output/$3_$4/$1/$2"

for f in $2/*
do
  echo "Processing $1 $f $3 $4" # always double quote "$f" filename
    $( dirname -- "$0"; )/runInstance.sh $1 "$f" "output/$3_$4/$1/$f" $3 $4
 # do something on $f
done
