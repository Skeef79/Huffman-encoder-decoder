#include"bstream.h"

using namespace std;

typedef short charT;

ibstream::ibstream(string file) :
	currentBit(0),
	index(BUFF_SIZE),
	currentSize(BUFF_SIZE) {
	input.open(file, ios::binary);
	buffer.resize(BUFF_SIZE);
	update();
}

ibstream::~ibstream() {
	input.close();
}

void ibstream::update() {
	input.read(buffer.data(), size(buffer));
	std::streamsize dataSize = input.gcount();
	currentSize = dataSize;
	index = 0;
	currentBit = 0;
}

int ibstream::readBit() {
	if (input.eof() && currentSize == 0)
		return -1;

	int bit = (buffer[index] >> currentBit) & 1;
	currentBit++;
	if (currentBit == 8) {
		currentBit = 0;
		index++;
	}

	if (index == currentSize)
		update();

	return bit;
}

void ibstream::rewind() {
	input.clear();
	input.seekg(0, std::ios::beg);

	currentBit = 0;
	index = 0;
	currentSize = BUFF_SIZE;
	update();
}

unsigned int ibstream::readInt() {
	unsigned int value = 0;
	int bit;
	for (int i = 0; i < sizeof(int) * 8; i++) {
		bit = readBit();
		value ^= (bit << i);
	}
	return value;
}

char ibstream::readChar() {
	char value = 0;
	int bit;
	for (int i = 0; i < sizeof(char) * 8; i++) {
		bit = readBit();
		value ^= (bit << i);
	}
	return value;
}

bool ibstream::eof() {
	return input.eof() && currentSize == 0;
}

//*-------------------------*
obstream::obstream(string file) :
	currentBit(0),
	index(0),
	currentSize(BUFF_SIZE) {
	out.open(file, ios::binary);
	buffer.resize(BUFF_SIZE);
}

obstream::~obstream() {
	out.close();
}

void obstream::update() {
	out.write(buffer.data(), currentSize);
	fill(buffer.begin(), buffer.end(), 0);
	index = 0;
	currentBit = 0;
	currentSize = BUFF_SIZE;
}

void obstream::writeBit(int bit) {
	buffer[index] ^= (bit << currentBit);
	currentBit++;
	if (currentBit == 8) {
		currentBit = 0;
		index++;
	}

	if (index == currentSize)
		update();
}

void obstream::finish() {
	if (currentBit != 0)
		index++;
	out.write(buffer.data(), index);
}

void obstream::writeInt(unsigned int value) {
	for (int i = 0; i < sizeof(int) * 8; i++)
		writeBit((value >> i) & 1);
}

void obstream::writeChar(char value) {
	for (int i = 0; i < sizeof(char) * 8; i++)
		writeBit((value >> i) & 1);
}