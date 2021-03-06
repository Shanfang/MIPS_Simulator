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
vector<int> memory;
vector<Node> nodes;
bool breakLine = false;
int memStart;
stringstream ss;
ofstream outFile2;

//calculate offset
int signExt16(string str) {
	long offset = strtoul(str.c_str(), NULL, 2);
    if(str.substr(0, 1) == "1") {
        offset = offset - pow(2, 16);
    }
    return offset;
}

int signExt5(string str) {
	long offset = strtoul(str.c_str(), NULL, 2);
    if(str.substr(0, 1) == "1") {
        offset = offset - pow(2, 5);
    }
    return offset;
}

//decode cat1 instructions
string cat1(string line, int PC) {
	string instruction;
    int opcode = strtoul(line.substr(3,3).c_str(), NULL, 2);
    //int cat = 1;
	if(opcode == 0) {
        //NOP
		instruction = "NOP";
		int opcode = 0;
        Node newNode(1, opcode, 0, 0, 0,PC, instruction);
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

        Node newNode(1, opcode, newPC, 0, 0, PC, instruction);
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
        Node newNode(1, opcode, rs, rt, offset, PC, instruction);
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
        Node newNode(1, opcode, rs, rt, offset, PC, instruction);
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
        Node newNode(1, opcode, rs, 0, offset, PC, instruction);
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
        Node newNode(1, opcode, base, rt, offset, PC, instruction);
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
        Node newNode(1, opcode, base, rt, offset, PC, instruction);
        nodes.push_back(newNode);
	}
	else {
		//BREAK
		instruction = "BREAK";
        Node newNode(1, opcode, 0, 0, 0, PC, instruction);
        nodes.push_back(newNode);
		breakLine = true;
	}
	return instruction;
}


string cat2(string line, int PC) {
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
    Node newNode(2, opcode, dest, src1, src2, PC, instruction);
    nodes.push_back(newNode);
    }
    else if(opcode == 1) {
    //MUL
    instruction = "MUL R" + Dest + ", " + "R" + Src1 + ", " +  + "R" + Src2;
    Node newNode(2, opcode, dest, src1, src2, PC, instruction);
    nodes.push_back(newNode);
    }
    else if(opcode == 2) {
    //ADD
    instruction = "ADD R" + Dest + ", " + "R" + Src1 + ", " +  + "R" + Src2;
    Node newNode(2, opcode, dest, src1, src2, PC, instruction);
    nodes.push_back(newNode);
    }
    else if(opcode == 3) {
    //SUB
    instruction = "SUB R" + Dest + ", " + "R" + Src1 + ", " +  + "R" + Src2;
    Node newNode(2, opcode, dest, src1, src2, PC, instruction);
    nodes.push_back(newNode);
    }
    else if(opcode == 4) {
    //AND
    instruction = "AND R" + Dest + ", " + "R" + Src1 + ", " +  + "R" + Src2;
    Node newNode(2, opcode, dest, src1, src2, PC, instruction);
    nodes.push_back(newNode);
    }
    else if(opcode == 5) {
    //OR
    instruction = "OR R" + Dest + ", " + "R" + Src1 + ", " +  + "R" + Src2;
    Node newNode(2, opcode, dest, src1, src2, PC, instruction);
    nodes.push_back(newNode);
    }
    else if(opcode == 6) {
    //ADDU
    instruction = "ADDU R" + Dest + ", " + "R" + Src1 + ", " +  + "R" + Src2;
    Node newNode(2, opcode, dest, src1, src2, PC, instruction);
    nodes.push_back(newNode);
    }
    else {
    //SUBU
    instruction = "SUBU R" + Dest + ", " + "R" + Src1 + ", " +  + "R" + Src2;
    Node newNode(2, opcode, dest, src1, src2, PC, instruction);
    nodes.push_back(newNode);
    }
    return instruction;
}


string cat3(string line, int PC) {
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
    Node newNode(3, opcode, dest, src1, src2, PC, instruction);
    nodes.push_back(newNode);
    }
    else if(opcode == 1) {
    //XORI
    instruction = "XORI R" + Dest + ", " + "R" + Src1 + ", " +  + "#" + Src2;
    Node newNode(3, opcode, dest, src1, src2, PC, instruction);
    nodes.push_back(newNode);
    }
    else if(opcode == 2) {
    //ADDI
    instruction = "ADDI R" + Dest + ", " + "R" + Src1 + ", " +  + "#" + Src2;
    Node newNode(3, opcode, dest, src1, src2, PC, instruction);
    nodes.push_back(newNode);
    }
    else if(opcode == 3) {
    //SUBI
    instruction = "SUBI R" + Dest + ", " + "R" + Src1 + ", " +  + "#" + Src2;
    Node newNode(3, opcode, dest, src1, src2, PC, instruction);
    nodes.push_back(newNode);
    }
    else if(opcode == 4) {
    //ANDI
    instruction = "ANDI R" + Dest + ", " + "R" + Src1 + ", " +  + "#" + Src2;
    Node newNode(3, opcode, dest, src1, src2, PC, instruction);
    nodes.push_back(newNode);
    }
    else if(opcode == 5) {
    //SRL
	src2 = signExt5(line.substr(27, 5));
	ss << src2;
    Src2 = ss.str();
    ss.str(string());
    instruction = "SRL R" + Dest + ", " + "R" + Src1 + ", " +  + "#" + Src2;
    Node newNode(3, opcode, dest, src1, src2, PC, instruction);
    nodes.push_back(newNode);
    }
    else if(opcode == 6) {
    //SRA
	src2 = signExt5(line.substr(27, 5));
	ss << src2;
    Src2 = ss.str();
    ss.str(string());
    instruction = "SRA R" + Dest + ", " + "R" + Src1 + ", " +  + "#" + Src2;
    Node newNode(3, opcode, dest, src1, src2, PC, instruction);
    nodes.push_back(newNode);
    }
    else {
    //SLL
	src2 = signExt5(line.substr(27, 5));
	ss << src2;
    Src2 = ss.str();
    ss.str(string());
    instruction = "SLL R" + Dest + ", " + "R" + Src1 + ", " +  + "#" + Src2;
    Node newNode(3, opcode, dest, src1, src2, PC, instruction);
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
    //Node newNode(cat, 0, 0, 0, data);
    //nodes.push_back(newNode);
    return instruction;
}

