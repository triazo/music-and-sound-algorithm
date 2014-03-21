//
//  main.cpp
//  Markov Algorithm
//
//

#include <iostream>
#include <list>
#include <vector>
#include "functions.h"
#include <cstdlib>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include "event.h"
#include "mchain.h"
#if defined(OS_MACOSX)
#include <endian.h>
#else
#include "endian.h"
#endif

// Hard-Coded Midi Generation
void usage(char* argv[]);

void compile_track(const vector<chanEvent> & track, ofstream & midifile);

int main(int argc, char* argv[]) {
    // Set the seed rand() will use.
    srand(time(NULL));
    // Open file in binary mode.  Currently hard-coded
    if (argc < 3) {
        usage(argv);
        exit(1);
    }
    // Pass name of markov file to constructor, and parse
    MChain markov_chain(argv[2]);

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
    unsigned int pos_int = 0;
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
        vector<chanEvent> track;
        markov_chain.runChain(track, markov_chain.getMaxNotes());
        int length = 4 * track.size() + 4;
        makeTrack(length, midifile);
        compile_track(track, midifile);
    }
    midifile.close();
}

void compile_track(const vector<chanEvent> & track, ofstream & midifile){
    for (int i = 0; i < track.size(); i++){
        channelEvent(track[i].deltime, track[i].type, track[i].channel_number, track[i].note_number, track[i].note_velocity, midifile);
    }
    MetaEvent(midifile);
}

void usage (char* argv[]) {
    std::cout << "Usage: " << argv[0] << " out_file " << "markov_file" << std::endl;
}
