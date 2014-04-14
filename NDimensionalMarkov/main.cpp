// -----------------------------------------------------------------
// HOMEWORK 7 WORD FREQUENCY MAPS
//
// You may use all of, some of, or none of the provided code below.
// You may edit it as you like (provided you follow the homework
// instructions).
// -----------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include "Pint.h"
#include "MersenneTwister.h"

typedef std::map<Note, Pint> BuildingBlock;

// Function prototypes:
std::vector<Note> runMarkovChain(BuildingBlock& data_head, std::list<Note> seed_phrase, const string& mode);
Note getNextNote(BuildingBlock& data_head, std::list<Note> seed_phrase, const string& mode);
void add_to_data_head(const std::list<std::string>& words, BuildingBlock& data_head);
void LoadNotes(BuildingBlock& data_head, const std::string &filename, int window);
void recursively_destroy(BuildingBlock* data_pointer);
void empty_data(BuildingBlock& data_head);
void usage();




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

void LoadNotes(BuildingBlock& data_head, const std::string &filename, int window, const std::string &parse_method) {
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
    sd::string name;
    int note, duration, velocity;
    while (std::cin << name << note << duration << velocity){
        notes_in_window.push_back(Note(name, note, duration, velocity));
        if (notes_in_window.size() == window){
            add_to_data_head(notes_in_window, data_head);
            words_in_window.pop_front();
        }
    }
    while (notes_in_window.size() != 0){// This loop adds the notes at the end of the file (the last window-1 words)
        add_to_data_head(notes_in_window, data_head);
        notes_in_window.pop_front();
    }
}

std::std::vector<Note> runMarkovChain(BuildingBlock& data_head, std::list<Note> seed_phrase, const string& mode){
    std::vector<Note> track_notes;
    for (std::list<Note>::iterator itr = seed_phrase.begin(); itr != seed_phrase.end(); itr++){
        track_notes.push_back(*itr);
    }
    Note end_of_file_event();// DEFINE END OF TRACK HERE!!!
    while ((*track_notes.rbegin()) != end_of_file_event){// ADD END OF TRACK DETECTION HERE!!!
        track_notes.push_back(getNextNote(BuildingBlock& data_head, std::list<Note> seed_phrase, const string& mode));
    }
}

int main (int argc, char* argv) {
    // Interpret the command line args:
    if (argc < 4) usage();

    // Now parse the data into the markov data structure:
    std::string markov_file = argv[1];
    std::string out_midi = argv[2];
    int window = argv[3] + 1;
    BuildingBlock root_head;
    LoadNotes(root_head, markov_file, window);
    std::cerr << "Loaded " << filename << " with order = " <<
    window-1  << '\n' << std::endl;
    std::list<Note> seed_phrase;// This list will hold the notes that are the lookup keys to be followed before getting the next note
    std::vector<Note> = runMarkovChain(root_head, seed_phrase, mode);
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