// mchain.cpp
//
// A class file for storing markov chains

#include "mchain.h"

Vertex* MChain::getVertex(const std::string& vert) {
    std::map<std::string, Vertex*>::iterator itr
        = vertexMap.find(vert);

    if (itr == vertexMap.end()) {
        std::cout << "No vertex named " << vert << std::endl;
        exit(1);
    }
    return (*itr).second;
}

void MChain::runChain(vector<chanEvent> & track, int limit){
    Vertex* current = getVertex(start_note);
    for (int i = 0; i < limit; i++){
        track.push_back(chanEvent(current->note, current->velocity, 0, 0, 1));                   // Begin the note
        track.push_back(chanEvent(current->note, current->velocity, current->duration, 0, 0));   // End the note
        int loc = rand() % current->prob.size();
        current = current->adj[loc];
    }
    //track.push_back(endofTrack(0, 15, 8, 47, 0));
}

void MChain::addVertex(const std::string& name,
                       int note, int duration, int velocity) {
    Vertex* vert = new Vertex(name, note, duration, velocity);
    allVertices.push_back(vert);
    vertexMap.insert(std::make_pair(name, vert));
}

void MChain::addEdge(const std::string& from,
                     const std::string& to,
                     int prob) {
    Vertex* f = getVertex(from);
    Vertex* t = getVertex(to);

    f->adj.push_back(t);
    f->prob.push_back(prob);
}

MChain::~MChain() {
    for (int i = 0; i < allVertices.size(); i++)
        delete allVertices[i];
}

MChain::MChain(const std::string& file) {
    ifstream inFile(file.c_str());
    if (!inFile) {
        std::cerr << "Cannot open " << file << std::endl;
        exit(2);
    }

    // Read in the inital verticies
    std::string noteName;
    int note, duration, velocity;
    while (true) {
        inFile >> noteName;
        if (noteName == "STARTLINKS") break;
        inFile >> note >> duration >> velocity;
        addVertex(noteName, note, duration, velocity);
    }
    inFile >> start_note;
    inFile >> max_notes;
    std::string from, to;
    int probability;
    while (inFile >> from >> to >> probability) {
        addEdge(from, to, probability);
    }
}
