#!/bin/bash

echo "enter a five digit number: "
read num 

y=$num
reversed=0

while [ $y != 0 ]
do
x=$((y%10))
reversed=$((reversed*10 + x))
y=$((y/10))
done

echo "reversed number is $reversed"