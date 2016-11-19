/* On my honor, I have neither given nor received unauthorized aid on this assignment.
	Name: Shanfang zhao
	UFID: 4334-1163
*/
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h> // for strtoull
#include <sstream>
#include <math.h>// for power function

using namespace std;
int registers[32];
stringstream ss;

//calculate offset
int calOffset16(string str) {
	int offset = strtol(str.c_str(), NULL, 2);
    if(str.substr(0, 1) == "1") {
        offset = offset - pow(2, 16);
    }
    return offset;
}

//decoding branch instructions
string branch(string line) {
	string result = "";
	int rs = strtoul(line.substr(6, 5).c_str(), NULL, 2);
    ss << rs;
	string RS = ss.str();
	ss.str(string());
	int rt = strtoul(line.substr(11,5).c_str(), NULL, 2);
    ss << rt;
    string RT = ss.str();
    ss.str(string());
	int offset = calOffset16(line.substr(16, 16));
	ss << offset;
	string strOff = ss.str();
    ss.str(string());
	result = "R" + RS + ", " + "R" + RT + ", " + "#" + strOff;
	return result;
}

//decoding SW and LW
string loadStore(string line) {
	int base = strtoul(line.substr(6, 5).c_str(), NULL, 2);
	ss << base;
    string Base = ss.str();
	ss.str(string());
	int rt = strtoul(line.substr(11,5).c_str(), NULL, 2);
	ss << rt;
    string RT = ss.str();
	ss.str(string());
	int offset = calOffset16(line.substr(16, 16));
	ss << offset;
	string strOff = ss.str();
	ss.str(string());
	string result = "R" + RT + ", " + strOff + "(R" + Base + ")";
	return result;
}

//decode cat1 instructions
string cat1(string line) {
	string instruction = "";
	string operation = "";

	//get the opcode
	string opcode = line.substr(3,3);
	if(opcode == "000") {
		instruction = "NOP";
	}
	else if(opcode == "001") {
		string bits26 = line.substr(6, 26);
		string bits28 = bits26.append("00");

		/*for this specific project, PC is a small number, so we just calculate the 28 bits,
		ignoring the four 0s in MSB. But this is not exactly what happens in processor.
		To follow the logic in processor, we can let X = 0xF0000000 &(pc+4), Y = offset << 2;
		then get newPC = X|Y
		*/
	    int newPC = strtol(bits28.c_str(), NULL, 2);
		operation = "J #";
		ss << newPC;
		instruction = operation + ss.str();
	    ss.str(string());
	}
	else if(opcode == "010") {
		//BEQ
		operation = "BEQ ";
		instruction = operation.append(branch(line));
	}
	else if(opcode == "011") {
		//BNE
		operation = "BNE ";
		instruction = operation.append(branch(line));
	}
	else if(opcode == "100") {
		//BGTZ
		operation = "BGTZ ";
		int rs = strtoul(line.substr(6, 5).c_str(), NULL, 2);
		ss << rs;
		string RS = ss.str();
    	ss.str(string());
	    int offset = calOffset16(line.substr(16, 16));
		//long long int offset = strtoll(line.substr(16, 16).c_str(), NULL, 2);
		ss << offset;
		instruction = operation + "R" + RS + ", " + "#" + ss.str();
    	ss.str(string());
	}
	else if(opcode == "101") {
		//SW
		operation = "SW ";
		instruction = operation.append(loadStore(line));
	}
	else if(opcode == "110") {
		//LW
		operation = "LW ";
		instruction = operation.append(loadStore(line));
	}
	else {
		//BREAK
		instruction = "BREAK";

	}
	return instruction;
}

int main() {
/*int main(int argc, char* argv[]) {
	ifstream inFile;
	if(argc == 2) {
		inFile.open(argv[1]);
		if(inFile.fail()) {
			cerr << "Error when opening file.\n";
			exit(1);
		}
	}
	else {
		cout << "\nInvalid input, please enter the executable name and the test file name\n";
	}*/

	//open the two output files
	ofstream outFile1;
	outFile1.open("disassembly.txt");
	ofstream outFile2;
	outFile2.open("simulation.txt");

	ifstream inFile;
	inFile.open("sample.txt");

	//read input and decode it
	string line;
	int address = 64;
	string instruction;

	while(getline(inFile, line)) {
		if(line.substr(0,3) == "001") {
			instruction = cat1(line);
			outFile1 << line << "\t" << address << "\t" << instruction << "\n";
		}
		else if(line.substr(0,3) == "010") {
			//cat2(line);
		}
		else {
			//cat3(line);
		}
		address += 4;
	}
	inFile.close();
	outFile1.close();
	outFile2.close();
	return 0;
}
