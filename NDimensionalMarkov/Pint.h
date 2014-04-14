//
//  Data.h
//  Homework 7
//
//  Created by Mark P. Blanco on 4/2/14.
//  Copyright (c) 2014 Mark Blanco. All rights reserved.
//

#ifndef __Homework_7__Data__
#define __Homework_7__Data__

#include <iostream>
#include <set>
#include <map>
#include <string>

using namespace std;

class Note;
// This is an all-public, simple class that contains a pointer to a map<string, Pint> object
// (in other words, a BuildingBlock) and an int for keeping track of the number of times
// a certain word progression has been encountered
class Pint{
public:
    Pint(): second(0), first(NULL) {}
    // Representation
    map<Note, Pint>* first;
    int second;
};


class Note{
public:
    Note(string name, int note, int duration, int velocity):  {}
    // Representation:
    int note, duration, velocity;
    string name;
};

#endif /* defined(__Homework_7__Data__) */