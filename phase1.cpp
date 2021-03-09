#define _GLIBCXX_USE_CXX11_ABI 0
#include <bits/stdc++.h>
using namespace std;

int register_values[32]={0};
string REG[32]={"zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"};

class mipsSimulator{
public:
    int MEM[1024]={0};
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

        ifstream InputFile;
        InputFile.open(fileName.c_str(),ios::in); //open file
        if(!InputFile){ //if open failed
            cout<<"Error: File does not exist or could not be opened"<<endl;
            exit(1);
        }
        string tempString;
        while(getline(InputFile,tempString)){ //read line by line
        //readInstruction(tempString);
            NumberOfInstructions++;
            if(NumberOfInstructions>MaxLength){ ///check number of instructions with maximum allowed
                cout<<"Error: Number of lines in input too large, maximum allowed is "<<MaxLength<<" line"<<endl;
                exit(1);
            }
            InputProgram.push_back(tempString); //store in InputProgram
        }
        InputFile.close();
        }
        string readInstruction(string str){
            if(str.find("#")!=-1){ //remove comments
                    str=str.substr(0,str.find("#"));
                    }
            str.erase(remove(str.begin(), str.end(), ' '), str.end());
            str.erase(remove(str.begin(), str.end(), ','), str.end());
            str.erase(remove(str.begin(), str.end(), '$'), str.end());
           return str;
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
            int wordindex,arrayindex;
            if(current_section==0){
                for(i=dataStart+1;i<NumberOfInstructions;i++){
                    string current_instruction="";
                    current_instruction=InputProgram[i];
                    current_instruction = readInstruction(current_instruction);
                    arrayindex=current_instruction.find(":");//array:.word9315
                    wordindex=current_instruction.find(".word");
                    int storeline;
                    if(wordindex==-1 && arrayindex==-1){
                        if(current_instruction.find(".text")==-1){ //if text section has not started
                            cout<<"Error: Unexpected symbol in data section"<<endl;
                        }
                        else{
                            break;
                        }
                    }
                    else{
                        string num=current_instruction.substr(arrayindex+6);//array:.word9135
                        //lets assume array values are <10
                        int k=0;
                        for(int i=0;i<num.length();i++){
                            MEM[k]=stoi(num.substr(i,1));
                            k++;
                        }
                    } 
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
                    temp=current_instruction.substr(0,j+1);
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


        void processInstruction(string current_instruction){
            if(current_instruction.substr(0,3)=="add" && current_instruction.substr(3,1)!="i"){
                int reg_store[3]={-1};
                for(int i=0;i<32;i++){
                    if(current_instruction.substr(3,2)==REG[i])
                        reg_store[0]=i;
                    if(current_instruction.substr(5,2)==REG[i])
                        reg_store[1]=i;
                    if(current_instruction.substr(7,2)==REG[i])
                        reg_store[2]=i;
                }
        
                register_values[reg_store[0]]= register_values[reg_store[1]]+ register_values[reg_store[2]];
                programCounter++;
                return;
            }
            if(current_instruction.substr(0,3)=="sub"){
                int reg_store[3]={-1};
                for(int i=0;i<32;i++){
                    if(current_instruction.substr(3,2)==REG[i])
                        reg_store[0]=i;
                    if(current_instruction.substr(5,2)==REG[i])
                        reg_store[1]=i;
                    if(current_instruction.substr(7,2)==REG[i])
                        reg_store[2]=i;

                }
               
                     register_values[reg_store[0]]= register_values[reg_store[1]]-register_values[reg_store[2]];
                      programCounter++;
                      return;
            }
            if(current_instruction.substr(0,3)=="mul"){
                int reg_store[3]={-1};
                for(int i=0;i<32;i++){
                    if(current_instruction.substr(3,2)==REG[i])
                        reg_store[0]=i;
                    if(current_instruction.substr(5,2)==REG[i])
                        reg_store[1]=i;
                    if(current_instruction.substr(7,2)==REG[i])
                        reg_store[2]=i;

                }
               
                     register_values[reg_store[0]]= register_values[reg_store[1]]*register_values[reg_store[2]];
                      programCounter++;
                      return;
            }
            if(current_instruction.substr(0,3)=="div"){
                int reg_store[3]={-1};
                for(int i=0;i<32;i++){
                    if(current_instruction.substr(3,2)==REG[i])
                        reg_store[0]=i;
                    if(current_instruction.substr(5,2)==REG[i])
                        reg_store[1]=i;
                    if(current_instruction.substr(7,2)==REG[i])
                        reg_store[2]=i;

                }
               
                     register_values[reg_store[0]]= register_values[reg_store[1]]/register_values[reg_store[2]];
                      programCounter++;
                      return;
            }


            if(current_instruction.substr(0,4)=="addi"){//addit2t34
                string rs,rd,imm;
                int immediate;
                rd=current_instruction.substr(4,2);
                if(current_instruction.substr(6,2)!="ze"){
                    rs=current_instruction.substr(6,2);
                    imm=current_instruction.substr(8);
                    immediate=stoi(imm);
                }
                else{
                     rs=current_instruction.substr(6,4);
                     imm=current_instruction.substr(10);
                    immediate=stoi(imm);
                }
                int reg_store[2]={-1};
                for(int i=0;i<32;i++){
                    if(rd==REG[i])
                        reg_store[0]=i;
                    if(rs==REG[i])
                        reg_store[1]=i;
                }
                register_values[reg_store[0]]=immediate+register_values[reg_store[1]];
                programCounter++;
                return;
            }

            
            
            
            if(current_instruction.substr(0,3)=="beq"){
                string st;
                int reg_store[2]={-1};
                if(current_instruction.substr(5,2)=="ze"){
                    for(int i=0;i<32;i++){
                        if(current_instruction.substr(3,2)==REG[i])
                            reg_store[0]=i;
                        if(current_instruction.substr(5,4)==REG[i]) //beqt0zeroLABEL
                            reg_store[1]=i;
                    }
                    st = current_instruction.substr(9);
                }
                else{
                    for(int i=0;i<32;i++){
                        if(current_instruction.substr(3,2)==REG[i])
                            reg_store[0]=i;
                        if(current_instruction.substr(5,2)==REG[i])
                            reg_store[1]=i;
                    }
                    st = current_instruction.substr(7);
                }

                string addr;
                for(int i=0;i<labeltable.size();i++){
                    if(labeltable[i].labelname==st){
                        addr=labeltable[i].address;
                    }
                }
                if(register_values[reg_store[0]]==register_values[reg_store[1]]){
                    programCounter=stoi(addr) + 1;
                }
                else{
                    programCounter++;
                }
                return;
            }

            if(current_instruction.substr(0,3)=="bne"){
                string st;
                int reg_store[2]={-1};
                if(current_instruction.substr(5,2)=="ze"){  //bnet2t3LABEL
                    for(int i=0;i<32;i++){
                        if(current_instruction.substr(3,2)==REG[i])
                            reg_store[0]=i;
                        if(current_instruction.substr(5,4)==REG[i]) //beqt0zeroLABEL
                            reg_store[1]=i;
                    }
                    st = current_instruction.substr(9);
                }
                else{
                    for(int i=0;i<32;i++){
                        if(current_instruction.substr(3,2)==REG[i])
                            reg_store[0]=i;
                        if(current_instruction.substr(5,2)==REG[i])
                            reg_store[1]=i;
                    }
                    st = current_instruction.substr(7);
                }
                string addr;
                for(int i=0;i<labeltable.size();i++){
                    if(labeltable[i].labelname==st){
                        addr=labeltable[i].address;
                    }
                }
                if(register_values[reg_store[0]]!=register_values[reg_store[1]]){
                    programCounter=stoi(addr) + 1;
                }
                else{
                    programCounter++;
                }
                return;
            }

            if(current_instruction.substr(0,1)=="j" && current_instruction.substr(1,1)!="r"){
              
                string st = current_instruction.substr(1);
                string addr;
                for(int i=0;i<labeltable.size();i++){
                    if(labeltable[i].labelname==st){
                        addr=labeltable[i].address;
                    }
                }
               programCounter=stoi(addr) + 1;
               return;

            }
           if(current_instruction.substr(0,2)=="lw"){
                  string rd,rs,offset;
                  rd=current_instruction.substr(2,2);
                  int index=current_instruction.find("(");
                  rs=current_instruction.substr(index+1,2);
                  offset=current_instruction.substr(4,index-4);
                int offs = stoi(offset);
                int value;
                int reg_store[2]={-1};
                for(int i=0;i<32;i++){
                    if(rs==REG[i])
                        reg_store[0]=i;
                    else if(rd==REG[i])
                        reg_store[1]=i;
                }
                value = register_values[reg_store[0]];
                register_values[reg_store[1]] = MEM[(offs + value)/4];
                programCounter++;
                return;
             }
            if(current_instruction.substr(0,2)=="sw"){
                  string rd,rs,offset;
                  rs=current_instruction.substr(2,2);
                  int index=current_instruction.find("(");
                  rd=current_instruction.substr(index+1,2);
                  offset=current_instruction.substr(4,index-4);
                int offs = stoi(offset);
                int value;

                int reg_store[2]={-1};
                for(int i=0;i<32;i++){
                    if(rs==REG[i])
                        reg_store[0]=i;
                    else if(rd==REG[i])
                        reg_store[1]=i;
                }                                     
                value = register_values[reg_store[1]];
                MEM[(offs + value)/4]=register_values[reg_store[0]];
                programCounter++;
                return;
            }
            if(current_instruction.substr(0,3)=="slt"){
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
                     register_values[reg_store[2]]=0;
                }
                programCounter++;
                return;
            }
            if(current_instruction.substr(0,2)=="la"){ //las0array
                for(int i=0;i<32;i++){
                    if(current_instruction.substr(2,2)==REG[i]){
                        register_values[i]=0; //MEM[0]=0;
                        break;
                    }
                }
                programCounter++;
                return;
            }
            if(current_instruction.substr(0,2)=="jr"){
                programCounter++;
                return;
            }
        }
        
        void display(){
           cout<<"Registers:"<<"        "<<"Value:"<<endl;
           cout<<endl;
           for(int i=0;i<32;i++){
               cout<<REG[i]<<"                 "<<register_values[i]<<endl;
           }
        }

        void execute(int mode){
            preprocess();
            int mainindex;
            for(int i=1;i<=NumberOfInstructions-1;i++){
                if(InputProgram[i]=="main:"){
                mainindex=i;
                break;
                }
            }
            programCounter=mainindex+2;
            if(mode==1){
            while(programCounter<=NumberOfInstructions){
                string current_instruction = readInstruction(InputProgram[programCounter-1]);
                 cout << "ProgramCounter:" << programCounter << endl;
                processInstruction(current_instruction);
                cout<<"MEMORY:"<<endl;
                for(int i=0;i<1024;i++){
                    if(MEM[i]!=0){
                         cout<<MEM[i]<<" ";
                    }
                }
                cout<<endl;
            }
            //cout << "ProgramCounter:" << programCounter << endl;
            cout<<endl<<endl;
            display();
            return;
            }
            else{
                 while(programCounter<=NumberOfInstructions){
                string current_instruction = readInstruction(InputProgram[programCounter-1]); 
                processInstruction(current_instruction);
            }
            cout << "ProgramCounter:" << programCounter << endl;
            cout<<endl;
            cout<<endl;
            display();
            cout<<endl;
            cout<<endl;
             cout<<"MEMORY:"<<endl;
                for(int i=0;i<1024;i++){
                    if(MEM[i]!=0){
                         cout<<MEM[i]<<" ";
                    }
                }
                cout<<endl;
            return;
            }
        }
};
int main(){
    cout<<"Welcome to Team dynamic MIPS SIMULATOR!!"<<endl;
    int mode;
    cout<<"Enter mode 1 or 2:      1.Step-bystep execution   2.Final output"<<endl;
    cin>>mode;
    mipsSimulator simulator("BubbleSort.asm");
    simulator.execute(mode);
}
