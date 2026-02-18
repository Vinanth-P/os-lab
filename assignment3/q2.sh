#!/bin/bash

echo "enter file name: "
read file_name

if [ -e $file_name ]; then
echo "$file_name passwords are enabled"

if [ -w $file_name ];then
echo "you have permission to edit $file_name"

else 
echo "you dont have permission to edit $file_name"
fi 

else 
echo "$file_path file does not exist"
fi