//print out data in registers and memory
void printTrace(Node* node, int cycle) {
	outFile2 << "--------------------\n";
	outFile2 << "Cycle " << cycle + 1 << ":\t" << node->getPC() << "\t" << node->getInstruction() << endl;
	
	//print out data in registers
	outFile2 << "\nRegisters\n";
	for(int i  = 0; i < 4; i++) {
		if(i <= 1) {
			outFile2 << "R0" << 8*i << ":\t";		
		}		
		else {
			outFile2 << "R" << 8*i << ":\t";	
		}
		for(int j = 0; j < 7; j++) {
			outFile2 << registers[8*i + j] << "\t";
		}
		outFile2 << registers[8*i + 7] << endl;
	}
	
	//print out data in memory
	outFile2 << "\nData\n";
	//cout << memStart << ":\t";
	for(int i = 0; i < memory.size()/8; i++) {
		outFile2 << memStart + i * 32 << ":\t";
		for(int j = 0; j < 8; j++) {
			outFile2 << memory[8*i + j] << "\t";
		}
		outFile2 << endl;
	}
}

//int main() {
int main(int argc, char* argv[]) {
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
	}

	//open output file for storing disassembly
	ofstream outFile1;
	outFile1.open("disassembly.txt");

	//ifstream inFile;
	//inFile.open("sample.txt");

	//read input and decode it
	string line;
	int address = 64;
	string instruction;

    //read input file and decode it then store decoded instructions into output file1
	//while(getline(inFile, line)) {
	while(inFile >> line) {
		if(line.substr(0,3) == "001") {
			instruction = cat1(line, address);
			outFile1 << line << "\t" << address << "\t" << instruction << "\n";
			if(breakLine) {
				memStart = address + 4;
			}
		}
		else if(line.substr(0,3) == "010") {
			instruction = cat2(line, address);
			outFile1 << line << "\t" << address << "\t" << instruction << "\n";
		}
		else if(line.substr(0,3) == "100"){
			instruction = cat3(line, address);
			outFile1 << line << "\t" << address << "\t" << instruction << "\n";
		}
		else {
            //data segment starts after break instruction, so this might be the right logic
            instruction = cat4(line);
			outFile1 << line << "\t" << address << "\t" << instruction << "\n";			
				
			//store data decoded after the break line into corresponding memeory address
			memory.push_back(strtoul(instruction.c_str(), NULL, 10));
		}
		address += 4;
	}
	inFile.close();
	outFile1.close();

    //initialize registers
	for(int i = 0; i < 32; i++) {
        registers[i] = 0;
	}

	//ofstream outFile2;
	outFile2.open("simulation.txt");

	//access each element in the vector which stores the decoded instruction
    for(int i = 0; i < nodes.size(); ) {

        //check if it is branch or jump instruction
        if(nodes[i].getCat() == 1 && nodes[i].getOpcode() == 1) {
            //jump to destination instruction, not accessing memory or registers
            printTrace(&nodes[i], i);			
			int j = (nodes[i].getDest() * 4 - nodes[i].getPC())/4;
            i = i + j;
        }
        else if(nodes[i].getCat() == 1 && nodes[i].getOpcode() == 2) {
			//branch on equal
            printTrace(&nodes[i], i);			
            if(registers[nodes[i].getSrc1()] == registers[nodes[i].getSrc2()]) {
                i += registers[nodes[i].getDest()];
            }
			i++;
        }
        else if(nodes[i].getCat() == 1 && nodes[i].getOpcode() == 3) {
			//branch not equal
            printTrace(&nodes[i], i);			
            if(registers[nodes[i].getSrc1()] != registers[nodes[i].getSrc2()]) {
                i += registers[nodes[i].getDest()];
            }
			i++;
        }
        else if(nodes[i].getCat() == 1 && nodes[i].getOpcode() == 4) {
			//branch greater than 0
            printTrace(&nodes[i], i);			
            if(registers[nodes[i].getSrc1()] > registers[nodes[i].getSrc2()]) {
                i += registers[nodes[i].getDest()];
            }
			i++;
        }
        else if(nodes[i].getCat() == 1 && nodes[i].getOpcode() == 5) {
			//store word
			int mem = registers[nodes[i].getDest()] + nodes[i].getSrc2();
			memory[(mem-memStart)/4] = registers[nodes[i].getSrc1()];
            printTrace(&nodes[i], i);			
			i++;
        }
        else if(nodes[i].getCat() == 1 && nodes[i].getOpcode() == 6) {
			//load word
			int mem = registers[nodes[i].getDest()] + nodes[i].getSrc2();
			registers[nodes[i].getSrc1()] = memory[(mem-64)/4];
            printTrace(&nodes[i], i);			
			i++;
        }
		//else if(nodes[i].cat == 2 && nodes[i].opcode == 0) {
			//XOR
			
			
		//}
		
        else {
            ////for other instructions just access memory and registers
            printTrace(&nodes[i], i);			
            i++;
        }
    }

    //output the break instruction at the end
    ////////

	outFile2.close();
	return 0;
}
