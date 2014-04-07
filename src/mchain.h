// mchain.h
// -*-c++-mode-*-
// Header file for the Markov chain class

#include <cstdlib>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

#include "event.h"

class Vertex {
public:
Vertex(const std::string & nme, int nt, int dr, int vt):
    name(nme),note(nt),duration(dr),velocity(vt) {}
    
// Representation
    Vertex* parent;
    std::vector<Vertex*> adj;   // Vector of pointers to adjacent/connected vertices
    std::vector<int>     prob;  // Probability values corresponding to adjacent locations
    std::string          name;  // Human-readable name of note. Because we're human.
    int note;                   // Numberical midi value of note
    int velocity;               // Velocity/volume of note
    int duration;               // Duration of note
};

class MChain {
public:
    MChain() {}
    MChain(const std::string& file);
    ~MChain();
    int getMaxNotes() const { return max_notes; }
    void runChain(vector<chanEvent> & track, int limit, int track);    // Run the chain, with a limit on the number of notes to add
    void addVertex(const std::string& name, int note, int duration, int velocity);  // Add the dots...
    void addEdge(const std::string& from, const std::string& to, int prob);         // Connect the dots!
private:
    Vertex* getVertex(const std::string& vert);
    std::map<std::string, Vertex*> vertexMap;
    std::vector<Vertex*> allVertices;
    std::string start_note;
    int max_notes;
    std::vector<Vertex*> starting_notes;       // Starting notes for successive tracks
};
