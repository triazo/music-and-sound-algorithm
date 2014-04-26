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

void MChain::runChain(vector<chanEvent> & track, int limit, int track_num){
    Vertex* current = starting_notes[track_num];
    for (int i = 0; i < limit; i++){

        // Add the current note to the future midi vertex.
        track.push_back(chanEvent(current->note, current->velocity, 0, 0, 1));                   // Begin the note
        track.push_back(chanEvent(current->note, current->velocity, current->duration, 0, 0));   // End the note

        // Randomly choose the next vertex

        // New array
        std::vector<int> v(current->prob.size(),0);
        v[0] = current->prob[0];
        for (int i = 1; i < v.size(); i++)
            v[i] = v[i-1] + current->prob[i];

        // March up the array until the random number is found
        int weightedloc = rand() % v[v.size()-1];
        int loc = 0;
        while (weightedloc > v[loc])
            loc++;

        // current->adj[loc]->parent = current;
        current = current->adj[loc];
    }
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
    int note, duration, velocity, num_starts;
    while (true) {
        inFile >> noteName;
        if (noteName == "STARTLINKS") break;
        inFile >> note >> duration >> velocity;
        addVertex(noteName, note, duration, velocity);
    }
    inFile >> num_starts;
    for (int i = 0; i < num_starts; i++){
        inFile >> noteName;
        starting_notes.push_back(getVertex(noteName));
    }
    inFile >> max_notes;
    std::string from, to;
    int probability;
    while (inFile >> from >> to >> probability) {
        addEdge(from, to, probability);
    }
}
