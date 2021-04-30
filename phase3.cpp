#define _GLIBCXX_USE_CXX11_ABI 0
#include <bits/stdc++.h>
using namespace std;

int register_values[32]={0};
string REG[32]={"zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"};


string pipeline[500][1000];

class mipsSimulator{
    public:
        int MEM[1024]={0};
        //------------------------------------
        int cache1[1024]={0};
        int tag1[1024]={-1};
        int cache2[1024]={0}; 
        int tag2[1024]={-1};
        int counter1[1024]={0};
        int counter2[1024]={0};
        int cache1Size, cache2Size, block1Size, block2Size;
        int associativity;
        int totalInts1, totalInts2;
        int blockInts1, blockInts2;
        int numblocks1, numblocks2;
        int accessLatency1, accessLatency2; //inputed as number of cycles needed to go to L1 and L2
        int memTime; //number of cycles needed to go to the main memory

        int miss = -1; //this could take only three values
        // miss=0 for a hit in cache l1
        //miss=1 for a miss in l1 and hit in l2;
        //miss=2 for a miss in l1 and l2

        //--------------------------------------
        int programCounter;
        int NumberOfInstructions;
        int branch_flag;
        int MaxLength;//10000
        vector<string> InputProgram; //to store the input program
        vector<string>Input_ins;
        int mainindex=0;
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

        mipsSimulator(string fileName, int cache1Size,int cache2Size,int block1Size,int block2Size,int accessLatency1,int accessLatency2,int memTime){
        programCounter=0;
        NumberOfInstructions=0;
        MaxLength=10000;
        branch_flag=0;
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

        //--------------------------------------
        cache1Size=cache1Size; 
        cache2Size=cache2Size; 
        block1Size=block1Size; 
        block2Size=block2Size;
        accessLatency1=accessLatency1; 
        accessLatency2=accessLatency2;
        memTime=memTime;
        totalInts1 = cache1Size/4;
        totalInts2 = cache2Size/4;
        blockInts1=block1Size/4;
        blockInts2=block2Size/4;
        numblocks1=cache1Size/block1Size;
        numblocks2=cache2Size/block2Size;

        //--------------------------------------
        }

        int nearest(int addrs){
            while(addrs%blockInts1!=0){
                addrs--;
            }
            return addrs;
        }

        bool search1(int addrs){
            for(int i=0;i<numblocks1;i++){
                if(tag1[i]==nearest(addrs)){
                   return true; 
                }
            }
            return false;
        }

        bool search2(int addrs){
            for(int i=0;i<numblocks2;i++){
                if(tag2[i]==nearest(addrs)){
                   return true; 
                }
            }
            return false;
        }

        void L2toL1(int addrs){
            //increment counters
            int i,j;
            for(i=0;i<numblocks2;i++){
                if(tag2[i]==addrs)
                    break;
            }
            for(j=0;j<numblocks1;j++){
                if(tag1[j]==-1){
                    tag1[j]=tag2[i];
                    tag2[i]=-1;

                    int k1=2*j;
                    int k2=2*i;
                    for(int k=0; k<blockInts1; k++){
                        cache1[k1]=cache2[k2];
                        cache2[k2]=0;
                        k1++;
                        k2++;
                    }
                    break;
                }
            }
            if(j==numblocks1){
                //lru
                //check with the counter in cache1, the least one will be pushed 
                //to cache2(if full, we again apply lru for cache2) and then we put 
                // new one in the place that has been emptied.
            }
        }

