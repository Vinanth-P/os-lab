#!/bin/bash

echo "enter a: "
read a

echo "enter b:"
read b

temp=$a
a=$b
b=$temp

echo "swapped number is $a and $b"