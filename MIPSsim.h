#include <string>

#ifndef MIPSSIM_H_INCLUDED
#define MIPSSIM_H_INCLUDED

class Node {
private:
    int cat;
    int opcode;
    int dest;
    int src1;
    int src2;
	int PC;
	std::string instruction;
public:
    int getCat(); 
    int getOpcode();
    int getDest();
    int getSrc1();
    int getSrc2();
	int getPC();
	std::string getInstruction(); 
    Node(int category, int op, int destination, int source1, int source2, int pc, std::string instr) {
        cat = category;
        opcode = op;
        dest = destination;
        src1= source1;
        src2 = source2;
		PC = pc;
		instruction = instr;
    }
};

 int Node:: getCat() {
	return cat;
}

 int Node:: getOpcode() {
	return opcode;
}

 int Node:: getDest() {
	return dest;
}

 int Node:: getSrc1() {
	return src1;
}

 int Node:: getSrc2() {
    return src2;
}
 int Node:: getPC(){
	return PC;
}

std::string Node:: getInstruction() {
	return instruction;
}

/*
string cat1(string line);
string cat2(string line);
string cat3(string line);
string cat4(string line);
string loadStore(string line);
string branch(string line);
int signExt16(string str);
string printDisembly(Node node);

string printTrace(Node node);
bool isBranch(Node);
bool branchTake(Node);*/


#endif // MIPSSIM_H_INCLUDED
