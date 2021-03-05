#define _GLIBCXX_USE_CXX11_ABI 0
#include <bits/stdc++.h>

using namespace std;

class generalFunctions{
    public:

        void terminate(){
            cout << "oops! An Error has occured!" << endl;
            exit(0);
        }

        string DecimalToBinary(int decimalVal){
            string r;
            while(decimalVal!=0) {r=(decimalVal%2==0 ?"0":"1")+r; decimalVal/=2;}
            return r;
        }

        int BinaryToDecimal(string BinaryVal){
            int i = std::stoi(BinaryVal,0, 2);
            return i;
        }
        string dectobin(int n){
        // Size of an integer is assumed to be 5 bits
        string temp;
        for (int i = 4; i >= 0; i--) {
            int k = n >> i;
            if (k & 1)
            temp=temp+"1";
            else
                temp=temp+"0";
            }
            return temp;
        }
};


class memory{
    public:
        generalFunctions gen;
        string MEM[1024]; //ex: string colour[4] = { "Blue", "Red", "Orange", "Yellow" };
        string getvalue;


        //functions dealing with memory
        void getdata(string adrs){
            getvalue = MEM[gen.BinaryToDecimal(adrs)];
        }


        void setdata(string adrs,int data){
            if (gen.DecimalToBinary(data).length() > 32){
                    gen.terminate();
                }
                MEM[gen.BinaryToDecimal(adrs)] = gen.DecimalToBinary(data);
        }
};


class instructionSet{
public:
    // Array that holds the supported instructions
  char instructions[13] = {"add","sub","mul","div","sll","srl","jr","lw","sw","beq","bne","j","jal"};
    // Struct for R-Type instructions and having function codes
struct {
	const string name;
	string function;
} rMap[] = {
		{ "add", "100000" },
		{ "sub", "100001" },
		{ "mul", "011000" },
		{ "div", "011010" },
		{ "sll", "000000" },
		{ "srl", "000010" },
		{ "jr",  "001000" },
		{ NULL, 0 } };
// Struct for I-Type instructions and opcode
struct {
	const string name;
	string function;
} 
iMap[] = {
		{ "lw",   "100011" },
		{ "sw",   "101011" },
		{ "beq",  "000100" },
		{ "bne", "001010" },
		{ NULL, 0 } 
};
// Struct for J-Type instructions and opcode
struct {
	const string name;
	string function;
} jMap[] = {
		{ "j", "000010" },
		{ "jal", "000011" },
		{ NULL, 0 } };
//funcn to search the instruction
 string search(string current_instruction) {
     string temp="";
	for (int i = 0; i < instructions.length(); i++) {
       int pos=0;
      while(pos<3)
		if (current_instruction.find(instructions[i],pos) == 0) {
			temp=instructions[i];
			break;
		}
		pos++;
	}
	if(temp!=""){
        return temp;
	}
	else{
        return NULL;
	}
}
char typeOfInstruction(string current_instruction){
    string str="invalid instruction";
    int store;
    string type;
    size_t found;
    for(int i=0;i<instructions.length();i++){
       found=current_instruction.find(instructions[i]);
       store=i;
    }
    if (found != string::npos){
        if(store>=0 && store<=7)
            type="r";
            else if(store>7 && store<12)
                type="i";
            else
                type="j";
     return type;
    }
    else{
        return str;
    }
}
//finds the functioncode for Rtype
string r_fncode(string instruction){
    size_t i;
    string func;
	for (i = 0; rMap[i].name != NULL; i++) {                        /*check*/
		if (instruction.compare(rMap[i].name) == 0) {
			func = rMap[i].function;
		}
	}
	return func;
}
//finds the opcode forItype
string i_opcode(string instruction){
    size_t i;
    string func;
	for (i = 0; iMap[i].name != NULL; i++) {
		if (instruction.compare(iMap[i].name) == 0) {
			func = iMap[i].function;
		}
	}
	return func;
}
//finds the opcode for JTYPE
string j_opcode(string instruction){
    size_t i;
    string func;
	for (i = 0; jMap[i].name != NULL; i++) {
		if (instruction.compare(jMap[i].name) == 0) {
			func = jMap[i].function;
		}
	}
	return func;
}
};

