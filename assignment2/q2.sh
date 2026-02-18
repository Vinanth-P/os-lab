#!/bin/bash

ADDRESSBOOK="addressbook.txt"

# Create Address Book
create_book() {
    if [ -f $ADDRESSBOOK ]; then
        echo "Address Book already exists."
    else
        touch $ADDRESSBOOK
        echo "Address Book created successfully."
    fi
}

# View Address Book
view_book() {
    if [ ! -s $ADDRESSBOOK ]; then # -s checks if file exist and size > 0
        echo "Address Book is empty."
    else
        echo "Address Book : "
        cat $ADDRESSBOOK
    fi
}

# Insert a Record
insert_record() {
    echo "Enter Name:"
    read name
    echo "Enter Phone:"
    read phone
    echo "Enter Email:"
    read email

    echo "$name | $phone | $email" >> $ADDRESSBOOK 
    # >> this is append
    # > overwrite file
    echo "Record added successfully."
}

# Delete a Record
delete_record() {
    echo "Enter Name to delete:"
    read name

    if grep -i "^$name |" $ADDRESSBOOK > /dev/null; # > /dev/null/ ->suppress output (only exit status matters) 
    then #-i ->case insensitive, grep->search,^->stat of line \,|->matches your record format
        grep -iv "^$name |" $ADDRESSBOOK > temp.txt #-v ->invert match,#Copies all lines except the matching one into temp.txt
        mv temp.txt $ADDRESSBOOK 
        echo "Record deleted successfully."
    else
        echo "Record not found."
    fi
}

# Modify a Record
modify_record() {
    echo "Enter Name to modify:"
    read name

    if grep -i "^$name |" $ADDRESSBOOK > /dev/null; then
        echo "Enter New Phone:"
        read phone
        echo "Enter New Email:"
        read email

        grep -iv "^$name |" $ADDRESSBOOK > temp.txt
        echo "$name | $phone | $email" >> temp.txt
        mv temp.txt $ADDRESSBOOK
        echo "Record modified successfully."
    else
        echo "Record not found."
    fi
}

# Menu
while true
do
    echo "----------------------------"
    echo " ADDRESS BOOK MENU"
    echo "----------------------------"
    echo "1. Create Address Book"
    echo "2. View Address Book"
    echo "3. Insert Record"
    echo "4. Delete Record"
    echo "5. Modify Record"
    echo "6. Exit"
    echo "----------------------------"
    echo "Enter your choice:"
    read choice

    case $choice in
        1) create_book ;;
        2) view_book ;;
        3) insert_record ;;
        4) delete_record ;;
        5) modify_record ;;
        6) echo "Exiting..."; exit ;;
        *) echo "Invalid choice. Try again." ;;
    esac
done