        void memtoL1(int addrs){
            int j;
            for(j=0;j<numblocks1,j++){
                if(tag1[j]==-1){
                    int k1= 2*j;
                    tag1[j]=nearest(addrs);
                    for(int k=0; k<blockInts1; k++){
                        cache1[k1]=MEM[addrs];
                        k1++;
                        addrs++;
                    }
                    break;
                }
            }
            if(j==numblocks1){
                //lru
            }
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



            for(int k=0;k<InputProgram.size();k++){
                if(InputProgram[k]=="main:")
                mainindex=k;
            }
        int p_count=mainindex+1;
        for(int k=p_count;k<InputProgram.size();k++){
            string current_instrucn=readInstruction(InputProgram[k]);
            Input_ins.push_back(current_instrucn);
        }

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

                //----------------------------------------

                int adrs;
                adrs = (offs + value)/4;
                if(search1(adrs) == true){//hit in L1
                    miss=0;
                    incrementcounter1(adrs);
                }
                else if(search2(adrs) == true){//hit in L2 but was a miss in L1
                    miss=1;
                    //incrementcounter2(adrs) will not happen since L2 is emptied and copied to L1
                    
                    L2toL1(adrs); 
                    //setcounter2tozero(addrs) should happen
                    //incrementcounter1(adrs) should happen since a new value is entered into L1
                }
                else{
                    miss=2;
                    memtoL1(adrs);
                    //incrementcounter1(adrs) 
                }

                //----------------------------------------

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

        void fill(int x,int y,int iF, int id, int ex, int mem, int wb){
            while(iF!=0){
                if(pipeline[x][y]=="stall"){
                    y++;
                }
                else{
                    pipeline[x][y]="stall";
                    y++;
                    iF--;
                }
            }
            while(pipeline[x][y]=="stall"){
                y++;
            }

            pipeline[x][y]="IF";
            y++;
            while(id!=0){
                if(pipeline[x][y]=="stall"){
                    y++;
                }
                else{
                    pipeline[x][y]="stall";
                    y++;
                    id--;
                }
            }
            while(pipeline[x][y]=="stall"){
                y++;
            }
            pipeline[x][y]="ID";
            y++;
            while(ex!=0){
                if(pipeline[x][y]=="stall"){
                    y++;
                }
                else{
                    pipeline[x][y]="stall";
                    y++;
                    ex--;
                }
            }
            while(pipeline[x][y]=="stall"){
                y++;
            }
            pipeline[x][y]="EX";
            y++;
            while(mem!=0){
                if(pipeline[x][y]=="stall"){
                    y++;
                }
                else{
                    pipeline[x][y]="stall";
                    y++;
                    mem--;
                }
            }
            while(pipeline[x][y]=="stall"){
                y++;
            }
            pipeline[x][y]="MEM";
            y++;
            while(wb!=0){
                if(pipeline[x][y]=="stall"){
                    y++;
                }
                else{
                    pipeline[x][y]="stall";
                    y++;
                    wb--;
                }
            }
            while(pipeline[x][y]=="stall"){
                y++;
            }
            pipeline[x][y]="WB";
        }
      
        string hazard(string ins){
            if(ins.substr(0,4)=="addi"){
                return ins.substr(4,2);
            }
            if(ins.substr(0,3)=="add" && (ins.substr(3,1)!="i")){
                return ins.substr(3,2);
            }
            if(ins.substr(0,3)=="sub"){
                return ins.substr(3,2);
            }
            if(ins.substr(0,3)=="mul"){
                return ins.substr(3,2);
            }
            if(ins.substr(0,3)=="div"){
                return ins.substr(3,2);
            }
            if(ins.substr(0,3)=="slt"){
                return ins.substr(3,2);
            }
            if(ins.substr(0,2)=="lw"){
                return ins.substr(2,2);
            }
            if(ins.substr(0,2)=="sw"){
                return ins.substr(2,2);
            }

            return "nulll";
            //if ..... other functions
        }

        bool branchhazard(string ins){
            bool flag=false;
             if(ins.substr(0,3)=="beq"||ins.substr(0,3)=="bne"||(ins.substr(0,1)=="j"&&ins.substr(1,1)!="r")){
                flag=true;
            }
            return flag;
        }
        
        void stalls_hazard(int ins_row){
            int IF,ID,EX,MEM;
            int clk_len=0;
            for(int j=1;j<1000;j++){
                if(pipeline[ins_row][j]=="WB")
                  clk_len=j;
            }
            for(int j=1;j<clk_len;j++){
              if(pipeline[ins_row][j]=="IF")
              IF=j;
               if(pipeline[ins_row][j]=="ID")
              ID=j;
               if(pipeline[ins_row][j]=="EX")
              EX=j;
               if(pipeline[ins_row][j]=="MEM")
              MEM=j;
            }
            for(int j=IF+1;j<ID;j++){
                if(pipeline[ins_row][j]=="stall"){

                  pipeline[ins_row+1][j]="stall";


                }
            }
            for(int j=ID+1;j<EX;j++){
                if(pipeline[ins_row][j]=="stall"){
                  pipeline[ins_row+1][j]="stall";
                }
            }
            for(int j=EX+1;j<MEM;j++){
                if(pipeline[ins_row][j]=="stall"){
                  //fill(ins_row+1,i,0,0,0,1,0);

                  pipeline[ins_row+1][j]="stall";
                }
            }
            for(int j=MEM+1;j<clk_len;j++){
                if(pipeline[ins_row][j]=="stall"){
                  //fill(ins_row+1,i,0,0,0,0,1);
                  pipeline[ins_row+1][j]="stall";
                }
            }

        }

        void fillPipeline(int numb_rows,int flagForwdg){
            int clock1=1;
            int j=0;
            for(int i=0; i<numb_rows; i++){
                j=clock1;

                if(pipeline[i][0].substr(0,4)=="addi"){
                    if(i!=0 && pipeline[i][0].substr(6,2) == hazard(pipeline[i-1][0])){
                         if(flagForwdg==0){//no forwarding
                            stalls_hazard(i-1);
                            if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                            fill(i,j,1,0,2,0,0);
                           }
                            else{
                            fill(i,j,0,0,2,0,0); 
                            }     
                        }
                        else{//with forwarding
                            stalls_hazard(i-1);
                            if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                            fill(i,j,1,0,0,0,0);
                            }
                            else{
                            fill(i,j,0,0,0,0,0);
                            }
                        }
                    }
                    else if(i==0){
                        fill(i,j,0,0,0,0,0);
                    }
                    else{   //i!=0 and no hazard in previous instruction
                         stalls_hazard(i-1);
                        if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                            fill(i,j,1,0,0,0,0);
                        }
                        else{
                            fill(i,j,0,0,0,0,0);
                        }
                       
                    }
                }
                if(pipeline[i][0].substr(0,3)=="add" && pipeline[i][0].substr(3,1)!="i"){
                    if(pipeline[i][0].substr(5,2) == hazard(pipeline[i-1][0]) || pipeline[i][0].substr(7,2) == hazard(pipeline[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                         stalls_hazard(i-1);
                            if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                            fill(i,j,1,0,2,0,0);
                            }
                            else{
                              fill(i,j,0,0,2,0,0);
                            }
                           
                            
                        }
                        else{//with forwarding
                         stalls_hazard(i-1);
                             if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                  fill(i,j,1,0,0,0,0);
                             }
                           
                            else{
                                 fill(i,j,0,0,0,0,0);
                            }
                           
                        }
                       
                    }
                    else{
                        stalls_hazard(i-1);
                        if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                             fill(i,j,1,0,0,0,0);
                         }
                            
