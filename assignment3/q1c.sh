#!/bin/bash

echo "enter a number : "
read num 

if ((num % 2 == 0)); then
echo "it is even"
else
echo "it is odd"
fi