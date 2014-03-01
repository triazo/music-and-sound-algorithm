//
//  event.h
//  Markov Algorithm
//
//  Created by Mark P. Blanco on 3/1/14.
//  Copyright (c) 2014 Mark P. Blanco. All rights reserved.
//

#ifndef __Markov_Algorithm__event__
#define __Markov_Algorithm__event__

#include <iostream>
class Event{
public:
    Event();
    
    
private:
    
    
};

//The following classes inherit from event and each other


class ChannelEvent: public Event{//specify inheritance from Event class
public:
    ChannelEvent();//Same args as Event constructor
    
private:
    
    
};

class NoteOn: public ChannelEvent{//specify inheritance from Event class
public:
    NoteOn();

};

class NoteOff:  public ChannelEvent{//specify inheritance from Event class
public:
    NoteOff();
};

class MetaEvent: public Event{//specify inheritance from Event class
    
    
    
};

#endif /* defined(__Markov_Algorithm__event__) */
