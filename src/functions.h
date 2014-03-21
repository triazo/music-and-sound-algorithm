//
//  event.h
//  Markov Algorithm
//
//  Created by Mark P. Blanco on 3/1/14.
//  Copyright (c) 2014 Mark P. Blanco. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#if defined(OS_MACOSX)
#include <endian.h>
#else
#include "endian.h"
#endif

// bytes is referenced size of event
// deltatime_ticks is number of ticks before event is executed
// type is type of event
// channel is channel event should be played on
// p1 and p2 are note values, if it's a note

// Prototypes:
void makeTrack(int& bytes, std::ofstream& midifile);
void channelEvent(int deltatime_ticks, int type, int channel, int p1, int  p2, std::ofstream& midifile);
void addDeltime(int& bytes, char*& event, int deltatime);
void MetaEvent(std::ofstream& midifile);// Not yet implemented

// Definitions:
void makeTrack(int& bytes, std::ofstream& midifile){
    // Start the Track
    midifile.write("MTrk", 4);
    // Output a sample size (number of bytes)
    int trackLength = htobe32(bytes);
    midifile.write((char*)& trackLength, 4);
}


void channelEvent(int deltatime_ticks, int type, int channel, int p1, int  p2, std::ofstream& midifile){
    char* event = NULL;
    int bytes;
    addDeltime(bytes, event, deltatime_ticks);
    event[bytes-3] = ((char)(type + 8) << 4) | ((char)channel);
    event[bytes-2] = (char)p1;
    event[bytes-1] = (char)p2;
    midifile.write(event, bytes);
    delete event;
}

void addDeltime(int& bytes, char*& event, int deltatime){
    // Get the correct size in bytes of the event
    bytes = 0;
    if (deltatime == 0) {
        bytes = 1;
    }
    else {
        int delTemp = deltatime;
        while (delTemp != 0) {
            delTemp >>= 7;
            bytes++;
        }
    }
    bytes += 3;

    // Allocate the event and begin filling it with data.
    event = new char[bytes];
    int count = 0;
    // Go through the first iteration differently because of the different bitmask.
    event[bytes-4] = ((char)(deltatime & 127));
    count++;
    deltatime >>= 7;
    while (deltatime != 0) {
        event[bytes-4-count] = ((char)(deltatime & 127)) | 128;
        deltatime >>= 7;
        count++;
    }
}

void MetaEvent(std::ofstream& midifile){
    // Incomplete
    // Add end of track (00FF2F00) event.
    // Note: need to add end of track detection!!
    // Quick hack: type 8 and track 15 makes a meta event. <-- type 7 or 8??
    // The rest of the fields match in length.
    channelEvent(0, 7, 15, 47, 0, midifile);
}
