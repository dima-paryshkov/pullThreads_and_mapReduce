#!/bin/bash

result=10000

for (( i=0; i<50; i++))
do
a=`./e4 $1 $2`
if [ "$a" -lt "$result" ]
then
	result=$a
fi
done

echo $result
