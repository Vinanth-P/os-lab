#!/bin/bash
echo "enter number of elements: "
read n

declare -a num

echo "enter number in sorted way:"
for((i=0;i<n;i++))
do
    read num[i]
done
    
low=0
high=$((n-1))
found=0

echo "enter the number you want to find: "
read key

while (( low <= high ))
do
mid=$(( (low+high)/2 ))

if (( num[mid] == key )); then
echo "number found at index $((mid+1))"
found=1
break
elif (( num[mid] < key )); then
high=$((mid-1))
else
low=$((mid+1))
fi
done

if ((found==0)); then
echo "number not found"
fi
