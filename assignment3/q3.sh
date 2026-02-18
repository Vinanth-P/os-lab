#!/bin/bash

echo "enter number of words: "
read n

declare -a word

for((i=0;i<n;i++))
do 
echo "enter word $((i+1)):"
read word[i]
done

echo "word length:"
for ((i=0;i<n;i++))
do 
echo "${word[i]} : ${#word}"
done
