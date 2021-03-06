#include<iostream>
#include<fstream>
#include"bstream.h"
#include<unordered_map>
#include<set>
#include<functional>
#include<string>
#include<time.h>
#include<map>
#include<assert.h>
#include<filesystem>

using namespace std;
typedef string codeT;
typedef short charT;
const charT TERM = 257;

class HuffmanNode {
public:
	int freq;
	charT c;
	bool isLeaf;
	HuffmanNode* left;
	HuffmanNode* right;
	HuffmanNode(int freq, charT c, bool isLeaf) :
		freq(freq),
		c(c),
		isLeaf(isLeaf),
		left(nullptr),
		right(nullptr) {}
};

void freeTree(HuffmanNode* node) {
	if (!node)
		return;

	freeTree(node->left);
	freeTree(node->right);

	delete node;
}

unordered_map<charT, int> buildFrequencyTable(ibstream &input) {
	charT c;
	unordered_map<charT, int> freqMap;
	while (!input.eof()) {
		c = input.readChar();
		freqMap[c]++;
	}
	freqMap[TERM]++;
	return freqMap;
}

HuffmanNode* buildEncodingTree(const unordered_map<charT, int> &freqTable) {
	auto cmp = [](HuffmanNode* a, HuffmanNode* b) {
		return a->freq < b->freq;
	};

	auto merge = [](HuffmanNode*a, HuffmanNode* b) {
		auto newNode = new HuffmanNode(a->freq + b->freq, TERM, false);
		newNode->left = a;
		newNode->right = b;
		return newNode;
	};

	multiset<HuffmanNode*, decltype(cmp)> nodes(cmp);
	for (auto[c, freq] : freqTable)
		nodes.insert(new HuffmanNode(freq, c, true));

	while (size(nodes) > 1) {
		auto leftNode = *nodes.begin();
		nodes.erase(nodes.begin());
		auto rightNode = *nodes.begin();
		nodes.erase(nodes.begin());
		nodes.insert(merge(leftNode, rightNode));
	}

	return *nodes.begin();
}

unordered_map<charT, codeT> buildEncodingMap(HuffmanNode* encTree) {
	unordered_map<charT, codeT> encodingMap;

	function<void(HuffmanNode*, codeT)> traverse = [&](HuffmanNode* node, codeT binaryValue) {
		if (!node)
			return;
		if (node->isLeaf) {
			encodingMap[node->c] = binaryValue;
			return;
		}

		traverse(node->left, binaryValue + "1");
		traverse(node->right, binaryValue + "0");
	};

	traverse(encTree, "");
	return encodingMap;
}

inline void writeCode(obstream &out, const codeT &value) {
	for (auto b : value)
		out.writeBit(int(b - '0'));
}

void writeHeader(unordered_map<charT, int> &freqTable, obstream &out) {
	unsigned int sz = size(freqTable) - 1;
	out.writeInt(sz);
	out.writeChar(' ');

	for (auto[c, freq] : freqTable) {
		if (c == TERM)
			continue;

		out.writeChar(c);
		out.writeChar(' ');
		out.writeInt(freq);
		out.writeChar(' ');
	}
}

unordered_map<charT, int> readHeader(ibstream &input) {
	unsigned int size;
	size = input.readInt();
	input.readChar();

	unordered_map<charT, int> freqMap;
	for (int i = 0; i < size; i++) {
		charT c = input.readChar();
		input.readChar();
		unsigned int freq;
		freq = input.readInt();
		input.readChar();
		freqMap[c] = freq;
	}
	freqMap[TERM]++;
	return freqMap;
}

void encodeData(ibstream &input, unordered_map<charT, codeT> &encodingMap, obstream &out) {
	charT c;
	while (!input.eof()) {
		c = input.readChar();
		writeCode(out, encodingMap[c]);
	}
	writeCode(out, encodingMap[TERM]);
	out.finish();
}

void decodeData(ibstream &input, HuffmanNode* node, obstream &out) {
	HuffmanNode* root = node;
	int b;

	while (true) {
		b = input.readBit();
		if (b == -1)
			break;

		if (b == 1) {
			if (node->left)
				node = node->left;
			else {
				if (node->c == TERM)
					break;
				out.writeChar(node->c);
				node = root->left;
			}
		}
		else {
			if (node->right)
				node = node->right;
			else {
				if (node->c == TERM)
					break;
				out.writeChar(node->c);
				node = root->right;
			}
		}
	}

	out.finish();
}

void compress(ibstream &input, obstream &out) {
	auto freqTable = buildFrequencyTable(input);
	auto tree = buildEncodingTree(freqTable);
	auto encodingMap = buildEncodingMap(tree);

	writeHeader(freqTable, out);
	input.rewind();
	encodeData(input, encodingMap, out);
	freeTree(tree);
}

void decompress(ibstream &input, obstream &out) {
	auto freqTable = readHeader(input);
	auto tree = buildEncodingTree(freqTable);

	decodeData(input, tree, out);
	freeTree(tree);
}

int main(int argc, char* argv[]) {

	/*auto time = clock();
	ibstream bin("igor.txt");
	obstream bout("result.huff");

	compress(bin, bout);

	cout << (double)(clock() - time) / 1000;
	*/

	/*auto time = clock();
	ibstream bin("result.huff");
	obstream bout("igor_enc.txt");

	decompress(bin, bout);
	cout << (double)(clock() - time) / 1000;*/

	string command, source, destination;
	if (argc != 4 || (command = argv[1]) != "zip" && (command = argv[1]) != "unzip") {
		cout << "Syntax:" << endl;
		cout << "huffman.exe zip <source file> <destination file>" << endl;
		cout << "huffman.exe unzip <source file> <destination file>" << endl;
		cout << "example: huffmax.exe zip text.txt text.huff" << endl;
		return -1;
	}

	if (command == "zip") {
		source = argv[2];
		destination = argv[3];
		if (!filesystem::exists(source)) {
			cout << "source file doesn't exist" << endl;
			return -1;
		}

		auto start_time = clock();
		cout << "Compression started..." << endl;
		ibstream bin(source);
		obstream bout(destination);
		compress(bin, bout);
		cout << "Compression took: " << fixed << setprecision(3) << (double)(clock() - start_time) / 1000 << "s." << endl;
	}
	else {
		source = argv[2];
		destination = argv[3];
		if (!filesystem::exists(source)) {
			cout << "source file doesn't exist" << endl;
			return -1;
		}
		auto start_time = clock();
		cout << "Decompression started..." << endl;
		ibstream bin(source);
		obstream bout(destination);
		decompress(bin, bout);
		cout << "Decompression took: " << fixed << setprecision(3) << (double)(clock() - start_time) / 1000 << "s." << endl;
	}
	return 0;
}
