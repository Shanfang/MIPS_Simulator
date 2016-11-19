/* On my honor, I have neither given nor received unauthorized aid on this assignment.
	Name: Shanfang zhao
	UFID: 4334-1163
*/
#include <iostream>
#include "MIPSsim.h"
#include <fstream>
#include <string>
#include <stdlib.h> // for strtoull
#include <sstream>
#include <math.h>// for power function
#include <vector>

using namespace std;
int registers[32];
vector<Node> nodes;
stringstream ss;

//calculate offset
int signExt16(string str) {
	long offset = strtoul(str.c_str(), NULL, 2);
    if(str.substr(0, 1) == "1") {
        offset = offset - pow(2, 16);
    }
    return offset;
}

//decode cat1 instructions
string cat1(string line) {
	string instruction;
    int opcode = strtoul(line.substr(3,3).c_str(), NULL, 2);
    //int cat = 1;
	cout << opcode << endl;
	if(opcode == 0) {
        //NOP
		instruction = "NOP";
		int opcode = 0;
        Node newNode(1, opcode, 0, 0, 0);
        nodes.push_back(newNode);
	}
    else if(opcode == 1) {
	    //Jump
		string bits26 = line.substr(6, 26);
		string bits28 = bits26.append("00");

		/*for this specific project, PC is a small number, so we just calculate the 28 bits,
		ignoring the four 0s in MSB. But this is not exactly what happens in processor.
		To follow the logic in processor, we can let X = 0xF0000000 &(pc+4), Y = offset << 2;
		then get newPC = X|Y
		*/
	    long newPC = strtoul(bits28.c_str(), NULL, 2);
		ss << newPC;
		instruction = "J #" + ss.str();
	    ss.str(string());

        Node newNode(1, opcode, 0, 0, newPC);
        nodes.push_back(newNode);
	}
	else if(opcode == 2) {
		//BEQ
        int rs = strtoul(line.substr(6, 5).c_str(), NULL, 2);
        ss << rs;
        string RS = ss.str();
        ss.str(string());
        int rt = strtoul(line.substr(11,5).c_str(), NULL, 2);
        ss << rt;
        string RT = ss.str();
        ss.str(string());
        long offset = signExt16(line.substr(16, 16));
        ss << offset;
        string strOff = ss.str();
        ss.str(string());
        instruction = "BEQ R" + RS + ", " + "R" + RT + ", " + "#" + strOff;
        Node newNode(1, opcode, rs, rt, offset);
        nodes.push_back(newNode);
	}
	else if(opcode == 3) {
		//BNE
        int rs = strtoul(line.substr(6, 5).c_str(), NULL, 2);
        ss << rs;
        string RS = ss.str();
        ss.str(string());
        int rt = strtoul(line.substr(11,5).c_str(), NULL, 2);
        ss << rt;
        string RT = ss.str();
        ss.str(string());
        long offset = signExt16(line.substr(16, 16));
        ss << offset;
        string strOff = ss.str();
        ss.str(string());
        instruction = "BNE R" + RS + ", " + "R" + RT + ", " + "#" + strOff;
        Node newNode(1, opcode, rs, rt, offset);
        nodes.push_back(newNode);
	}
	else if(opcode == 4) {
		//BGTZ
		int rs = strtoul(line.substr(6, 5).c_str(), NULL, 2);
		ss << rs;
		string RS = ss.str();
    	ss.str(string());
	    long offset = signExt16(line.substr(16, 16));
		ss << offset;
		instruction = "BGTZ R" + RS + ", " + "#" + ss.str();
    	ss.str(string());
        Node newNode(1, opcode, rs, 0, offset);
        nodes.push_back(newNode);
	}
	else if(opcode == 5) {
		//SW
        int base = strtoul(line.substr(6, 5).c_str(), NULL, 2);
        ss << base;
        string Base = ss.str();
        ss.str(string());
        int rt = strtoul(line.substr(11,5).c_str(), NULL, 2);
        ss << rt;
        string RT = ss.str();
        ss.str(string());
        long offset = signExt16(line.substr(16, 16));
        ss << offset;
        string strOff = ss.str();
        ss.str(string());
        instruction = "SW R" + RT + ", " + strOff + "(R" + Base + ")";
        Node newNode(1, opcode, base, rt, offset);
        nodes.push_back(newNode);
	}
	else if(opcode == 6) {
		//LW
        int base = strtoul(line.substr(6, 5).c_str(), NULL, 2);
        ss << base;
        string Base = ss.str();
        ss.str(string());
        int rt = strtoul(line.substr(11,5).c_str(), NULL, 2);
        ss << rt;
        string RT = ss.str();
        ss.str(string());
        long offset = signExt16(line.substr(16, 16));
        ss << offset;
        string strOff = ss.str();
        ss.str(string());
        instruction = "LW R" + RT + ", " + strOff + "(R" + Base + ")";
        Node newNode(1, opcode, base, rt, offset);
        nodes.push_back(newNode);
	}
	else {
		//BREAK
		instruction = "BREAK";
        Node newNode(1, opcode, 0, 0, 0);
        nodes.push_back(newNode);
	}
	return instruction;
}


