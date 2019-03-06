#!/bin/bash
touch numbers.txt
i=1
#exec 1>numbers.txt
while [ $i -le 150 ]
do                                                                                                                                                                                                                                                                                                                                                                              
od -A n -t d -N 1 /dev/random >> numbers.txt
i=$(( i + 1 ))
done