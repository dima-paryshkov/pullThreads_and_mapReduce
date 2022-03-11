#!/bin/bash

result=0

for (( i=0; i<50; i++))
do
a=`./e4 $1 $2`
result=`expr $a + $result`
done

result=`expr $result / 50`
echo $result
