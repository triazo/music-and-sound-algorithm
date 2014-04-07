// readmidi.cpp
//
// A program to read in a midi file and print out its contents

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <list>
#if defined(OS_MACOSX)
#include <endian.h>
#else
#include "endian.h"
#endif

#include "event.h"

void usage(char** argv);
void readtrack(std::ifstream& midifile, std::list<Event*>& events);

int main(int argc, char** argv) {
    // open file in binary mode for reading
    if (argc < 3) {
        usage(argv);
        exit(1);
    }

    // open the markov chain file for writing
    // std::ofstream mchain(argv[2]);
    
    // Make midifile out-stream to write to
    std::ifstream midifile(argv[1], std::ios::in | std::ios::binary);
    if (!midifile) {
        std::cerr << "Cannot open " << argv[1] << std::endl;
        usage(argv);
    }
        

    // Begin the reading in of the file
    char head[4];
    midifile.read(head, 4);

    // Error checking.  May be useful down the road.
    if (*(int*)head != *(int*)"MThd") {
        std::cerr << "Inproper midi header (first 4 bytes). Got: " 
                  <<  std::hex << *(int*)head << " Expected: " << std::hex << *(int*)"MThd" << std::endl;
        usage(argv);
        exit(1);
    }

    // Read the header size
    midifile.read(head, 4);
    int headersize = be32toh(*(int*)head);

    if (headersize != 6) {
        std::cerr << "Inproper midi header (Invalid header size)" << std::endl;
        usage(argv);
        exit(1);
    }


    // The track type comes next in the midi header
    char shortint[2];
    midifile.read(shortint, 2);

    short int fileformat = be16toh(*(short int*)shortint);
    if (fileformat != 0) {
        std::cerr << "Multiple tracks not supported" << std::endl;
        usage(argv);
        exit(1);
    }

    // Then comes the number of tracks
    midifile.read(shortint, 2);
    short int numtracks = be16toh(*(short int*)shortint);
    if (numtracks > 1) {
        std::cerr << "Multiple tracks not supported" << std::endl;
        std::cout << numtracks << std::endl;
        usage(argv);
        exit(1);
    }

    // The number of dtime ticks per quarter note
    midifile.read(shortint, 2);
    short int dtimeticksperqnote = be16toh(*(short int*)shortint);

    std::list<Event*> events;
    readtrack(midifile, events);
    
    
}

void usage (char* argv[]) {
    std::cout << "Usage: " << argv[0] << " out_file " << "markov_file" << std::endl;
}


void readtrack(std::ifstream& midifile, std::list<Event*>& events) {
    // Read a track header
    char head[4];
    midifile.read(head, 4);

    if (*(int*)head != *(int*)"MTrk") {
        std::cerr << "Improper track header" << std::endl;
        exit(1);
    }

    // Read the header size
    midifile.read(head, 4);

    int tracksize = be32toh(*(int*)head);
    // char track[tracksize];
    // midifile.read(track, tracksize);n

    // Start the event loop
    int trackread = 0;
    while (trackread < tracksize) {
        
        // Loop for deltatime
        int deltime = 0;
        char c = 0;
        midifile.read(&c, 1);
        trackread++;
        // In the case of the first bit being zero, this will not enter the loop
        while (c > 127) {
            deltime <<= 7;
            deltime = deltime | (c & 127);
            midifile.read(&c, 1);
            trackread++;
        }
        
        
        // Event type and channel number are both a single nyble
        midifile.read(&c, 1);
        trackread++;
        int eventtype = (c & 240) >> 4;
        int channel = (c & 15);
        
        // Note number and velocity are a whole byte
        midifile.read(&c, 1);
        trackread++;
        int notenumber = c;
        midifile.read(&c, 1);
        trackread++;
        int velocity = c;

        Event* event = new chanEvent(notenumber, velocity, deltime, channel, eventtype - 8);
        events.push_back(event);
        
    }
    
}
