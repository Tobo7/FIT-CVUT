#!/bin/bash

# ./parseData.sh set alpha T0 N Tn extraDir


SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )


PARAMS_NAME="$6/$2_$3_$4_$5"

rm -rf $SCRIPT_DIR/tmp/$PARAMS_NAME
mkdir -p $SCRIPT_DIR/tmp/$PARAMS_NAME

FILE_LIMIT=6

for d in $1/*
do
    if [ -d "$d" ]; then
        DIR1=$SCRIPT_DIR/tmp/$PARAMS_NAME/"$(basename $d).stat"
        
        echo "name steps sat clauseSat clauseUnsat clausesCount startTemp endTemp weight weightCorrect" > $DIR1

        
        A=0
        for f in $d/*
        do
        
#             if (( A >= FILE_LIMIT )); then
#                 break
#             fi
#             
#             A=$((A+1))
        
            echo "$f"
            
            
            
            for i in {1..10}
            do
                ./SA $2 $3 $4 $5 "$f" >> $DIR1
                
                cat "$d-opt.dat" | grep "$(basename $f .mwcnf | cut -c2-) " | head -1 | awk '{print $2}' >> $DIR1
                
            done            
        done
    fi

done