string cat2(string line) {
	string instruction;
    int opcode = strtoul(line.substr(3,3).c_str(), NULL, 2);
	int dest = strtoul(line.substr(6,5).c_str(), NULL, 2);
	int src1 = strtoul(line.substr(11,5).c_str(), NULL, 2);
	int src2 = strtoul(line.substr(16,5).c_str(), NULL, 2);
    //int cat = 2;

    //convert int to string
    ss << dest;
    string Dest = ss.str();
    ss.str(string());
    ss << src1;
    string Src1 = ss.str();
    ss.str(string());
    ss << src2;
    string Src2 = ss.str();
    ss.str(string());

    if(opcode == 0) {
    //XOR
    instruction = "XOR R" + Dest + ", " + "R" + Src1 + ", " +  + "R" + Src2;
    Node newNode(2, opcode, dest, src1, src2);
    nodes.push_back(newNode);
    }
    else if(opcode == 1) {
    //MUL
    instruction = "MUL R" + Dest + ", " + "R" + Src1 + ", " +  + "R" + Src2;
    Node newNode(2, opcode, dest, src1, src2);
    nodes.push_back(newNode);
    }
    else if(opcode == 2) {
    //ADD
    instruction = "ADD R" + Dest + ", " + "R" + Src1 + ", " +  + "R" + Src2;
    Node newNode(2, opcode, dest, src1, src2);
    nodes.push_back(newNode);
    }
    else if(opcode == 3) {
    //SUB
    instruction = "SUB R" + Dest + ", " + "R" + Src1 + ", " +  + "R" + Src2;
    Node newNode(2, opcode, dest, src1, src2);
    nodes.push_back(newNode);
    }
    else if(opcode == 4) {
    //AND
    instruction = "AND R" + Dest + ", " + "R" + Src1 + ", " +  + "R" + Src2;
    Node newNode(2, opcode, dest, src1, src2);
    nodes.push_back(newNode);
    }
    else if(opcode == 5) {
    //OR
    instruction = "OR R" + Dest + ", " + "R" + Src1 + ", " +  + "R" + Src2;
    Node newNode(2, opcode, dest, src1, src2);
    nodes.push_back(newNode);
    }
    else if(opcode == 6) {
    //ADDU
    instruction = "ADDU R" + Dest + ", " + "R" + Src1 + ", " +  + "R" + Src2;
    Node newNode(2, opcode, dest, src1, src2);
    nodes.push_back(newNode);
    }
    else {
    //SUBU
    instruction = "SUBU R" + Dest + ", " + "R" + Src1 + ", " +  + "R" + Src2;
    Node newNode(2, opcode, dest, src1, src2);
    nodes.push_back(newNode);
    }
    return instruction;
}


