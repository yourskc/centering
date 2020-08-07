#!/bin/bash
if [[ "$OSTYPE" == "linux-gnueabihf" ]]; then
g++ -o main.out circle.cpp moildev_rpi.a `pkg-config --cflags opencv` `pkg-config --libs opencv` 
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
g++ -o main.out circle.cpp moildev.a `pkg-config --cflags opencv` `pkg-config --libs opencv` 
fi
