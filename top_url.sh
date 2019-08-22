#!/bin/bash

ulimit -n 4102

if [ ! -x "top_url" ]; then 
echo "Error, no executable file."
exit 1
fi 

if [ ! -d "./temp" ]; then
    mkdir temp
fi

./top_url $* &> top_url.log

rm -rf temp