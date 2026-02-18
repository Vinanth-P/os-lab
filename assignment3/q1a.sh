#!/bin/bash
echo "Enter how many numbers:"
read n

echo "enter number1: "
read max

for((i=2;i<=n;i++))
do
echo "enter number $i:"
read num

if [ $num -gt $max ]; then
max=$num
fi
done

echo "largest number is $max"