// readmidi.cpp
//
// A program to read in a midi file and print out its contents

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <list>
#include <map>
#if defined(OS_MACOSX)
#include <endian.h>
#else
#include "endian.h"
#endif


void usage(char** argv);
void readtrack(std::ifstream& midifile, int ticksperquarter, std::ostream& out);


std::string string_to_hex(char* s, size_t len) {
    static const char* const lut = "0123456789ABCDEF";
    //size_t len = input.length();

    std::string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i) {
        const unsigned char c = s[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}

int tempo = 500000;

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

    std::ofstream out(argv[2]);
    if (!out) {
        std::cerr << "Cannot open " << argv[1] << " for writing" << std::endl;
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
    // if (fileformat != 0) {
    //     std::cerr << "Multiple tracks not supported" << std::endl;
    //     usage(argv);
    //     exit(1);
    // }

    // Then comes the number of tracks
    midifile.read(shortint, 2);
    short int numtracks = be16toh(*(short int*)shortint);
    if (numtracks > 100) {
        std::cerr << "Multiple tracks not supported" << std::endl;
        std::cout << numtracks << std::endl;
        usage(argv);
        exit(1);
    }

    // The number of dtime ticks per quarter note
    midifile.read(shortint, 2);
    short int dtimeticksperqnote = be16toh(*(short int*)shortint);


    for (int i=0; i < numtracks; i++) {
        readtrack(midifile, dtimeticksperqnote, out);
    }
    
}

void usage (char* argv[]) {
    std::cout << "Usage: " << argv[0] << " midi_file " << "notes_file" << std::endl;
}

// Puts  the delta time in terms of 64
int normDeltime(int deltime, int t) {
    int time = deltime * (tempo / t);
    return(time / (500000 / 64));
}


void readtrack(std::ifstream& midifile, int ticksperquarter, std::ostream& out) {
    char notes[12][3] = {"C", "Cs", "D", "Ds", "E", "F", "Fs", "G", "Gs", "A", "As", "B"};
    
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
    // midifile.read(track, tracksize);

    // Start the event loop
    int trackread = 0;
    std::map<int, int> playing;
    int curtime = 0;
    int eventtype, channel;
    bool foundnote = false;
    while (trackread < tracksize) {
        
        // Loop for deltatime
        int deltime = 0;
        char c = 0;
        midifile.read(&c, 1);
        trackread++;
        deltime = deltime | (c & 127);
        // In the case of the first bit being zero, this will not enter the loop
        while (c < 0) {
            deltime <<= 7;
            deltime = deltime | (c & 127);
            midifile.read(&c, 1);
            trackread++;
        }

        // Keep a running count of the total time
        curtime += deltime;
       
        // Event type and channel number are both a single nyble
        midifile.read(&c, 1);
        trackread++;
        // Do a check for running statuses
        int prevevent = eventtype;
        int prevchannel = channel;
        eventtype = (c & 240) >> 4;
        channel = (c & 15);
        if (eventtype < 8) {
            midifile.putback(c);
            trackread--;
            eventtype = prevevent;
            channel = prevchannel;
        }

        // Test if meta event
        if (eventtype == 15 && channel == 15) {
            
            // Read in the metaevent type
            midifile.read(&c, 1);
            trackread++;
            int type = c;


            // Read in the metaevent size
            midifile.read(&c, 1);
            trackread++;
            int datasize = c;

            // Read in (and discard) the data
            char buf[datasize];
            midifile.read(buf, datasize);
            trackread += datasize;

            // Tempo
            if (type == 81) {
                char tempoint[4];
                for (int i = 0; i < 3; i++)
                    tempoint[2-i] = buf[i];
                tempoint[3] = 0;
                tempo = *(int*)tempoint;
                std::cerr << "Tempo set to " << string_to_hex(buf, datasize) << std::endl;
                std::cerr << "Tempo set to " << string_to_hex((char*)&tempo, 4) << std::endl;
            }
            
            // End of track event    
            if (type == 47) {
                if (foundnote)
                    out << "EOT -1 -1 -1" << std::endl;
            }
            else std::cerr << "Ignoring Meta Event: type="
                           << type
                           << ", size=" << datasize
                           << ", data=" << string_to_hex(buf,datasize) << std::endl;
            
            continue;
        }

        // These event types only have
        if (eventtype == 12 || eventtype == 13) {
            std::cerr << "Event type not supported: " << eventtype << std::endl;
            midifile.read(&c, 1);
            trackread++;
            continue;
        }

        
        // Note number and velocity are a whole byte
        midifile.read(&c, 1);
        trackread++;
        int notenumber = c;
        midifile.read(&c, 1);
        trackread++;
        int velocity = c;


        // Code for outputting and sorting the events
        
        
        // Note off event
        if (eventtype == 8 || (eventtype == 9 && velocity == 0)) {
            std::map<int, int>::iterator start = playing.find(notenumber);
            if (start == playing.end()) {
                int length = normDeltime(curtime - start->second, ticksperquarter);
                std::cerr << "Unknown note termination: "
                          << notes[notenumber%12] << notenumber/12 << length << " "
                          << notenumber << " "
                          << length << " "
                          << velocity
                          << std::endl;
                continue;
            }

            int length = normDeltime(curtime - start->second, ticksperquarter);
            out << notes[notenumber%12] << notenumber/12 << length << " "
                << notenumber << " "
                << length << " "
                << "100"
                << std::endl;
            foundnote = true;
            playing.erase(start);
        }
        else if (eventtype == 9) {
            // Insert into map
             std::cerr << "Note on event: " << notes[notenumber%12]<<notenumber/12 << std::endl;
            if (!playing.insert(std::make_pair(notenumber, curtime)).second) {
                std::cerr << "This note is already playing: "
                          << notes[notenumber%12] << notenumber/12 << "--" << " "
                          << notenumber << " "
                          << "--" << " "
                          << velocity
                          << std::endl;
            }
        }
        else
            std::cerr << "Event type not supported: " << eventtype<< std::endl;        
    }

}
