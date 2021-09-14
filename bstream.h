#include <istream>
#include <ostream>
#include <fstream>
#include <sstream>
#include<vector>
#include<string>

using namespace std;
const int BUFF_SIZE = 1 << 20;
typedef short charT;

class ibstream {
private:
	ifstream input;
	vector<char>buffer;
	int currentBit, index, currentSize;

	void update();

public:
	ibstream(string file);
	~ibstream();
	void rewind();
	int readBit();
	unsigned int readInt();
	char readChar();
	bool eof();
};

class obstream {
private:
	ofstream out;
	vector<char>buffer;
	int currentBit, index, currentSize;

	void update();

public:
	obstream(string file);
	~obstream();
	void writeBit(int bit);
	void writeInt(unsigned int value);
	void writeChar(char value);
	void finish();
};