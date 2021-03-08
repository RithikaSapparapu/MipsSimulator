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

class mipsSimulator{
public:
    generalFunctions gen;
    int MEM[1024];
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
         int register_values[32]={0};
         string REG[32]={"zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"};
        string instructions[16] = {"add","sub","mul","div","sll","slt","srl","jr","lw","sw","la","lui","addi","beq","bne","j"};
    mipsSimulator(string fileName){
       programCounter=0;
       NumberOfInstructions=0;
       MaxLength=10000;


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
        string readInstruction(string current_instruction){
                if(current_instruction.find("#")!=-1) //remove comments
                    {
                    current_instruction=current_instruction.substr(0,current_instruction.find("#"));
                    }
            removeSpacesandCommas(current_instruction);
            removeDollar(current_instruction);

            return current_instruction;
        }

        void reportError(int line_number){
            cout<<"Error found in :"<<(line_number+1)<<": "<<InputProgram[line_number]<<endl;
        }






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
                current_instruction = readInstruction(current_instruction);
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
                    current_instruction = readInstruction(current_instruction);
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
                current_instruction = readInstruction(current_instruction);
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
                current_instruction = readInstruction(current_instruction);
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


        void processInstruction(string current_instruction){
            if(current_instruction.substr(0,3)=="add"){
                int reg_store[3]={-1};
                for(int i=0;i<33;i++){
                    if(current_instruction.substr(3,2)==REG[i])
                        reg_store[0]=i;
                    if(current_instruction.substr(5,2)==REG[i])
                        reg_store[1]=i;
                    if(current_instruction.substr(7,2)==REG[i])
                        reg_store[2]=i;

                }
        
                    register_values[reg_store[0]]= register_values[reg_store[1]]+ register_values[reg_store[2]];
                    programCounter++;


            }
            else if(current_instruction.substr(0,3)=="sub"){
                int reg_store[3]={-1};
                for(int i=0;i<33;i++){
                    if(current_instruction.substr(3,2)==REG[i])
                        reg_store[0]=i;
                    if(current_instruction.substr(5,2)==REG[i])
                        reg_store[1]=i;
                    if(current_instruction.substr(7,2)==REG[i])
                        reg_store[2]=i;

                }
               
                     register_values[reg_store[0]]= register_values[reg_store[1]]-register_values[reg_store[2]];
                      programCounter++;
            }
            else if(current_instruction.substr(0,4)=="addi"){
                string rs,rd,imm;
                int immediate;
                rd=current_instruction.substr(4,2);
                if(current_instruction.substr(6,2)!="ze"){
                    rd=current_instruction.substr(6,2);
                    imm=current_instruction.substr(8);
                    immediate=stoi(imm);
                }
                else{
                     rd=current_instruction.substr(6,4);
                     imm=current_instruction.substr(10);
                    immediate=stoi(imm);
                }
                int reg_store[2]={-1};
                for(int i=0;i<33;i++){
                    if(REG[i].find(rd)!=-1){
                        reg_store[0]=i;
                    }
                    if(REG[i].find(rs)!=-1){
                        reg_store[1]=i;
                    }
                }
                register_values[reg_store[0]]=immediate+register_values[reg_store[1]];
                 programCounter++;
            }
            else if(current_instruction.substr(0,3)=="beq"){
                int reg_store[2]={-1};
                for(int i=0;i<33;i++){
                    if(current_instruction.substr(3,2)==REG[i])
                        reg_store[0]=i;
                    if(current_instruction.substr(5,2)==REG[i])
                        reg_store[1]=i;
                }
                string st = current_instruction.substr(7);
                string addr;
                for(int i=0;i<labeltable.size();i++){
                    if(labeltable[i].labelname==st){
                        addr=labeltable[i].address;
                    }
                }
                if(register_values[reg_store[0]]==register_values[reg_store[1]]){
                    programCounter=stoi(addr);
                }
                else{
                    programCounter++;
                }

            }
            else if(current_instruction.substr(0,3)=="bne"){
                int reg_store[2]={-1};
                for(int i=0;i<33;i++){
                    if(current_instruction.substr(3,2)==REG[i])
                        reg_store[0]=i;
                    else if(current_instruction.substr(5,2)==REG[i])
                        reg_store[1]=i;
                }
                string st = current_instruction.substr(7);
                string addr;
                for(int i=0;i<labeltable.size();i++){
                    if(labeltable[i].labelname==st){
                        addr=labeltable[i].address;
                    }
                }
                if(register_values[reg_store[0]]!=register_values[reg_store[1]]){
                    programCounter=stoi(addr);
                }
                else{
                    programCounter++;
                }

            }
            else if(current_instruction.substr(0,1)=="j"){
              
                string st = current_instruction.substr(1);
                string addr;
                for(int i=0;i<labeltable.size();i++){
                    if(labeltable[i].labelname==st){
                        addr=labeltable[i].address;
                    }
                }
               programCounter=stoi(addr);

            }
           else if(current_instruction.substr(0,2)=="lw"){
                  string rd,rs,offset;
                  rd=current_instruction.substr(2,2);
                  int index=current_instruction.find("(");
                  rs=current_instruction.substr(index+1,2);
                  offset=current_instruction.substr(4,index-4);
                int offs = stoi(offset)/4;
                int value;
                int reg_store[2]={-1};
                for(int i=0;i<33;i++){
                    if(rs==REG[i])
                        reg_store[0]=i;
                    else if(rd==REG[i])
                        reg_store[1]=i;
                }
                value = register_values[reg_store[0]];
                register_values[reg_store[1]] = MEM[(offs + value)/4];
                programCounter++;
             }
            else if(current_instruction.substr(0,2)=="sw"){
                  string rd,rs,offset;
                  rs=current_instruction.substr(2,2);
                  int index=current_instruction.find("(");
                  rd=current_instruction.substr(index+1,2);
                  offset=current_instruction.substr(4,index-4);
                int offs = stoi(offset)/4;
                int value;

                int reg_store[2]={-1};
                for(int i=0;i<33;i++){
                    if(rs==REG[i])
                        reg_store[0]=i;
                    else if(rd==REG[i])
                        reg_store[1]=i;
                }                                     
                value = register_values[reg_store[1]];
                MEM[(offs + value)/4]=register_values[reg_store[0]];
                programCounter++;
            }
            else if(current_instruction.substr(0,3)=="slt"){
                string rd,src1,src2;
                rd=current_instruction.substr(3,2);
                src1=current_instruction.substr(5,2);
                src2=current_instruction.substr(7,2);
                int reg_store[3]={-1};
                for(int i=0;i<32;i++){
                    if(src1==REG[i])
                    reg_store[0]=i;
                    else if(src2==REG[i])
                    reg_store[1]=i;
                    else if(rd==REG[i])
                    reg_store[2]=i;
                }
                if(register_values[reg_store[0]]<register_values[reg_store[1]]){
                    register_values[reg_store[2]]=1;
                }
                else{
                     register_values[reg_store[2]]=1;
                }
                programCounter++;
            }
            else if(current_instruction.substr(0,2)=="la"){ //las0array
                for(int i=0;i<33;i++){
                    if(current_instruction.substr(2,2)==REG[i]){
                        register_values[i]=0; //MEM[0]=0;
                        break;
                    }
                }
                programCounter++;
            }
        }
        void display(){
            if(programCounter<NumberOfInstructions) //display current instruction
	        {
		       cout<<endl<<"Executing instruction: "<<InputProgram[programCounter]<<endl;
	        }
	      else
	       {
		     cout<<endl<<"Executing instruction: "<<InputProgram[programCounter-1]<<endl;
	       }

           cout<<"Registers:"<<endl<<endl
           printf("%10s%10s\n","Register","Value");
           
        }

        void execute(int mode){
        
            preprocess();
            int mainindex;
            for(int i=1;i<=NumberOfInstructions;i++){
                if(InputProgram[i]=="main:"){
                mainindex=i;
                break;
                }
            }
            programCounter=mainindex+1;
            while(programCounter<=NumberOfInstructions){
                string current_instruction = readInstruction(InputProgram[programCounter]);
                processInstruction(current_instruction);
                
            }


    }
};
