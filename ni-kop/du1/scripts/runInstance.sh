#!/bin/bash

rm -f $3

for VARIABLE in {1..1000}
do

if [[ $1 == gsat2  ]]
then
 ./gsat2 -r time -p 0.4 $2 -i $4 -T $5 > /dev/null 2>> $3
elif [[ $1 == probsat ]]
then
 ./probsat -r time $2 -i $4 -T $5 > /dev/null 2>> $3
else
echo 'Invalid algorithm name'
fi

done





# cat stat.dat | awk '$3 = $4' > stat.dat.tmp; mv stat.dat.tmp stat.dat


# ./$1 -r time uf20-91/uf20-01.cnf > /dev/null 2>> stat.dat