class registers{
    public:
        //arrays for storing register values,addresses and register names
        string REG[33]={"zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra","NULL"};
        int register_values[32]={0};
        string reg_address[32]={"00000","00001","00010","00011","00100","00101","00110","00111","01000","01001","01010","01011","01100","01101","01110","01111","10000","10001","10010","10011","10100","10101","10110","10111","11000","11001","0"};
        generalFunctions gen;
        instructionSet inst;
        // setvalue of a register funct
        //void setdata(destAddrs,data_to_be_stored);
        // getvalue of a register funct
        string getdata(string value){
            string result;
            if(value == "00000"){
                result = "0";   //R0 will have zero stored inside it.
            }
            else{
                result = REG[gen.BinaryToDecimal(value)];
            }
            return result;
        }
        //finds the address of given register
        string findRegisterAddress(string str){
            string temp;
            int pos;
            for(int i=0;i<32;i++){
                if(str.find(REG[i])!=string::npos){
                   pos=i;
                   temp=reg_address[pos];
                   break;
                }
            }
            return temp;
        }
        //other functions required for registers
};
class computingUnit{ //we compute only for rtypes - add, sub, etc
    public:
        generalFunctions gen;
        int finalResult;
        void compute(string source1, string source2, string funct){ //funct is function code that identifies the specific R type instruction
            if (funct == "100000"){
                finalResult = gen.BinaryToDecimal(source1) + gen.BinaryToDecimal(source2);
            }
            else if(funct == "100010"){
                finalResult = gen.BinaryToDecimal(source1) - gen.BinaryToDecimal(source2);
            }
            //elseif any other r type instructions which need to be computed
        }
};
class parseInstructions{
public:
    instructionSet obj;
    generalFunctions gen;
    registers reg;
     int typeOfInstrucn;
    //str is current_instruction
    void removeSpacesandCommas(char *str){
        int count = 0;
    for (int i = 0; str[i]; i++){
        if (str[i] != ' ')
            str[count++] = str[i];
      str[count] = '\0';
    }
    int count1= 0;
    for (int j = 0; str[j]; j++){
        if (str[j] != ',')
            str[count1++] = str[j];
      str[count1] = '\0';
    }
 }
 void removeDollar(char *str){
        int count = 0;
    for (int i = 0; str[i]; i++){
        if (str[i] != '$')
            str[count++] = str[i];
      str[count] = '\0';
    }
 }
    void readInstruction(string current_instruction){
        if(current_instruction==""||current_instruction=="#")
            cout<<"Invalid Instruction"<<endl;
            if(current_instruction.find("#")!=-1) //remove comments
	          {
                current_instruction=current_instruction.substr(0,current_instruction.find("#"));
	           }
        removeSpacesandCommas(current_instruction);
        removeDollar(current_instruction);
    }
    string parseinstruction(string current_instruction){
        int key;
        readInstruction(current_instruction);
        string instructn=obj.search(current_instruction);
        char type=obj.typeOfInstruction(instructn);
        string final="";
        switch(type){
        case 'r':
            //convert R-type instruction into binary string
            final=rtype_instruction(current_instruction);
            break;
        case 'i':
            //convert I-type instruction into binary string
            final=itype_instruction(current_instruction);
            break;
        case 'j':
            //convert J-type instruction into binary string
             final=jtype_instruction(current_instruction);
            break;
        default:
            return NULL;
            break;
        }
    }
    string rtype_instruction(string current_instruction){
        readinstruction(current_instruction);
        string store=obj.search(current_instruction);
        if(store!="jr"){
            current_instruction=current_instruction.substr(3);
        }
        else
            current_instruction=current_instruction.substr(2);
        string temp="";
        char opcode="000000";
        char shamt,functionCode;
        temp=temp+opcode;
        if(store.compare("add")==0 || store.compare("sub")==0 ||store.compare("mul")==0 ||store.compare("div")==0){
             string reg_store[3];//stores src1,src2,dest
             shamt="00000";
             if(current_instruction.find("at")!=-1){
             if(current_instruction.substr(0,4)=="zero"){
             reg_store[0]=current_instruction.substr(0,4);
                if(current_instruction.substr(4,4)=="zero"){
                   reg_store[1]=current_instruction.substr(4,4);
                   reg_store[2]=current_instruction.substr(8);
                   }
                else{
                    reg_store[1]=current_instruction.substr(3,2);
                    reg_store[2]=current_instruction.substr(6);
                }
             }
             else {
             reg_store[0]=current_instruction.substr(0,2);
             if(current_instruction.substr(2,4)=="zero"){
                   reg_store[1]=current_instruction.substr(4,4);
                   reg_store[2]=current_instruction.substr(8);
                   }
                else{
                    reg_store[1]=current_instruction.substr(3,2);
                    reg_store[2]=current_instruction.substr(6);
                 }
             }
             else{
                return NULL;
             }
             for(int i=0;i<3;i++){
                string addr=reg.findRegisterAddress(reg_store[i]);
                temp=temp+addr;
            }
           temp=temp+shamt;
           functionCode=obj.r_fncode(store);
           temp=temp+functionCode;
           return temp;
        }
        else if(store.compare("sll")==0 || store.compare("srl")==0){
            temp=temp+"00000";
            string reg_store[2];
            reg_store[0]=current_instruction(0,2);
            reg_store[1]=current_instruction(2,2);
            for(int i=0;i<2;i++){
                string addr=reg.findRegisterAddress(reg_store[i]);
                temp=temp+addr;
             }
             shamt=gen.dectobin(stoi(current_instruction(4)));
             temp=temp+shamt;
           functionCode=obj.r_fncode(store);
           temp=temp+functionCode;
           return temp;
        }
        //Have to write for jr
        else{
        }
    }
    string itype_instruction(string current_instruction){
    }
};


class control{  // we might use flag kinda things to know what instruction has appeared. for example, if 000000 is the opcode we will update flag1 (suppose) to 1, and we will use it in simulator class
    public:
        generalFunctions gen;
        int checker = 0;
        void checkInstruction(string opcode, string funct){
            if (opcode == "000000"){
                if (funct == "100000" || funct == "100010"){
                    //we know now that instruction is add or subtract
                    //update value of add_sub to 1 maybe?
                    checker = 1;
                }
            }
            else if (opcode == "000100"){
               //beq
               //here we could do checker = 2
               checker = 2;
            }
            else if (opcode == "000010"){
                //j
                // here checker = 3 and so on...
            }
            else if (opcode == "101011"){
                //sw
            }
            else if (opcode == "100011"){
                //lw
                checker = 3;
            }
            else {
                gen.terminate;
            }
        }
        void printRegisters(registers rgstr){
            // prints in terminal
            cout << "R1: " << rgstr.REG[1] << endl;
            cout << "R2: " << rgstr.REG[2] << endl;
            cout << "R3: " << rgstr.REG[3] << endl;
            cout << "R4: " << rgstr.REG[4] << endl;
            cout << "R5: " << rgstr.REG[5] << endl;
            cout << "R6: " << rgstr.REG[6] << endl;
            cout << "R7: " << rgstr.REG[7] << endl;
            cout << "R8: " << rgstr.REG[8] << endl;
            cout << "R9: " << rgstr.REG[9] << endl;
            cout << "R10: " << rgstr.REG[10] << endl;
            cout << "R11: " << rgstr.REG[11] << endl;
            cout << "R12: " << rgstr.REG[12] << endl;
            cout << "R13: " << rgstr.REG[13] << endl;
            cout << "R14: " << rgstr.REG[14] << endl;
            cout << "R15: " << rgstr.REG[15] << endl;
            cout << "R16: " << rgstr.REG[16] << endl;
            cout << "R17: " << rgstr.REG[17] << endl;
            cout << "R18: " << rgstr.REG[18] << endl;
            cout << "R19: " << rgstr.REG[19] << endl;
            cout << "R20: " << rgstr.REG[20] << endl;
            cout << "R21: " << rgstr.REG[21] << endl;
            //other registers
        }
};
class simulator{
    public:
        void execute(){
            string  instruction = "00000010001100101000000000100000"; //one add instruction is hardcoded as an example just to understand how the everything works
            string opcode, functionCode, src1, src2;
            computingUnit computingU;
            control contr;                          //creating objects for all classes
            generalFunctions gen;
            registers reg;
            memory memo;
            opcode = instruction.substr(0,6);
            functionCode = instruction.substr(26,6);
            contr.checkInstruction(opcode,functionCode);
            if(contr.checker==1){ //this if block is for add, sub and all such similar instructions (may include more if required)
                src1 = reg.getdata(instruction.substr(6,5));
                src2 = reg.getdata(instruction.substr(11,5));
                string destAddrs = instruction.substr(16,5); //stores the adress of the destination register in binary form
                computingU.compute(src1,src2,functionCode);
                reg.setdata(destAddrs,computingU.finalResult);  //this function not yet defined (has to be defined in class registers)
            }
            //elseif other operations like bne, j etc
            else if(contr.checker==2){
                src1 = reg.getdata(instruction.substr(6,5));
                src2 = reg.getdata(instruction.substr(11,5));
                if(gen.BinaryToDecimal(src1) - gen.BinaryToDecimal(src2)==0){
                    //increment pc to the value which the label indicates
                }
                
            }
            //finally we print all registers here
            contr.printRegisters(reg);
        }
};
/*
00100000000100010000000000000001
00100000000100100000000000001010
*/
int main(){ 
    simulator simulate;
    simulate.execute();
}
//end of program