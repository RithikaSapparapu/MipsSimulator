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
        string dectobin(int n,int bits){
        // can covert decimal to required bits
        string temp;
        for (int i = bits-1; i >= 0; i--) {
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
  string instructions[16] = {"add","sub","mul","div","sll","slt","srl","jr","lw","sw","la","lui","addi","beq","bne","j"};
    // Struct for R-Type instructions and having function codes
struct {
	const string name;
	string function;
} rMap[9] =
     {
		{ "add", "100000" },
		{ "sub", "100001" },
		{ "mul", "011000" },
		{ "div", "011010" },
		{ "sll", "000000" },
		{ "srl", "000010" },
        { "slt", "101010" },
		{ "jr",  "001000" },
        { NULL, 0 }
     };
// Struct for I-Type instructions and opcode
struct {
	const string name;
	string function;
} 
iMap[8] = {
		{ "lw",   "100011" },
		{ "sw",   "101011" },
        { "la",   "101011" },
        { "lui",  "001111" },
        { "addi", "001000" },
		{ "beq",  "000100" },
		{ "bne", "001010" },
		{ NULL, 0 } 
};
// Struct for J-Type instructions and opcode
struct {
	const string name;
	string function;
} jMap[2] = {
		{ "j", "000010" },
		{ NULL, 0 } };
//funcn to search the instruction
 string search(string current_instruction) {
     string temp="";
     int n=instructions->length();
	for (int i = 0; i <n; i++) {
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
//returns the type of instruction
string typeOfInstruction(string current_instruction){
    string str="invalid instruction";
    int store;
    string type;
    size_t found;
    for(int i=0;i<instructions->length();i++){
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
    string null="NULL";
	for (i = 0; rMap[i].name != null; i++) {                        /*check*/
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
    string null="NULL";
	for (i = 0; iMap[i].name != null; i++) {
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
    string null="NULL";
	for (i = 0; jMap[i].name != null; i++) {
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
        registers(){
        int register_values[32]={0};
        string reg_address[33];
        for(int i=0;i<32;i++){
            reg_address[i]=gen.dectobin(i,5);
        }
        reg_address[32]="0";
     }
        string reg_address[33];
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
class mipsSimulator{
public:
    instructionSet obj;
    generalFunctions gen;
    registers reg;
    memory mem_obj;
       int programCounter;
       int NumberOfInstructions;
       int MaxLength;//10000
       vector<string> InputProgram; //to store the input program
       struct Memoryword{
           string value;
           string address;//pc line number
        };
        struct Label{
           string labelname;
           string address;
        };
        vector<struct Memoryword>Mem;
        vector<struct Label>labeltable;
    mipsSimulator(string fileName){
       programCounter=0;
       NumberOfInstructions=0;
       MaxLength=10000;
    }
    void storefile(string fileName){
    ifstream InputFile;
	InputFile.open(fileName.c_str(),ios::in); //open file
	if(!InputFile) //if open failed
	{
		cout<<"Error: File does not exist or could not be opened"<<endl;
		exit(1);
	}
	string tempString;
	while(getline(InputFile,tempString)) //read line by line
	{
       //readInstruction(tempString);
		NumberOfInstructions++;
		if(NumberOfInstructions>MaxLength) ///check number of instructions with maximum allowed
		{
			cout<<"Error: Number of lines in input too large, maximum allowed is "<<MaxLength<<" line"<<endl;
			exit(1);
		}
		InputProgram.push_back(tempString); //store in InputProgram
	}
	InputFile.close();
    }
    //str is current_instruction
    void removeSpacesandCommas(string str){
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
 void removeDollar(string str){
        int count = 0;
    for (int i = 0; str[i]; i++){
        if (str[i] != '$')
            str[count++] = str[i];
      str[count] = '\0';
    }
 }
    void readInstruction(string current_instruction){
            if(current_instruction.find("#")!=-1) //remove comments
	            {
                current_instruction=current_instruction.substr(0,current_instruction.find("#"));
	            }
        removeSpacesandCommas(current_instruction);
        removeDollar(current_instruction);

    }
    void reportError(int line_number){
       cout<<"Error found in :"<<(line_number+1)<<": "<<InputProgram[line_number]<<endl;
    }
    //parses r,i,j type
    string parseinstruction(string current_instruction){
        int key;
        readInstruction(current_instruction);
        string instructn=obj.search(current_instruction);
        string type=obj.typeOfInstruction(instructn);
        if(type=="r")
        key=1;
        else if(type=="i")
        key=2;
        else
        key=3;

        string final="";
        switch(key){
        case 1:
            //convert R-type instruction into binary string
            final=rtype_instruction(current_instruction);
            break;
        case 2:
            //convert I-type instruction into binary string
            final=itype_instruction(current_instruction);
            break;
        case 3:
            //convert J-type instruction into binary string
             final=jtype_instruction(current_instruction);
            break;
        default:
            return NULL;
            break;
        }
    }
    //program read->line by line->datasegement->.word->struct->store in struct
    void preprocess(){
    int i=0,j=0;
	int current_section=-1; //current_section=0 - data section, current_section=1 - text section
	int index; //to hold index of ".data"
	int flag=0; //whether "..data" found
	//string current_instruction="";
	int dataStart=0; //line number for start of data section
	int textStart=0;
    for(i=0;i<NumberOfInstructions;i++){
        string current_instruction="";
        current_instruction=InputProgram[i];
        readInstruction(current_instruction);
        index=current_instruction.find(".data");
        if(index==-1)
        continue;
        else if(flag==0){
            flag=1;
            current_section=0;
            dataStart=i;
        }
        else if(flag==1){
            cout<<"Multiple instances of .data found"<<endl;
            exit(1);
        }
    }
    int wordindex;
    if(current_section==0){
        for(i=dataStart+1;i<NumberOfInstructions;i++){
            string current_instruction="";
            current_instruction=InputProgram[i];
            readInstruction(current_instruction);
            wordindex=current_instruction.find(".word");
            int storeline;
            if(wordindex==-1){
                if(current_instruction.find(".text")==-1) //if text section has not started
				{
					cout<<"Error: Unexpected symbol in data section"<<endl;
				}
				else
				{
					break;
				}
            }
           else{
               string num=current_instruction.substr(wordindex+5);
               Memoryword tempmemory;
               tempmemory.value=num;
               tempmemory.address=to_string(i+1);;
               Mem.push_back(tempmemory);
           } 
        }
    }
    for(i=0;Mem.size()>0 && i<Mem.size()-1;i++) //check for duplicates
	{
		if(Mem[i].value==Mem[i+1].value)
		{
			cout<<"Error: One or more labels are repeated"<<endl;
			exit(1);
		}
	}
    int textIndex=0;
    int textFlag=0;

    for(i=programCounter;i<NumberOfInstructions;i++)
	{
        string current_instruction=InputProgram[i];
		readInstruction(current_instruction);
		if(current_instruction=="")
		{
			continue;
		}
		textIndex=current_instruction.find(".text"); //find text section similar as above
		if(textIndex==-1)
		{
			continue;
		}
		else if(textFlag==0)
		{
			textFlag=1;
			current_section=1;
			textStart=i;
		}
		else if(textFlag==1)
		{
			cout<<"Error: Multiple instances of .text"<<endl;
			reportError(i);
		}
	}
    if(current_section!=1) //if text section not found
	{
		cout<<"Error: Text section does not exist or found unknown string"<<endl;
		exit(1);
	}
    if(InputProgram[textStart+1]!=".globl main"){
         cout<<"Error: No (.globl main) found"<<endl;
		exit(1);
    }
    int foundmain=0;
    int main_index=0,labelindex=-1;
    if(InputProgram[textStart+2]!="main:"){
         cout<<"Error: No main found"<<endl;
		exit(1);
    }
    else{
        foundmain=1;
        main_index=textStart+2;
    }
    for(int i=main_index+1;i<NumberOfInstructions;i++){
        string current_instruction=InputProgram[i];
		readInstruction(current_instruction);
        labelindex=current_instruction.find(":");
        if(labelindex==0){
            cout<<"Error : Label name expected"<<endl;
            reportError(i);
        }
        else if(labelindex==-1){
            continue;
        }
        else{
            j=labelindex-1;
            string temp="";
            temp=current_instruction.substr(j);
            Label templabel;
            templabel.labelname=temp;
            templabel.address=to_string(i+1);
            labeltable.push_back(templabel);

        }
    }
    for(i=0;labeltable.size()>0 && i<(labeltable.size()-1);i++) //check for duplicates
	{
		if(labeltable[i].labelname==labeltable[i+1].labelname)
		{
			cout<<"Error: One or more labels are repeated"<<endl;
			exit(1);
		}
	}
}
    string rtype_instruction(string current_instruction){
        readInstruction(current_instruction);
        string store=obj.search(current_instruction);
         string temp="";
        string opcode="000000";
        string shamt,functionCode;
        if(store=="jr"){
            current_instruction=current_instruction.substr(2);
            temp="00000011111000000000000000001000";
        }
        else{
            current_instruction=current_instruction.substr(3);
        temp=temp+opcode;
        if(store.compare("add")==0 || store.compare("sub")==0 ||store.compare("mul")==0 ||store.compare("div")==0||store.compare("slt")==0){
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
        }
        else if(store.compare("sll")==0 || store.compare("srl")==0){
            temp=temp+"00000";
            string reg_store[2];
            reg_store[0]=current_instruction.substr(0,2);
            reg_store[1]=current_instruction.substr(2,2);
            for(int i=0;i<2;i++){
                string addr=reg.findRegisterAddress(reg_store[i]);
                temp=temp+addr;
             }
             shamt=gen.dectobin(stoi(current_instruction.substr(4)),5);
             temp=temp+shamt;
           functionCode=obj.r_fncode(store);
           temp=temp+functionCode;
        }
      }
      return temp;
   }
    //Parsing itype instructions
    string itype_instruction(string current_instruction){
        readInstruction(current_instruction);
        string store=obj.search(current_instruction);
         string temp="";
         string opcode,immediate,reg_store[2];
         if(store=="lw"||store=="sw"){
             opcode=obj.i_opcode(store);
             temp=temp+opcode;
             current_instruction=current_instruction.substr(2);
             reg_store[1]=current_instruction.substr(0,2);
             int search=current_instruction.find("(");
             int temp_imm=stoi(current_instruction.substr(2,search-2));
             immediate=gen.dectobin(temp_imm,16);
             reg_store[0]=current_instruction.substr(search+1,2);
             for(int i=0;i<2;i++){
                string addr=reg.findRegisterAddress(reg_store[i]);
                temp=temp+addr;
             }
             temp=temp+immediate;
         }
         else if(store=="beq"||store=="bne"){
             opcode=obj.i_opcode(store);
             temp=temp+opcode;
             current_instruction=current_instruction.substr(3);
             if(current_instruction.substr(0,2)!="ze"){
             reg_store[0]=current_instruction.substr(0,2);
             if(current_instruction.substr(2,2)!="ze")
             reg_store[1]=current_instruction.substr(2,2);
             else
             reg_store[1]=current_instruction.substr(2,4);
             }
             else{
             reg_store[0]=current_instruction.substr(0,4);
             if(current_instruction.substr(4,2)!="ze")
             reg_store[1]=current_instruction.substr(4,2);
             }
            for(int i=0;i<2;i++){
                string addr=reg.findRegisterAddress(reg_store[i]);
                temp=temp+addr;
             }
             int foundlabel=0;
             for(int i=0;i<labeltable.size();i++){
            if(current_instruction.find(labeltable[i].labelname)!=-1){
                int val=stoi(labeltable[i].address);
                string temp_val=gen.dectobin(val,16);
                temp=temp+temp_val;
                foundlabel=1;
                break;
            }
        }
        if(foundlabel==0){
            cout<<"Invalid label name"<<endl;
            exit(1);
        }
        else{
            return temp;
        }
    }
         else if(store=="addi"){
            opcode=obj.i_opcode(store);
            temp=temp+opcode;
            current_instruction=current_instruction.substr(4);
            reg_store[0]=current_instruction.substr(0,2);
            current_instruction=current_instruction.substr(0,2);
            if(current_instruction.find("zero")!=-1){
            reg_store[1]=current_instruction.substr(0,2);
            current_instruction=current_instruction.substr(0,2);

            }
            else{
                 reg_store[1]=current_instruction.substr(0,4);
                 current_instruction=current_instruction.substr(0,4);
            }
            int temp_imm=stoi(current_instruction);
            immediate=gen.dectobin(temp_imm,16);
             for(int i=0;i<2;i++){
                string addr=reg.findRegisterAddress(reg_store[i]);
                temp=temp+addr;
             }
             temp=temp+immediate;
         }

    }
    //Parsing jtype instructions
    string jtype_instruction(string current_instruction){
        readInstruction(current_instruction);
        string opcode=obj.j_opcode("j");
        string temp="";
        temp=temp+opcode;
        current_instruction=current_instruction.substr(1);
        for(int i=0;i<labeltable.size();i++){
            if(labeltable[i].labelname==current_instruction){
                int val=stoi(labeltable[i].address);
                string temp_val=gen.dectobin(val,26);
                temp=temp+temp_val;
                //temp=temp+labeltable[i].address;
                break;
            }
        }
        return temp;

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