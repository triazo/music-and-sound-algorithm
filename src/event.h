//
//  event.h
//  Midi_Implementation
//
//  Created by Mark P. Blanco on 3/19/14.
//  Copyright (c) 2014 Mark P. Blanco. All rights reserved.
//

#ifndef Midi_Implementation_Event_h
#define Midi_Implementation_Event_h

//Will use vector of events to store midi and track data, it will all be "compiled" at the end of runtime
#include <string>

using namespace std;

class Event{
public:
    Event(int deltime, int channel_number, int type);

//private:
    int deltime; //Note that this will be of variable length when converted to hex
    int channel_number;
    int type;

};

class chanEvent: public Event{
public:
    chanEvent(int note_number, int note_velocity, int deltime, int channel_number, int type);

//private:
    int note_number;//Note number
    int note_velocity;//Volume

};

class metaEvent: public Event{
public:
    metaEvent(int deltime, int channel_number, int type /*And more args...*/);
//Write several constructors, the first of which can be the end of track file

private:
    int length;
};


class endofTrack: public Event{
public:
endofTrack(int deltime, int channel_number, int type, int p1, int p2)
    : Event(deltime, channel_number, type), p1(p1), p2(p2) {}
//private:
    int p1, p2;
};
#endif
