#define _GLIBCXX_USE_CXX11_ABI 0
#include <bits/stdc++.h>
using namespace std;

int register_values[32]={0};
string REG[32]={"zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"};


string pipeline[500][1000];

class mipsSimulator{
public:
    int MEM[1024]={0};
    int miss_arr[1024]={0};
       int programCounter;
       int NumberOfInstructions;
       int branch_flag;
       int miss=0;
       int countmiss=0;
       int L1_filled=0,L2_filled=0;
      int  numofblocks_l1,numOfints_l1,numOfSets_l1, offset_l1,index_l1,tag_l1 ;
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
        struct Block{
            int arrOfints[1024];
            Block(){
                for(int i=0;i<1024;i++){
                    arrOfints[i]=0;
                }
            }
        };
        

        struct cache{
            //int numofblocks;
            string tag[1024];
            int dirty[1024];
             Block blk_arr[1024];
            int counter[1024];
            //string set[1024];
            cache(){
                for(int i=0;i<1024;i++){
                    dirty[i]=0;
                    counter[i]=0;
                }
                for(int j=0;j<1024;j++){
                    //set[j]="NULL";
                     tag[j]="NULL";

                }
            }
        };
        
         
        mipsSimulator(string fileName,int l1_cachesize ,int l1_blocksize, int l1_assoc,int l1_time,int l2_cachesize ,int l2_blocksize, int l2_assoc,int l2_time,int mem_time){//in terms of bytes
        programCounter=0;
        NumberOfInstructions=0;
        MaxLength=10000;
        branch_flag=0;
        //fix address to 8 bits
    int  numofblocks_l1,numOfints_l1,numOfSets_l1, offset_l1,index_l1,tag_l1,time_l1;
    int  numofblocks_l2,numOfints_l2,numOfSets_l2, offset_l2,index_l2,tag_l2,time_l2;
    int mem_time;
      numofblocks_l1 = l1_cachesize/l1_blocksize;
        numOfints_l1 = l1_blocksize/4;
        numOfSets_l1 = numofblocks_l1/l1_assoc;

        offset_l1 = logValue(numofblocks_l1);
        index_l1 = logValue(numOfSets_l1);
        tag_l1 = 8 - index_l1 - offset_l1;

         numofblocks_l2= l2_cachesize/l2_blocksize;
        numOfints_l2 = l2_blocksize/4;
        numOfSets_l2 = numofblocks_l2/l2_assoc;

        offset_l2 = logValue(numofblocks_l2);
        index_l2 = logValue(numOfSets_l2);
        tag_l2 = 8 - index_l2 - offset_l2; 

       

        
        

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
        struct cache cac_l1;
        struct cache cac_l2;
        string decToBinary(int n,int size)
        {
          string temp="";
         for (int i = size-1; i >= 0; i--) {
               int k = n >> i;
             if (k & 1)
                temp=temp+"1";
              else
                temp=temp+"0";
          }
          return temp;
       }
       int binaryToDecimal(string n)
     {
         string num = n;
        int dec_value = 0;
 
    // Initializing base value to 1, i.e 2^0
        int base = 1;
 
       int len = num.length();
      for (int i = len - 1; i >= 0; i--) {
        if (num[i] == '1')
            dec_value += base;
        base = base * 2;
      }
 
      return dec_value;
  }
       int logValue(int val){
            //function - returns log to the base 2
            int t=log2(val);
            return t;
        }
        int closestMultiple(int n, int x)
         {   
              if(x>n)
                return 0;
  
             n = n + x/2;
             n = n - (n%x);
           return n;
         }
        
         int getMin(int arr[], int n)
        {
             int res = arr[0];
             int index=0;
             for (int i = 1; i < n; i++){
                  if(res>arr[i]){
                      res=arr[i];
                  }
             }  
             for(int i=0;i<n;i++){
                 if(res==arr[i]){
                 index=i;
                 break;
                 }
             }
             return index;
         }
        int search(string addr,int offs,int val){
            L1_filled=0;//not empty
            for(int k=0;k<numofblocks_l1;k++){
               if(cac_l1.tag[k]=="NULL")
               L1_filled=1;
            }
            int i;
            for(i=0;i<numofblocks_l1;i++){
                if(cac_l1.tag[i]==addr){
                    for(int j=0;j<numOfints_l1;j++){
                        if(cac_l1.blk_arr[i].arrOfints[j]==MEM[(offs+val/4)]){
                            cac_l1.counter[i]+=1;
                            miss=0;
                            break;
                        }
                    }
                }
            }
            if(i==numofblocks_l1 ){
                miss=1;
                countmiss++;
              if(L1_filled==1){
               for(int m=0;m<numofblocks_l1;m++){
                   if(cac_l1.tag[m]=="NULL"){
                       int dec=binaryToDecimal(addr);
                       int closest_addr=closestMultiple(dec,numOfints_l1);
                       cac_l1.tag[m]=decToBinary(closest_addr,8);
                       for(int k=0;k<numOfints_l1;k++){
                           cac_l1.blk_arr[m].arrOfints[k]=MEM[closest_addr];
                           closest_addr++;
                       }
                       break;
                   }
               }
              }
              else{
                  //have to apply LRU
                  int min=getMin(cac_l1.counter,numofblocks_l1);
                  cac_l1.tag[min]=addr;
                   int dec=binaryToDecimal(addr);
                       int closest_addr=closestMultiple(dec,numOfints_l1);
                       for(int k=0;k<numOfints_l1;k++){
                           cac_l1.blk_arr[min].arrOfints[k]=MEM[closest_addr];
                           closest_addr++;
                       }

              }

               
             }
             return miss;

        }
        void writeback(string addr,int offs,int val){
              

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
            miss=0;
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

                //cachesearch() - updates miss variable by searching in cache(includes splitting the adress into tag, offset,index)

                string temp_addr=decToBinary((offs+value)/4,8);
                

                if(miss==true){
                    if(/*l1 and l2 are full*/ ){
                        //do LRU
                    }
                    else{
                        //go to mem and bring the value put it in cache
                    }
                }

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
    mipsSimulator simulator("BubbleSortlite.asm");


    int flagFrwd;
    cout << "ENTER 1 for Forwarding and 0 for NO forwarding" << endl;
    cin >> flagFrwd;
    simulator.execute(flagFrwd);
}
