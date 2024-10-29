#!/bin/sh
g++ -o bin/main src/main.cpp src/json.hpp -lsfml-system -lsfml-graphics -lsfml-window -lsfml-audio
cd bin/
./main
