#!/bin/bash
echo Number of parameters: $#
sum=0
for var in $@
do
sum=$(( $sum + $var ))
done
sum=$(( $sum / $# ))
echo Average: $sum