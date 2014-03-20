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
