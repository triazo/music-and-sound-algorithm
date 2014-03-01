// midi.cpp
//
// A test file for writing midi.

#include <cstdlib>
#include <endian.h>
#include <iostream>
#include <fstream>

void usage(char* argv[]);
// void encodeVarint(int value, char* output, uint8_t* outputSizePtr);

void makeTrack(char* track, int& bytes);
void channelEvent(char*& event, int& bytes,
									int deltatime, int type, int channel, int p1, int  p2) {

	// Determine the number of bytes, by bitshifting the deltatime, and
	// adding a constant three for the rest of it.
	bytes = 0;
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

	// Allocate the event and fill it with data.
	char* data = new char[bytes];
	event = data;
	int count = 0;
	while (deltatime > 127) {
		// This only works because x86 is little endian...
		event[count] = ((char)(deltatime & 127)) | 128;
		deltatime >>= 7;
		count++;
	}
	// TODO: check that the result is big-endian
	event[count] = ((char)deltatime) & 127;

	event[bytes-3] = ((char)(type + 8) << 4) | ((char)channel);
	event[bytes-2] = (char)p1;
	event[bytes-1] = (char)p2;
	
}


int main(int argc, char* argv[]) {
  // Open file in binary mode.  Currently hard-coded
    
  if (argc < 2) {
		usage(argv);
		exit(1);
	}
	
	std::ofstream midifile(argv[1], std::ios::out | std::ios::binary);
	midifile.seekp (0);
	
	// The Header for a midi file
	midifile.write("MThd",4);
	// Length of 6
	int length = htobe32(6);
	midifile.write((char*)&length, 4);
	
	// 0 means one track
	short tracktype = htobe16(0);
	midifile.write((char*)& tracktype, 2);
	
	// Number of tracks (if tracktype == 0, must be 1)
	short trackcount = htobe16(1);
	midifile.write((char*)& trackcount, 2);
	
	// Number of ticks per quarter note (used by delta-time, tempo determined later?)
	short ticksperbeat = htobe16(64);
	midifile.write((char*)& ticksperbeat, 2);
	
	
	// Start the Track
	midifile.write("MTrk", 4);
	
	// Output a sample size (should be eight for this example)
	int trackLength = htobe32(12);
	midifile.write((char*)& trackLength, 4);
	
	// Generate and write the first event.
	char* event1;
	int event1Size;
	channelEvent(event1, event1Size,
							 0, 1, 0, 64, 64);
	midifile.write(event1, event1Size);
	delete event1;
	
	// Generate and write the second event.
	char* event2;
	int event2Size;
	channelEvent(event2, event2Size,
							 64, 0, 0, 64, 64);
	midifile.write(event2, event2Size);
	delete event2;

	// Add end of track (00FF2F00) event.
	char* eotEvent;
	int eotEventSize;
	// Quick hack: type 8 and track 15 makes a meta event.
	// The rest of the fields match in length.
	channelEvent(eotEvent, eotEventSize,
							 0, 7, 15, 47, 0);
	midifile.write(eotEvent, eotEventSize);
	delete eotEvent;
	
	
	
}


void usage (char* argv[]) {
  std::cout << "Usage: " << argv[0] << " outfile" << std::endl;
}

// void encodeVarint(int value, char* output, int& outputSize) {
// 	int outputSize = 0;
// 	// While more than 7 bits of data are left, occupy the last output byte
// 	// and set the next byte flag
// 	while (value > 127) {
// 		// |128: Set the next byte flag
// 		output[outputSize] = ((char)(value & 127)) | 128;
// 		// Remove the seven bits we just wrote
// 		value >>= 7;
// 		outputSize++;
// 	}
// 	output[outputSize++] = ((char)value) & 127;
// 	*outputSizePtr = outputSize;
// }

// void channelEvent(char*& event, int& bytes,
// 									int deltatime, int type, int channel, int p1, int  p2) {

// 	// Determine the number of bytes, by bitshifting the deltatime, and
// 	// adding a constant three for the rest of it.
// 	bytes = 0;
// 	int delTemp = deltatime;
// 	while (delTemp != 0) {
// 		delTemp >>= 7;
// 		bytes++;
// 	}
// 	bytes += 3;

// 	// Allocate the event and fill it with data.
// 	event = new char[bytes];
// 	int count = 0;
// 	while (deltatime > 127) {
// 		// This only works because x86 is little endian...
// 		event[count] = ((char)(deltatime & 127)) | 128;
// 		deltatime >>= 7;
// 		count++;
// 	}
// 	// TODO: check that the result is big-endian
// 	event[count] = ((char)deltatime) & 127;

// 	event[bytes-3] = ((char)(type + 8) << 4) | ((char)channel << 4);
// 	event[bytes-2] = (char)p1;
// 	event[bytes-1] = (char)p2;
// }
