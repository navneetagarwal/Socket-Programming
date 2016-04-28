#!/bin/bash

python random_length.py 8
g++ crypt_ran.cpp -lcrypt
./a.out

while IFS='' read -r line || [[ -n "$line" ]]; do
    echo "$line"
    ./client 10.105.12.50 5001 $line 8 001 >> text.txt
done < "$1"

python Avg.py

rm text.txt
rm Output.txt
rm Crypt.txt