string cat3(string line) {
	string instruction;
    int opcode = strtoul(line.substr(3,3).c_str(), NULL, 2);
	int dest = strtoul(line.substr(6,5).c_str(), NULL, 2);
	int src1 = strtoul(line.substr(11,5).c_str(), NULL, 2);
	long src2 = signExt16(line.substr(16, 16));
    //int cat = 3;

    //convert int to string
    ss << dest;
    string Dest = ss.str();
    ss.str(string());
    ss << src1;
    string Src1 = ss.str();
    ss.str(string());
    ss << src2;
    string Src2 = ss.str();
    ss.str(string());

    if(opcode == 0 ) {
    //ORI
    instruction = "ORI R" + Dest + ", " + "R" + Src1 + ", " +  + "#" + Src2;
    Node newNode(3, opcode, dest, src1, src2);
    nodes.push_back(newNode);
    }
    else if(opcode == 1) {
    //XORI
    instruction = "XORI R" + Dest + ", " + "R" + Src1 + ", " +  + "#" + Src2;
    Node newNode(3, opcode, dest, src1, src2);
    nodes.push_back(newNode);
    }
    else if(opcode == 2) {
    //ADDI
    instruction = "ADDI R" + Dest + ", " + "R" + Src1 + ", " +  + "#" + Src2;
    Node newNode(3, opcode, dest, src1, src2);
    nodes.push_back(newNode);
    }
    else if(opcode == 3) {
    //SUBI
    instruction = "SUBI R" + Dest + ", " + "R" + Src1 + ", " +  + "#" + Src2;
    Node newNode(3, opcode, dest, src1, src2);
    nodes.push_back(newNode);
    }
    else if(opcode == 4) {
    //ANDI
    instruction = "ANDI R" + Dest + ", " + "R" + Src1 + ", " +  + "#" + Src2;
    Node newNode(3, opcode, dest, src1, src2);
    nodes.push_back(newNode);
    }
    else if(opcode == 5) {
    //SRL
    instruction = "SRL R" + Dest + ", " + "R" + Src1 + ", " +  + "#" + Src2;
    Node newNode(3, opcode, dest, src1, src2);
    nodes.push_back(newNode);
    }
    else if(opcode == 6) {
    //SRA
    instruction = "SRA R" + Dest + ", " + "R" + Src1 + ", " +  + "#" + Src2;
    Node newNode(3, opcode, dest, src1, src2);
    nodes.push_back(newNode);
    }
    else {
    //SLL
    instruction = "SLL R" + Dest + ", " + "R" + Src1 + ", " +  + "#" + Src2;
    Node newNode(3, opcode, dest, src1, src2);
    nodes.push_back(newNode);
    }
    return instruction;
}

//convert 2's complement to decimal
string cat4(string line) {
    int cat = 4;
    long data = strtoul(line.c_str(), NULL, 2);
    if(line.substr(0,1) == "1") {
        data = data - pow(2, 32);		
    }
    ss << data;
    string  instruction = ss.str();
    ss.str(string());
    Node newNode(cat, 0, 0, 0, data);
    nodes.push_back(newNode);
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

	ifstream inFile;
	inFile.open("sample.txt");

	//read input and decode it
	string line;
	int address = 64;
	string instruction;

    //read input file and decode it then store decoded instructions into output file1
	//while(getline(inFile, line)) {
	while(inFile >> line) {
		if(line.substr(0,3) == "001") {
			instruction = cat1(line);
			outFile1 << line << "\t" << address << "\t" << instruction << "\n";
		}
		else if(line.substr(0,3) == "010") {
			instruction = cat2(line);
			outFile1 << line << "\t" << address << "\t" << instruction << "\n";
		}
		else if(line.substr(0,3) == "100"){
			instruction = cat3(line);
			outFile1 << line << "\t" << address << "\t" << instruction << "\n";
		}
		else {
            //data segment starts after break instruction, so this might be the right logic
            instruction = cat4(line);
			outFile1 << line << "\t" << address << "\t" << instruction << "\n";
		}
		address += 4;
	}
	inFile.close();
	outFile1.close();

/*////////////////////////////////////
//manipulate data in memory and registers
    //initialize registers
	for(int i = 0; i < 32; i++) {
        registers[i] = 0;
	}
	//initialize data segment with decoded data after the break line
    write to memeory{}



	ofstream outFile2;
	outFile2.open("simulation.txt");

	//access each element in the vector which stores the decoded instruction
    //vector<instruct>:: iterator it = instructions.begin();
    //while(it != instructions.end() && !(*it).isBreak()) {
    for(int i = 0; !instructions[i].isBreak(); ) {

        //check if it is branch or jump instruction
        if(instructions[i].isJump()) {
            //jump to destination instruction, not accessing memory or registers
            printTrace();
            i = dest + 1;
        }
        else if(instructions[i].isBranch()) {
            if(branchTake()) {
                i = dest + 1;
            }
            printTrace();
        }
        else {
            ////for other instructions just access memory and registers
            printTrace();
            i++;
        }
    }

    //output the break instruction at the end
    ////////
    trace();

	outFile2.close();*/
	return 0;
}
