//
//  event.cpp
//  Midi_Implementation
//
//  Created by Mark P. Blanco on 3/19/14.
//  Copyright (c) 2014 Mark P. Blanco. All rights reserved.
//

#include "event.h"

Event::Event(int deltime, int channel_number, int type)
    : deltime(deltime), channel_number(channel_number), type(type) {}

chanEvent::chanEvent(int note_number, int note_velocity, int deltime, int channel_number, int type)
    : Event(deltime, channel_number, type), note_number(note_number), note_velocity(note_velocity) {}

metaEvent::metaEvent(int deltime, int channel_number, int type)
    : Event(deltime, channel_number, type) {}

char* chanEvent::getMidi() const {
    std::cout << "deltime: "        << std::setw(2) << deltime
              << " note_number: "   << note_number
              << " note_velocity: " << note_velocity;

    int bytes = 0;
    int deltatime = deltime;
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

    //std::cout << " Bytes: " << bytes;

    // Allocate the event and begin filling it with data.
    char event[bytes];
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

    event[bytes-3] = ((char)(type + 8) << 4) | ((char)channel_number);
    event[bytes-2] = (char)note_number;
    event[bytes-1] = (char)note_velocity;
    std::cout << " Hex: " << hex << (long int)event << std::endl;
    return event;
}
int chanEvent::getBytes() const {
    int bytes = 0;
    if (deltime == 0) {
        bytes = 1;
    }
    else {
        int delTemp = deltime;
        while (delTemp != 0) {
            delTemp >>= 7;
            bytes++;
        }
    }
    bytes += 3;
    return bytes;
}
