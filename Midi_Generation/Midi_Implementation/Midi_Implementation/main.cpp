//
//  main.cpp
//  Markov Algorithm
//
//

#include <iostream>
#include <list>
#include <vector>
#include "event.h"
#include <cstdlib>
#include <fstream>
#if defined(OS_MACOSX)
#include <endian.h>
#else
#include "endian.h"
#endif

void create_chain();

void create_melody();

// Hard-Coded Midi Generation
void usage(char* argv[]);

int main(int argc, char* argv[]) {
    
    // Open file in binary mode.  Currently hard-coded
    if (argc < 2) {
		usage(argv);
		exit(1);
	}
	
    // Make midifile out-stream to write to
	std::ofstream midifile(argv[1], std::ios::out | std::ios::binary);
	midifile.seekp (0);
	
	// The header for a midi file
	midifile.write("MThd",4);
	// Length of 6
	int length = htobe32(6);
    // Tricking the compiler to typecast the length (previous line) to char array
	midifile.write((char*)&length, 4);
    
    // Header set-up information processing
    std::string track_type_string;
    std::cout << "Please enter the kind of tracks you would like in the midi file (single, synchronous, or asynchronous):" << std::endl;
    std::cin >> track_type_string;
    bool valid = false;
    short tracktype;
    short trackcount;
    unsigned int pos_int;
    while (!valid){
        if (track_type_string == "single"){
            valid = true;
            tracktype = htobe16(0);
            trackcount = htobe16(1);
        }
        else if (track_type_string == "synchronous"){
            valid = true;
            tracktype = htobe16(1);
            std::cout << "Please enter a positive integer number of tracks for the midi file:" << std::endl;
            std::cin >> pos_int;
            trackcount = htobe16(pos_int);
            
        }
        else if (track_type_string == "asynchronous"){
            valid = true;
            tracktype = htobe16(2);
            std::cout << "Please enter a positive integer number of tracks for the midi file:" << std::endl;
            std::cin >> pos_int;
            trackcount = htobe16(pos_int);
        }
        else {
            std::cout << "The inputted track type was invalid! Please re-enter here (single, synchronous, or asynchronous):" << std::endl;
            std::cin >> track_type_string;
        }
    }
    // Tricking the compiler again...won't label further tricks
	midifile.write((char*)& tracktype, 2);
    
	// Number of tracks (if tracktype == 0, must be 1)
	midifile.write((char*)& trackcount, 2);
	
	// Number of ticks per quarter note (used by delta-time, tempo determined later?)
	short ticksperbeat = htobe16(64);
	midifile.write((char*)& ticksperbeat, 2);
    std::cout << "Ticks per quarter note has been set to a default value of 64" << std::endl;
    
    // Make the tracks, with their events:
	for (int num_tracks = 0; num_tracks <= pos_int; num_tracks++){
        int bytes;
        int deltatime;
        int type;
        int channel;
        int p1;
        int p2;
        makeTrack(bytes, midifile);
        // Generate and write the first event.
        std::cout << "Please enter deltatime to precede event, followed by type, channel, and two note values:" << std::endl;
        std::cin >> deltatime >> type >> channel >> p1 >> p2;
        channelEvent(deltatime, type, channel, p1, p2, midifile);
        
        // Generate and write the second event.
        std::cout << "Please enter deltatime to precede event, followed by type, channel, and two note values:" << std::endl;
        std::cin >> deltatime >> type >> channel >> p1 >> p2;
        channelEvent(deltatime, type, channel, p1, p2, midifile);
        
        // Add end of track (00FF2F00) event.
        // Note: need to add end of track detection!!
        // Quick hack: type 8 and track 15 makes a meta event. <-- type 7 or 8??
        // The rest of the fields match in length.
        channelEvent(0, 7, 15, 47, 0, midifile);
    }
    midifile.close();
}

void usage (char* argv[]) {
    std::cout << "Usage: " << argv[0] << " outfile" << std::endl;
}