                        else{
                            fill(i,j,0,0,0,0,0);
                        }
                       
                    }
                }

                if(pipeline[i][0].substr(0,3)=="sub"){
                    if(pipeline[i][0].substr(5,2) == hazard(pipeline[i-1][0]) || pipeline[i][0].substr(7,2) == hazard(pipeline[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                         stalls_hazard(i-1);
                             if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                 fill(i,j,1,0,2,0,0);
                             }
                            
                            else{
                                fill(i,j,0,0,2,0,0);
                            }
                            
                            //stalls_hazard(i-1);
                        }
                        else{//with forwarding
                             stalls_hazard(i-1);
                             if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                 fill(i,j,1,0,0,0,0);
                             }
                            
                            else{
                                 fill(i,j,0,0,0,0,0);
                            }
                           
                        }
                      
                    }
                    else{
                        stalls_hazard(i-1);
                       if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                           fill(i,j,1,0,0,0,0);
                       }
                            
                        else{
                        fill(i,j,0,0,0,0,0);
                        }
                    }
                }

                if(pipeline[i][0].substr(0,3)=="mul"){
                    if(pipeline[i][0].substr(5,2) == hazard(pipeline[i-1][0]) || pipeline[i][0].substr(7,2) == hazard(pipeline[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                         stalls_hazard(i-1);
                           if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,2,0,0);
                           }
                           
                            else{
                                fill(i,j,0,0,2,0,0);
                            }
                        }
                        else{//with forwarding
                            stalls_hazard(i-1);
                           if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,0,0,0);
                           }
                           
                            else{
                              fill(i,j,0,0,0,0,0);
                            }  
                        } 
                    }
                    else{
                         stalls_hazard(i-1);
                        if(branchhazard(pipeline[i-1][0]) && branch_flag==1){  
                              fill(i,j,1,0,0,0,0);
                        }
                           
                        else{
                          fill(i,j,0,0,0,0,0);
                        }
                        
                    }
                }

                if(pipeline[i][0].substr(0,3)=="div"){
                    if(pipeline[i][0].substr(5,2) == hazard(pipeline[i-1][0]) || pipeline[i][0].substr(7,2) == hazard(pipeline[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                         stalls_hazard(i-1);
                            if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,2,0,0);
                            }
                            
                            else{
                                fill(i,j,0,0,2,0,0);
                            }
                        }
                        else{//with forwarding
                            stalls_hazard(i-1);
                           if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,0,0,0);
                           }
                           
                            else{
                            fill(i,j,0,0,0,0,0);
                            }
                        }   
                    }
                    else{
                        stalls_hazard(i-1);
                        if(branchhazard(pipeline[i-1][0]) && branch_flag==1){  
                             fill(i,j,1,0,0,0,0);
                        }         
                        else{
                             fill(i,j,0,0,0,0,0);
                        }
                       
                    }
                }

                if(pipeline[i][0].substr(0,3)=="slt"){
                   if(pipeline[i][0].substr(5,2) == hazard(pipeline[i-1][0]) || pipeline[i][0].substr(7,2) == hazard(pipeline[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                         stalls_hazard(i-1);
                            if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                 fill(i,j,1,0,2,0,0);
                            }
                            
                            else{
                                fill(i,j,0,0,2,0,0);
                            }
                        }
                        else{//with forwarding
                            stalls_hazard(i-1);
                            if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,0,0,0);
                           }  
                            else{
                            fill(i,j,0,0,0,0,0);
                            }
                        }
                       
                    }
                    else{
                         stalls_hazard(i-1);
                        if(branchhazard(pipeline[i-1][0]) && branch_flag==1){   
                             fill(i,j,1,0,0,0,0);
                        }
                            
                        else{
                             fill(i,j,0,0,0,0,0);
                        }
                       
                    }
                }
                if(pipeline[i][0].substr(0,3)=="beq"){
                     int pc;
                     branch_flag=0;
                     for(int j=0;j<Input_ins.size();j++){
                        if(pipeline[i][0]==Input_ins[j]){
                        pc=j;
                        }
                                                    
                     }
                     if(pipeline[i+1][0]!=Input_ins[pc+1])
                     branch_flag=1;
                     else
                     branch_flag=0;
                    if(pipeline[i][0].substr(3,2) == hazard(pipeline[i-1][0]) || pipeline[i][0].substr(5,2) == hazard(pipeline[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                         stalls_hazard(i-1);
                            if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                 fill(i,j,1,0,2,0,0);
                            }
                            
                            else{
                            fill(i,j,0,0,2,0,0);
                            //stalls_hazard(i-1);
                            }
                        }
                        else{//with forwarding
                            stalls_hazard(i-1);
                           if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,0,0,0);
                           }
                           
                            else{
                            fill(i,j,0,0,0,0,0);
                            }
                        }
                       
                    }
                    else{
                         stalls_hazard(i-1);
                        if(branchhazard(pipeline[i-1][0]) && branch_flag==1){  
                              fill(i,j,1,0,0,0,0);
                        }
                           
                        else{
                          fill(i,j,0,0,0,0,0);
                        }
                        
                    }
                    

                }
                 if(pipeline[i][0].substr(0,3)=="bne"){
                    int pc;
                     branch_flag=0;
                     for(int j=0;j<Input_ins.size();j++){
                        if(pipeline[i][0]==Input_ins[j])
                            pc=j;
                     }
                     if(pipeline[i+1][0]!=Input_ins[pc+1])
                     branch_flag=1;
                     else
                     branch_flag=0;
                     if(pipeline[i][0].substr(3,2) == hazard(pipeline[i-1][0]) || pipeline[i][0].substr(5,2) == hazard(pipeline[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                         stalls_hazard(i-1);
                            if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                 fill(i,j,1,0,2,0,0);
                            }
                            
                            else{
                            fill(i,j,0,0,2,0,0);
                            }
                        }
                        else{//with forwarding
                            stalls_hazard(i-1);
                            if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,0,0,0);
                           }
                            else{
                            fill(i,j,0,0,0,0,0);
                            }
                        }
                       
                    }
                    else{
                         stalls_hazard(i-1);
                        if(branchhazard(pipeline[i-1][0]) && branch_flag==1){  
                            fill(i,j,1,0,0,0,0);
                        }
                           
                        else{
                            fill(i,j,0,0,0,0,0);
                        }
                        
                    }
                     

                }
                 if(pipeline[i][0].substr(0,1)=="j" && pipeline[i][0].substr(1,1)!="r"){
                    int pc;
                     branch_flag=0;
                     for(int j=0;j<Input_ins.size();j++){
                        if(pipeline[i][0]==Input_ins[j])
                            pc=j;
                     }
                     if(pipeline[i+1][0]!=Input_ins[pc+1])
                     branch_flag=1;
                     else
                     branch_flag=0;
                     
                    stalls_hazard(i-1);
                    if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                        fill(i,j,1,0,0,0,0);
                    }
                    else{
                        fill(i,j,0,0,0,0,0);
                    }
                }
                  if(pipeline[i][0].substr(0,2)=="lw"){
                    if(i!=0 && pipeline[i][0].substr(pipeline[i][0].length()-3,2) == hazard(pipeline[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                            stalls_hazard(i-1);
                            if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,2,0,0);
                            }
                            else{
                                fill(i,j,0,0,2,0,0);
                            }
                        }
                        else{//with forwarding
                            stalls_hazard(i-1);
                            if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,0,0,0);
                            }
                            else{
                                fill(i,j,0,0,0,0,0);
                            }  
                        }
                    }
                    else{
                        stalls_hazard(i-1);
                        if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                            fill(i,j,1,0,0,0,0);
                        }
                        else{
                            fill(i,j,0,0,0,0,0);
                        }
                    }
                }

                if(pipeline[i][0].substr(0,2)=="sw"){
                    if(i!=0 && pipeline[i][0].substr(pipeline[i][0].length()-3,2) == hazard(pipeline[i-1][0])){
                        if(flagForwdg==0){//no forwarding
                            stalls_hazard(i-1);
                            if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,2,0,0);
                            }
                            else{
                                fill(i,j,0,0,2,0,0);
                            }
                        }
                        else{//with forwarding
                            stalls_hazard(i-1);
                            if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                                fill(i,j,1,0,0,0,0);
                            }
                            else{
                                fill(i,j,0,0,0,0,0);
                            }  
                        }
                    }
                    else{
                        stalls_hazard(i-1);
                        if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                            fill(i,j,1,0,0,0,0);
                        }
                        else{
                            fill(i,j,0,0,0,0,0);
                        }
                    }
                }


                if(pipeline[i][0].substr(0,2)=="la"){   //data and structural hazards not possible in la
                        stalls_hazard(i-1);
                        if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                            fill(i,j,1,0,0,0,0);
                        }
                        else{
                            fill(i,j,0,0,0,0,0);
                        }
                }

                if(pipeline[i][0].substr(0,2)=="jr"){   //data and structural hazards not possible in jr
                    stalls_hazard(i-1);
                    if(branchhazard(pipeline[i-1][0]) && branch_flag==1){
                        fill(i,j,1,0,0,0,0);
                    }
                    else{
                        fill(i,j,0,0,0,0,0);
                    }
                }

                for(int q=1; q<1000; q++){
                    if(pipeline[i][q]=="IF"){
                            clock1=q+1;
                    }
                }
            }
        }

        void display(){
           cout<<"Registers:"<<"        "<<"Value:"<<endl;
           cout<<endl;
           for(int i=0;i<32;i++){
               cout<<REG[i]<<"                 "<<register_values[i]<<endl;
           }
        }

        void execute(int flag){
            preprocess();
            //int mainindex;
            for(int i=1;i<=NumberOfInstructions-1;i++){
                if(InputProgram[i]=="main:"){
                mainindex=i;
                break;
                }
            }
            programCounter=mainindex+2;

            int pipeRow = 0;
            while(programCounter<=NumberOfInstructions){
                string current_instruction = readInstruction(InputProgram[programCounter-1]); 
                processInstruction(current_instruction);
                //cout << current_instruction << endl;
                pipeline[pipeRow][0]=current_instruction;
                pipeRow++;
            }

            fillPipeline(pipeRow, flag);

            int cnt=0;
            for(int j=1;j<1000;j++){
                if(pipeline[pipeRow-1][j] == "WB"){
                    cout << "Total number of clock cycles: " << j << endl<<endl;
                    cnt=j;
                }
            }

            string stallInstruction[500];
            int count=0;
            int k=0;
            for(int i=0; i<pipeRow ;i++){
                for(int j=1;j<1000;j++){
                    if(pipeline[i][j]=="stall"){
                        count++;
                        stallInstruction[k] = pipeline[i][0];
                    }
                }
                k++;
            }
 
            cout << "Total number of stalls: " << count <<endl<<endl;
            float ipc=(float)pipeRow/cnt;
            cout<<"IPC(Instructions per cycle is) :"<<ipc<<endl<<endl;

            cout << "List of instructions for which stalls occur: " << endl<<endl<<endl;

            for(int i=0;i<k;i++){
                if(stallInstruction[i]!=""){
                    cout << stallInstruction[i] << endl;
                }
            }
            cout<<endl<<endl;
             cout<<"MEMORY:"<<endl;
                for(int i=0;i<1024;i++){
                    if(MEM[i]!=0){
                         cout<<MEM[i]<<" ";
                    }
                }
                cout<<endl;
            return;
        }
};
int main(){
    cout<<"Welcome to Team dynamic MIPS SIMULATOR!!"<<endl;
    //mipsSimulator simulator("BubbleSort.asm");
mipsSimulator simulator("mipsBubblesort.asm");

    int flagFrwd;
    cout << "ENTER 1 for Forwarding and 0 for NO forwarding" << endl;
    cin >> flagFrwd;
    simulator.execute(flagFrwd);
}
