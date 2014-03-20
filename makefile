all:
	mkdir -p bin
	g++ src/*.cpp -o bin/midichain
debug:	
	mkdir -p bin
	g++ src/*.cpp -o bin/midichain -g -Wall
