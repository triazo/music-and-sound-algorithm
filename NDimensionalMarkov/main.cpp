#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include "event.h"
#include "mchain.h"
#include "Pint.h"
#include "MersenneTwister.h"
#if defined(OS_MACOSX)
#include <endian.h>
#else
#include "endian.h"
#endif

typedef std::map<Note, Pint> BuildingBlock;

// Function prototypes:
std::vector<Note> runMarkovChain(BuildingBlock& data_head, int order, const string& mode);
Note getNextNote(BuildingBlock& data_head, std::list<Note> seed_phrase, int order);
void add_to_data_head(const std::list<std::string>& words, BuildingBlock& data_head);
void LoadNotes(BuildingBlock& data_head, const std::string &filename, int window);
void recursively_destroy(BuildingBlock* data_pointer);
void empty_data(BuildingBlock& data_head);
void writeMidiFile(std::string outfile, const std::vector<Note>& notes);
void compile_track(const vector<chanEvent> & track, ofstream & midifile);
void makeTrack(int& bytes, std::ofstream& midifile);
void usage();

void compile_track(const vector<chanEvent> & track, ofstream & midifile){
    for (int i = 0; i < track.size(); i++){
        midifile.write(track[i].getMidi().c_str(),track[i].getBytes());
    }
    int endOfTrack = 0x00FF2F00;
    midifile.write((char*)&endOfTrack,4);
}

void makeTrack(int& bytes, std::ofstream& midifile){
    // Start the Track
    midifile.write("MTrk", 4);
    // Output a sample size (number of bytes)
    int trackLength = htobe32(bytes);
    midifile.write((char*)& trackLength, 4);
}

void writeMidiFile(std::string outfile, const std::vector<Note>& notes){
    // Make midifile out-stream to write to
    std::ofstream midifile(outfile, std::ios::out | std::ios::binary);
    midifile.seekp (0);
    // The header for a midi file
    midifile.write("MThd",4);
    // Length of 6
    int length = htobe32(6);
    // Tricking the compiler to typecast the length (previous line) to char array
    midifile.write((char*)&length, 4);
    // Header set-up information processing
    short tracktype = htobe16(0);
    short trackcount = htobe16(1);
    // Tricking the compiler again...won't label further tricks
    midifile.write((char*)& tracktype, 2);
    // Number of tracks (if tracktype == 0, must be 1)
    midifile.write((char*)& trackcount, 2);
    // Number of ticks per quarter note (used by delta-time, tempo determined later?)
    short ticksperbeat = htobe16(64);
    midifile.write((char*)& ticksperbeat, 2);
    std::cout << "Ticks per quarter note has been set to a default value of 64" << std::endl;
    // Make the tracks, with their events:
    std::cout << "Running chain..." << endl;
    vector<chanEvent> track;
    for (int i = 0; i < notes.size(); i++){// Turn the notes to channel events
        track.push_back(chanEvent(notes[i].note, notes[i].velocity, 0, 0, 1));
        track.push_back(chanEvent(notes[i].note, notes[i].velocity, notes[i].duration, 0, 0));
    }
    length = 0;
    for (int i = 0; i < track.size(); i++)
        length += track[i].getBytes();
    length += 4;
    makeTrack(length, midifile);
    compile_track(track, midifile);
    midifile.close();
}


int main (int argc, char** argv) {
    // Interpret the command line args:
    if (argc < 4) usage();
    
    // Now parse the data into the markov data structure:
    std::string markov_file = argv[1];
    std::string out_midi = argv[2];
    int window = atoi(argv[3]) + 1;
    BuildingBlock root_head;
    LoadNotes(root_head, markov_file, window);
    std::cerr << "Loaded " << markov_file << " with order = " <<
    window-1  << '\n' << std::endl;
    std::string mode;
    std::vector<Note> track = runMarkovChain(root_head, window-1, mode);
    // Now compile all the notes into a midi file
    writeMidiFile(out_midi, track);
    
}

//#############################################################################################################
// Finished code, no changes needed:

void usage(){
    std::cout << "program_name input_markov_file midi_outfile markov_order -options" << std::endl;
}

// Head function to return all heap memory from data container
void empty_data(BuildingBlock& data_head){
    BuildingBlock::iterator m_itr = data_head.begin();
    for (m_itr; m_itr != data_head.end(); m_itr++){
        if (m_itr->second.first != NULL){// If the pointer leads somewhere...
            recursively_destroy(m_itr->second.first);// Release the hounds!
        }
    }
}

// Recursive helper function to return heap memory from data conatiner
void recursively_destroy(BuildingBlock* data_pointer){
    if (data_pointer != NULL){
        BuildingBlock::iterator m_itr = data_pointer->begin();
        for (m_itr; m_itr != data_pointer->end(); m_itr++){
            if (m_itr->second.first != NULL){// If the pointer leads somewhere...
                recursively_destroy(m_itr->second.first);// Recurse!
            }
        }
        delete data_pointer;// Once all sub-pointers are deleted, delete this one
    }
    // If the pointer is NULL, nothing happens
}

