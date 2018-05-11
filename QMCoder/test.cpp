#include <iostream>
#include <string>
#include <bitset>
#include "FileReadWrite.h"
#include <vector>
using namespace std;

int dSta[46] = { 0,1,1,1,1,1,1,1,2,1,2,1,1,2,1,2,1,2,2,1,2,2,2,2,1,2,2,2,2,2,2,2,2,2,2,1,2,2,2,2,2,3,2,2,2,1 };
int iSta[46] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0 };
unsigned int qes[46] = { 0x59EB,0x5522,0x504F,0x4B85,0x4639,0x415E,0x3C3D,0x375E,0x32B4,0x2E17,0x299A,0x2516,0x1EDF,0x1AA9,0x174E,0x1424,0x119C,0x0F6B,0x0D51,0x0BB6,0x0A40,0x0861,0x0706,0x05CD,0x04DE,0x040F,0x0363,0x02D4,0x025C,0x01F8,0x01A4,0x0160,0x0125,0x00F6,0x00CB,0x00AB,0x008F,0x0068,0x004E,0x003B,0x002C,0x001A,0x000D,0x0006,0x0003,0x0001 };
unsigned int a = 0x10000;
unsigned short c = 0x0000;
unsigned int qe = 0;
int mps = 0;
//int lps = 1; 
bool carry = false;
int cSta;
std::string outBitString = "";
//input initial situation


void showStatus(bool show) {
	if (show) {
		//------------------show update process------------------//
		cout << "current state:" << cSta << endl;
		printf("A:0x%x\t  ", a);
		printf("C:0x%x\t  ", c);
		printf("Qe:0x%x\t ", qe);
		cout << "MPS:" << mps << endl;
		//=======================================================//
	}
}
void renormalize() {
	if (carry == true) {
		outBitString += '1';
		carry = false;
	}
	while (a < 0x8000) {
		a <<= 1;
		outBitString += ('0' + c / 0x8000);
		c <<= 1;
	}
}
void encodeMPS() {
	a -= qe;
	if (a < 0x8000) {
		if (a < qe) {
			if (c + a > 0xffff) {
				carry = true;
			}
			c += a;
			a = qe;
		}
		cSta += iSta[cSta];
		qe = qes[cSta];
		renormalize();
	}
	showStatus(true);
}
void encodeLPS() {
	a -= qe;
	if (a >= qe) {
		if (c + a > 0xffff) {
			carry = true;
		}
		c += a;
		a = qe;
	}

	cSta -= dSta[cSta];
	qe = qes[cSta];
	if (dSta[cSta] == 0) {
		mps = 1 - mps;
	}
	renormalize();
	showStatus(true);
}
void encode(std::string bitString, unsigned char mpsSymbol, int cState) {
	mps = mpsSymbol - '0';
	cSta = cState;
	qe = qes[cSta];//initial state of qe
	for (auto charac : bitString) {
		if (charac == '0' + mps) {
			encodeMPS();
		}
		else {
			encodeLPS();
		}
	}
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

int main(int argc, char** argv) {
	if (argc == 1) {
		string test = "0100110010";
		encode(test, '0', 12);
		return 0;
	}




		//=========================main access===============================//

	if (argc != 3) {
		cout << "Usage: QMCoder.exe filename [mapping type]" << endl;
		cout << "Example: >> QMCoder.exe ./data/text.dat normal" << endl << "mapping in normal way" << endl;
		cout << "Example: >> QMCoder.exe ./data/text.dat bit" << endl << "bit plane mapping" << endl;
		return -1;
	}

		std::string argument(argv[1]);
		FileReadWrite file = FileReadWrite(argument);
		char * buffer = file.content;
		std::string inputString;
		inputString.assign(buffer, file.length);
		std::string inputbitString;
		//==================================================================//



		//=========================test====================================//
		//FileReadWrite file = FileReadWrite("../data/image.dat");
		//char * buffer = file.content;
		//std::string inputString;
		//inputString.assign(buffer, file.length);
		//std::string inputbitString;
		//================================================================//


		std::string argument2(argv[2]);
	if (argument2.compare("normal")==0) {
		inputbitString = normalMap(inputString);
	}
	else {
		inputbitString = bitPlaneMap(inputString);
	}
	encode(inputbitString,'0',12);
	std::cout << "output size(byte):" <<outBitString.size()/8 << endl;
	std::cout << "compression ratio:" << (double)(outBitString.size() / 8) / (double)file.length;


	return 0;
}