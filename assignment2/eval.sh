#!/bin/bash
echo "enter number of terms:"
read n

a=0
b=1

echo "fibonacci series:"
for ((i=0; i<n; i++))
do
    echo -n "$a "
    fib=$((a + b))
    a=$b
    b=$fib
done