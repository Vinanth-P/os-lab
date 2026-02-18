#!/bin/bash

# Get today's date in YYYY-MM-DD format
today=$(date +%Y-%m-%d)

# Loop through all .jpg files
for file in *.jpg
do
    # Check if any .jpg files exist
    [ -e "$file" ] || continue  #e for file exist (o for exits)

    # Rename file
    mv "$file" "$today-$file"
done

echo "Renaming completed."
