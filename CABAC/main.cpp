#include <iostream>
#include "QMCODER.h"
#include <string>
#include <bitset>
#include <vector>
#include <fstream>
#include "FileReadWrite.h"
using namespace std;

long filesize(std::string filename) {
	ifstream fin;
	fin.open(filename, ios_base::in | ios_base::binary);

	long length;
	if (!fin.is_open()) {
		cout << "can not locate file!" << endl;
	}
	else {
		fin.seekg(0, fin.end);
		length = fin.tellg();
	}
	fin.close();
	return length;
}



std::string toBitString(char symbol) {
	std::string outStr = "";
	std::bitset<8> bits((int)symbol);
	outStr += bits.to_string();
	return outStr;
}
std::string bitPlaneMap(std::string inputString) {
	std::string outputString = "";
	vector<std::string> bitStringMatrix;
	for (auto symbol : inputString) {
		bitStringMatrix.push_back(toBitString(symbol));
	}
	for (int i = 0; i < 7; i++) {
		for (auto it = bitStringMatrix.begin(); it != bitStringMatrix.end(); it++) {
			outputString += (*it).at(i);
		}
	}
	return outputString;
}
std::string normalMap(std::string inputString) {
	std::string outputbitString = "";
	for (auto symbol : inputString) {
		outputbitString += toBitString(symbol);
	}
	return outputbitString;
}


std::string context = "";


int bitStrToInt(std::string bitString) {
	int a = 0;
	for (auto bit : bitString) {
		a = a * 2 + bit - '0';
	}
	return a;
}

int main(int argc, char** argv) {

	if (argc != 3) {
			cout << "Usage: CABAC.exe filename [mapping type]" << endl;
			cout << "Example: >> CABAC.exe ./data/text.dat normal" << endl << "(mapping in normal way)" << endl;
			cout << "Example: >> CABAC.exe ./data/text.dat bit" << endl << "(bit plane mapping)" << endl;
			return -1;


	}
    int n;
	cout << "input context size 1 to 3:";
	cin >> n;
	cout << n << endl;
	std::string inputFilename(argv[1]);
	char nchar = '0' + n;
	std::string outputFilename = inputFilename.substr(0, inputFilename.size() - 4)+nchar+"EncodeCABAC.dat";
	FileReadWrite file = FileReadWrite(inputFilename);
	char * buffer = file.content;
	std::string inputString;
	inputString.assign(buffer, file.length);
	std::string inputbitString;
	//inputbitString = bitPlaneMap(inputString);

	std::string argument2(argv[2]);
	if (argument2.compare("normal") == 0) {
		inputbitString = normalMap(inputString);
		cout << "normal mapping" << endl;
	}
	else {
		inputbitString = bitPlaneMap(inputString);
		cout << "bit mapping" << endl;
	}
	//write file
	FILE *fp;
	fp = fopen(outputFilename.c_str(),"w+");
	QM oneTrial(fp);
	//initiate
	oneTrial.StartQM("encode");
	//initiate context
	for (int i = 0; i < n; i++) {
		context += '0';
	}

	for(auto bit:inputbitString){
		int getContext = bitStrToInt(context);
		oneTrial.encode(bit - '0', getContext);	
		for (int i = 1; i < n ; i++) {
			context.at(i - 1) = context.at(i);
		}
		context.at(n - 1) = bit;
		//cout << "context:" << context <<":"<< getContext <<endl;
	}
	

	oneTrial.Flush();
	fclose(fp);
	cout << "coding done!" << endl;
	cout << "file saved to :" << outputFilename << endl;
	cout << "compression ratio:" << (double)(filesize(outputFilename)) / (double)(file.length) << endl;
	return 0;
}