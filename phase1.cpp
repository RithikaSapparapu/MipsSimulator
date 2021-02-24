#include <bits/stdc++.h>
using namespace std;


class generalFunctions{
    public:

        void terminate(){   
            cout << "oops! An Error has occured!" << endl;
            exit(0);
        }

        int power(int a, int b){
            int ans = 1;
            for (int i=0; i < b; i++){
                ans = ans*a;
            }
            return ans;
        }

        //converts int decimal to string binary
        string DecimalToBinary(int decimalVal){
            int i = 0, ans = 0;
            while(decimalVal >= 2){
                ans += power(10,i)*(decimalVal % 2);
                decimalVal /= 2;
                i++;
            }
            ans = ans + power(10,i)*(decimalVal);
            ostringstream strr;
            strr << ans;
            return strr.str();
        }

        //converts string binary to int decimal
        int BinaryToDecimal(string BinaryVal){
            int ans = 0;
            for (int i = 0; i < inp.length(); i++){
                ans = ans + power(2,i)*(inp.at(inp.length() - i - 1) - '0');
            }
            return ans;
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




class registers{
    public:
        string REG[32];
        
        generalFunctions gen;
        
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