#!/bin/bash

PI=3.14159

while true
do
    echo "\n"
    echo " GEOMETRY CALCULATOR MENU"
    echo "\n"
    echo "1. Area of a Circle"
    echo "2. Circumference of a Circle"
    echo "3. Area of a Rectangle"
    echo "4. Area of a Square"
    echo "5. Exit"
    echo "----------------------------"
    echo "Enter your choice:"
    read choice

    case $choice in
        1)
            echo "Enter radius:"
            read r
            area=$(echo "$PI * $r * $r" | bc)
            echo "Area of Circle = $area"
            ;;
        2)
            echo "Enter radius:"
            read r
            circum=$(echo "2 * $PI * $r" | bc)
            echo "Circumference of Circle = $circum"
            ;;
        3)
            echo "Enter length:"
            read l
            echo "Enter breadth:"
            read b
            area=$(echo "$l * $b" | bc)
            echo "Area of Rectangle = $area"
            ;;
        4)
            echo "Enter side:"
            read s
            area=$(echo "$s * $s" | bc)
            echo "Area of Square = $area"
            ;;
        5)
            echo "Exiting..."
            exit
            ;;
        *)
            echo "Invalid choice. Try again."
            ;;
    esac
done
