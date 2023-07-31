#include<bits/stdc++.h>
using namespace std;
vector<pair<string, int>>machineCode; //MachineCode to be executed. Int stores how much time a particular instruction has been exectued
vector<string>data(10112, "00000000");  //Memory  //Top of the memory is stack
map<string, string>Mnemonics; //Opcode and Mnemonic name 
bool compare(string a, string b){  //Compare if two strings are equal or not 
	if(a.length() != b.length()){
		return false;
	}
	int i= 0;
	while(a[i] != '\0'){
		if(a[i] != b[i]){
			return false;
		}
		i++;
	}
	return true;
}
void initialize(){   //It intialize the Mnemmonics table in reverse order so that we can get the operation using it's opCode
    Mnemonics["13"] = "data";
    Mnemonics["00"] = "ldc";
    Mnemonics["01"] = "adc";
    Mnemonics["02"] = "ldl";
    Mnemonics["03"] = "stl";
    Mnemonics["04"] = "ldnl";
    Mnemonics["05"] = "stnl";
    Mnemonics["06"] = "add";
    Mnemonics["07"] = "sub";
    Mnemonics["08"] = "shl";
    Mnemonics["09"] = "shr";
    Mnemonics["0A"] = "adj";
    Mnemonics["0B"] = "a2sp";
    Mnemonics["0C"] = "sp2a";
    Mnemonics["0D"] = "call";
    Mnemonics["0E"] = "return";
    Mnemonics["0F"] = "brz";
    Mnemonics["10"] = "brlz";
    Mnemonics["11"] = "br";
    Mnemonics["12"] = "HALT";
    Mnemonics["14"] = "SET";
}
void onesComplement(string &s){
    for(int i = s.size()-1; i>= 0;i--){
        if(s[i] >= '6' && s[i] <= '9'){
            int t= s[i] - '0';
            t = 15 - t;
            s[i] = '0' + t;
            continue;
        }
        if(s[i] >= 'A' && s[i] <= 'F'){
            int t = 10 + (s[i] - 'A');
            t = 15 - t;
            s[i] = '0' + t;
            continue;
        }
        if(s[i] >= '0' && s[i] <= '5'){
            int t = s[i] - '0';
            t = 5 - t;
            s[i] = 'A' + t;
            continue;
        }
    }
}
int hexaToDeci(string s){    //Function to convert the Offset Or Value of Machine Code to Integer 
	bool negative = false;
	if(s[0] >= '8' || (s[0] >= 'A' && s[0] <= 'F')){
		onesComplement(s);
        negative = true;
	}
	int val = stoi(s, nullptr, 16);
	if(negative){
        val = (val+1) * -1;
    }
	return val;
}
string instruct(string machine){  //Converts a machine Code to corresponding instruction 
	string mnemonic = (*Mnemonics.find(machine.substr(6, 2))).second;
	int val = hexaToDeci(machine.substr(0, 6));
	return mnemonic + " " + to_string(val);
}
int PC = 0;   //Gloabl PC
long long SP = 10111;   //Stack Pointer at the top of Memory Space (277F);
int A = 0;  //Registers A and B 
int B = 0;
int dataMemoryUsed = 0;  //Part Of memory used to store instructions 
string IntToHexa(int n){  //Converts a number to Hexa Form of Twos Complement Form 
	unsigned int x = n ;
	string ans ;
	while(x!=0){
		int rem = x%16;
		if(rem <= 9){
			ans = char('0' + rem) + ans;
		}else{
			ans = char('A' + (rem - 10)) + ans;
		}
		x/= 16;
	}   
    while(ans.length() > 8){
        ans.erase(0, 1);
    }
    while(ans.length() < 8){
        ans = '0' + ans;
    }
	return ans ;
}
bool done = false;   //If Halt is found it terminates 
vector<string>traces;  //Store the trace status 
int maxTime = 0;   //Checks if there is any infinite loop 
bool terminated = false;
void execute(string machine){    //Execute the Instruction based on the opCode 
	string op = machine.substr(6, 2);
	int opcode = hexaToDeci(op);
	int value = hexaToDeci(machine.substr(0, 6));
	switch(opcode){
		case 0:
				B = A;
				A = value;
				break;
		case 1:
				A = A + value;
				break;
		case 2:
				B = A;
				A = hexaToDeci(data[SP + value]);
				break;
		case 3:
				data[SP + value] = IntToHexa(A);
				A = B;
				break;
		case 4:
				A  = hexaToDeci(data[A + value]);
				break;
		case 5:
				data[A + value] = IntToHexa(B);
				break;
		case 6:
				A = A + B;
				break;
		case 7:
				A = B-A;
				break;
		case 8:
				A = (B<<A);
				break;
		case 9:
				A = (B>>A);
				break;
		case 10:
				SP = SP + value;
				break;
		case 11:
				SP = A;
				A = B;
				break;
		case 12:
				B = A;
				A = SP;
				break;
		case 13:
				B = A;
				A = PC;
				PC += value;
				break;
		case 14:
				PC = A;
				A = B;
				break;
		case 15:
				if(A == 0){
					PC = PC + value;
				}
				break;
		case 16:
				if(A<0){
					PC = PC + value;
				}
				break;
		case 17:
				PC = PC + value;
				break;
		case 18:
				done = true;
				break;
		default:
				break;
	}
}
void run(bool trace = false){  
	if(trace){
		string temp = "";
		temp = temp +  "  PC  " + '\t' + "    Stack Pointer(SP)" + '\t' + "    A" + '\t' + "            B\n";
		traces.push_back(temp);
	}
	for(PC;;){
		string op = machineCode[PC].first.substr(6, 2);
		if(trace){
			string temp = "";
			temp = temp + IntToHexa(PC) + "\t" + IntToHexa(SP) + "\t" + IntToHexa(A) + '\t' + IntToHexa(B) + '\t' + instruct(machineCode[PC].first) + '\n';
			traces.push_back(temp);
		}
		execute(machineCode[PC].first);
		machineCode[PC].second++;
		if(machineCode[PC].second > maxTime){
			maxTime = machineCode[PC].second;
			if(maxTime > 10000){
				cout<<"Infinite Loop Detected "<<endl;
				terminated = true;
				return;
			}
		}
		if(done){
			return;
		}
		if((PC+1) != machineCode.size()){
			PC++;
		}else{
			cout<<"Fatal Error : No instruction found (Possibly HALT)"<<endl;
			terminated = true;
			return;
		}
	}
}
bool compare(char *a, string b){  //Function to compare two strings 
	int i = 0;
	while(*a != '\0'){
		if(*a == b[i]){
			a++;
			i++;
		}else{
			return false;
		}
	}
	return true;
}
bool readFile(string name){   //Function to read binary file and putting the MachineCode in vector 
	int read;
	ifstream file(name, ios::in | ios::binary);
	if(file.fail()){
		cout<<"File couldn't be read :("<<endl;
		return false;
	}
	int i = 0;
    while(file.read((char*)&read,sizeof(int)))
    {
	   string temp = IntToHexa(read);
	   string op = temp.substr(6, 2);
	   string first = "14";
	   string second = "13";
	   machineCode.push_back(make_pair(temp, 0));
	   if(compare(op, first) || compare(op, second)){
			temp.erase(temp.length()-2, 2);
			temp = "00" + temp;
	   }
	  data[i] = temp;
	  dataMemoryUsed++;
	  i++;
    }
	return true;
}
int main(int argc , char* argv[])
{	

	if(argc == 2){
		cout<<"Select one of the options : "<<endl;
		cout<<"1. trace"<<endl<<"2. before"<<endl<<"3. after"<<endl;
		cout<<"Write as : emu.exe -option "<<"binaryFile.o"<<endl;
		return 0;
	}
	if(argc == 1){
		cout<<"Give the binary file as input :)"<<endl;
		return 0;
	}
	initialize();
	string name = argv[2];
	if(readFile(name)){
		if(compare(argv[1], "-before")){
     		cout<<"Printing memory dump before execution : "<<endl;
     		for(int i = 0;i<dataMemoryUsed;i++){
     			if(i%4 == 0){
     				cout<<IntToHexa(i)<<"\t";
     			}
     			cout<<data[i]<<'\t';
     			if(i%4 == 3){
     				cout<<endl;
     			}
     		}
			return 0;
		}
		if(compare(argv[1], "-after")){
			run();
			if(!terminated){
				for(int i = 0;i<dataMemoryUsed;i++){
        			if(i%4 == 0){
        				cout<<IntToHexa(i)<<"\t";
        			}
        			cout<<data[i]<<'\t';
        			if(i%4 == 3){
        				cout<<endl;
        			}
     			}
			}
			return 0;
		}
		if(compare(argv[1], "-trace")){
			run(true);
			if(!terminated){
				ofstream traceFile(name.erase(name.length()-2, 2) + ".trace");
				for(int i = 0;i<traces.size();i++){
					traceFile<<traces[i];
					cout<<traces[i];
				}
				cout<<endl<<"Register values after Execution : "<<"A: "<<A<<"\t"<<"B: "<<B;
				cout<<endl<<"---------------------Memory After Execution---------------------------"<<endl;
				for(int i = 0;i<dataMemoryUsed;i++){
     				if(i%4 == 0){
     					cout<<IntToHexa(i)<<"\t";
     				}
     				cout<<data[i]<<'\t';
     				if(i%4 == 3){
     					cout<<endl;	
   					}
			}
			return 0;
			}
		}
		if(compare(argv[1], "-isa")){
			for(int i = 0;i<dataMemoryUsed;i++){
				cout<<instruct(data[i])<<endl;
			}
		}
	}
}