void add_to_data_head(const std::list<Note>& notes, BuildingBlock& data_head){
    std::list<Note>::const_iterator itr = notes.begin();
    BuildingBlock* ptr = &data_head;
    int size = int(notes.size());
    for (int i = 0; i < size; i++){
        ptr->insert(std::make_pair(*itr, Pint()));  // Insert the map entry in case it's not already there, with value 0
        (*ptr)[*itr].second+=1;                      // Increment the value
        if ((*ptr)[*itr].first == NULL && i != size-1)// If the next map block doesn't exist, add it
            (*ptr)[*itr].first = new BuildingBlock; // If a map entry doesn't have a map in the next level, add it
        ptr = (*ptr)[*itr].first;                   // Move ptr up to the next level for the next iteration
        itr++;                                      // Increment the list iterator to the next word
    }
}

void LoadNotes(BuildingBlock& data_head, const std::string &filename, int window) {
    // open the file stream, check for errors:
    std::ifstream istr(filename.c_str());
    if (!istr) {
        std::cerr << "ERROR cannot open file: " << filename << std::endl;
        std::exit(1);
    }
    // verify the window parameter is appropriate
    if (window < 2) {
        std::cerr << "ERROR Markov order size must be greater than or equal to 1:" << window-1 << std::endl;
    }
    
    // Read in the note data:
    std::list<Note> notes_in_window;
    std::string name;
    int note, duration, velocity;
    while (std::cin >> name >> note >> duration >> velocity){
        notes_in_window.push_back(Note(name, note, duration, velocity));
        if (notes_in_window.size() == window){
            add_to_data_head(notes_in_window, data_head);
            notes_in_window.pop_front();
        }
    }
    while (notes_in_window.size() != 0){// This loop adds the notes at the end of the file (the last window-1 words)
        add_to_data_head(notes_in_window, data_head);
        notes_in_window.pop_front();
    }
}

std::vector<Note> runMarkovChain(BuildingBlock& data_head, int order, const string& mode){
    std::vector<Note> track_notes;
    std::list<Note> seed_phrase;// This list will hold the notes that are the lookup keys to be followed before getting the next note
    BuildingBlock::iterator itr = data_head.begin();
    std::vector<Note> notes_by_weight;
    for (; itr != data_head.end(); itr++){
        for (int i = 0; i < (*itr).second.second; i++){
            notes_by_weight.push_back((*itr).first);
        }
    }
    MTRand mtrand_autoseed;
    int random = mtrand_autoseed.randInt(notes_by_weight.size()-1);
    for (int i = 0; i < random; i++){
        seed_phrase.push_back(notes_by_weight[i]);
    }
    for (std::list<Note>::iterator itr = seed_phrase.begin(); itr != seed_phrase.end(); itr++){
        track_notes.push_back(*itr);
    }
    while ((*track_notes.rbegin()).name != "EOT" && (*track_notes.rbegin()).note != -1 &&
           (*track_notes.rbegin()).velocity != -1 && (*track_notes.rbegin()).duration != -1){
        track_notes.push_back(getNextNote(data_head, seed_phrase, order));
    }
    track_notes.pop_back();// get rid of the eot marker
    return track_notes;
}

Note getNextNote(BuildingBlock& data_head, std::list<Note> seed_phrase, int order){
    std::list<Note>::iterator itr = seed_phrase.begin();
    BuildingBlock::iterator m_itr;
    BuildingBlock* ptr = &data_head;
    while(itr != seed_phrase.end()){    // This while loop positions ptr to point at the level from which new notes will be drawn
        if (ptr == NULL){               // (e.g if there are two starting notes, the notes will come from level three) <--'
            std::cerr << "That's too many levels! Too many notes for the number of levels!" << std::endl;
            std::exit(1);
        }
        m_itr = ptr->find(*itr);        //Look for the current note of the seed phrase in the current level, set m_itr to that map entry
        if (m_itr == ptr->end()){
            std::cerr << "Note not found for the progression: " << std::endl;
            std::exit(1);
        }
        ptr = m_itr->second.first;      // Point to the next map (level) in the progression
        itr++;                          // Move to the next word in the phrase list
    }
    if (ptr == NULL){                   // If there are no more maps after the current one, then there are
        std::cerr << "No more notes! Can't return one!" << std::endl;// no more notes, so none can be found
        std::exit(1);
    }
    // Random word choosing:
    std::vector<Note> notes_by_weight; // Vector of notes to which next notes will be added by weight
    m_itr = ptr->begin();           // Point m_itr to the first entry in the map that the word to be found is in
    while (m_itr != ptr->end()){
        for (int i = 0; i < m_itr->second.second; i++){
            notes_by_weight.push_back(m_itr->first);
        }
        m_itr++;
    }
    m_itr = ptr->begin();           // Reset m_itr to start of map
    MTRand mtrand_autoseed;
    int random = mtrand_autoseed.randInt(notes_by_weight.size()-1);// Use size-1, else the range will be size + 1 (from 0 to size)
    if (seed_phrase.size() < order){
        seed_phrase.push_back(notes_by_weight[random]);
    }
    if (seed_phrase.size() == order){
        seed_phrase.push_back(notes_by_weight[random]);
        seed_phrase.pop_front();
    }
    return notes_by_weight[random];            // Return the random note
}
