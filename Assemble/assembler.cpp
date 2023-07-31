#include<bits/stdc++.h>
using namespace std;
typedef struct instruction{   //Data type to define all types of instructions with the details required to check for errors
    int type;   //0 ----> Without Operand    1 ---> With Operand  2----> operand with offset   -1 ----> label
    int lineNumber; 
    string name; 
    string operand;
    string machineCode;   //After two pass machineCode generated is stored in this variable
    int pc;
    instruction(int d, string a, string b, int c, int e){
        name = a;
        operand = b;
        pc = c;
        type = d;
        lineNumber = e;
    }
}instruction;
//For Global Variabels used for different operations
int pc = 0;   //To keep track of PC values for instructions
bool illegalNumber = false;  // If a Number is found to be Illegal (Not a number) 
bool overFlow = false; //If size of number given exceed 6 digits of hexadecimal
bool haltExist =false;  //To check if there is Halt in program or not
vector<string>warnings;    //Vector to store warnings
vector<string>pass1Errors; // Stores the errors generated in pass1 like invalide syntax or unknown mnemonics
vector<string>pass2Errors; //Store errors of pass 2
map<string, pair<int, int>>Mnemonics;   //Mnemonics Table 
map<string, pair<int, int>>labels; //In pair first is PC value for label and second tells whether used or not
vector<instruction>instructions;  //Array of instructions to be executed
vector<string>MachineCode; //MachineCode to be written in binary file for execution 
string decToHex(long deci, int size = 6){   //Converts a given decimal number to hexa form of default size 6
    ::overFlow = false;
    bool negative = (deci < 0)?1:0;
    unsigned int num = deci;
    string h = "";
    while(num != 0){
        char a;
        if(num%16 < 10){
            a = char('0' + num%16);
        }else{
            a = char('A' + (num%16 - 10));
        }
        num = num/16;
        h = a + h;
    }
    if(negative){
        string sub = h.substr(0, 3);
        if(sub[0] == 'F' && sub[1] == 'F' && (sub[2] =='8' || sub[2] == '9' || (sub[2] >= 'A' && sub[2] <= 'F'))){
            h.erase(0, 2);
        }else{
            ::overFlow = true;
        }
    }else{
        if(h.length() > size){
            ::overFlow = true;
        }else{
            while(h.length() < size){
                h = "0" + h;
            }
        }
    }
    return h;
}
void initialize(){   //Initialize the mnemomnics table 
    Mnemonics["data"] = make_pair(19, 1);
    Mnemonics["ldc"] = make_pair(0, 1);
    Mnemonics["adc"] = make_pair(1, 1);
    Mnemonics["ldl"] = make_pair(2, 2);
    Mnemonics["stl"] = make_pair(3, 2);
    Mnemonics["ldnl"] = make_pair(4, 2);
    Mnemonics["stnl"] = make_pair(5, 2);
    Mnemonics["add"] = make_pair(6, 0);
    Mnemonics["sub"] = make_pair(7, 0);
    Mnemonics["shl"] = make_pair(8, 0);
    Mnemonics["shr"] = make_pair(9, 0);
    Mnemonics["adj"] = make_pair(10, 1);
    Mnemonics["a2sp"] = make_pair(11, 0);
    Mnemonics["sp2a"] = make_pair(12, 0);
    Mnemonics["call"] = make_pair(13, 2);
    Mnemonics["return"] = make_pair(14, 0);
    Mnemonics["brz"] = make_pair(15, 2);
    Mnemonics["brlz"] = make_pair(16, 2);
    Mnemonics["br"] = make_pair(17, 2);
    Mnemonics["HALT"] = make_pair(18, 0);
    Mnemonics["SET"] = make_pair(20, 1);

}
int stringToInt(string s){   //Converts a number in the string form to integer form 
    illegalNumber = false;
    int num = 0;
    bool negative = false;
    int i = 0;
    if(s[0] == '-'){negative = true;i++;}
    if(s[0]=='+'){i++;}
    for(i;s[i] != '\0';i++){
        if(s[i] >= '0' && s[i] <= '9'){
            num = num*10 + (s[i] - '0');
            continue;
        }
        if(s[i]==','){
            illegalNumber = true;
            return INT_MAX;
        }
        return INT_MAX;
    }
    if(negative){
        num = num * -1;
    }
    return num;
}
bool operandIsNumber(string &operand, int size = 6){  //If the operand is a number then converts it to a Hexa Form Number accepted by strtol function 
    illegalNumber = false;
    const char *cstr = operand.c_str();
    bool negative = false;
    if(operand[0] == '-'){
        negative = true;
    }
    if(operand[0] == '+' || operand[0] == '-'){
        operand.erase(0, 1);
    }
    char *end;
    if(operand[0] == '0' && operand[1] == 'x'){
        long num = strtol(cstr, &end, 16);
        if(negative){num *= -1;}
        if(end ==  '\0'){
            illegalNumber = true;
        }else{
            operand = decToHex(num);
        }
        return true;
    }else if(operand[0] == '0'){
        long num = strtol(cstr, &end, 8);
        if(negative){num *= -1;}
        if(*end != '\0'){
            illegalNumber = true;
        }else{
            operand = decToHex(num);   
        }
        return true;
    }
    if(operand[0] > '0' && operand[0] <= '9'){
        long num = strtol(cstr, &end, 10);
        if(negative){num *= -1;}
        if(*end != '\0'){
            illegalNumber = true;
        }else{
            operand = decToHex(num);
        }
        return true;
    }
    return false;
}
bool validLabel(string label_name){ //Check if the label name is valie as per syntax rules or not 
    if((label_name[0] >= 'a' && label_name[0] <= 'z') || (label_name[0] >= 'A' && label_name[0] <= 'Z')){
        for(int i = 0;i<label_name.length();i++){
            if((label_name[i] >= 'a' && label_name[i] <= 'z') || (label_name[i] >= 'A' && label_name[i] <= 'Z')){
                continue;
            }
            if(label_name[i] == '_'){
                continue;
            }
            if(label_name[i] >= '0' && label_name[i] <= '9'){
                continue;
            }
            return false;
        }
    }else{
        return false;
    }
    return true;
}
string removeSpaceComment(string s){ //Removes all types of excess space and comments from a given string 
    char last = s[0];
    replace(s.begin(), s.end(), '\t', ' ');
    string duplicate = "";
    for(int i = 0;s[i] != '\0';i++){
        if(s[i] == ';'){
            break;
        }
        if(s[i] == ' '){
            if(last == ' '){
                continue;
            }else{
                duplicate = duplicate + s[i];
            }
        }else{
            duplicate = duplicate + s[i];
        }
        last = s[i];
    }
    if(duplicate[duplicate.length()-1] == ' '){
        duplicate.pop_back();
    }
    return duplicate;
}
//Pass1 For the assembler. Goes through each line and puts the corresponding instruction in the instruction in vector
void processLine(string s, int lineNumber){ 
    bool labelPresent = false;
    string label_name = "";
    string inst = "";
    string operand = "";
    string line = removeSpaceComment(s);
    // cout<<"Proessing :"<<line<<endl;
    if(line.length() == 0){
        return;
    }
    int i;
    for(i =0;line[i] != '\0';i++){
        if(line[i] == ':'){
            labelPresent = true;
            break;
        }
        label_name = label_name + line[i];
    }
    if(label_name[label_name.length()-1] == ' '){
        label_name.pop_back();   
    }
    if(labelPresent){
        if(labels.find(label_name) == labels.end()){
            if(validLabel(label_name)){
                labels.insert({label_name, make_pair(pc, 0)});
            }else{
                pass1Errors.push_back("Invalid Label Name `" + label_name + "` at line " + to_string(lineNumber));
            }
        }else{
                pass1Errors.push_back("Duplicate Label at Line  " + to_string(lineNumber));
        }
    }
    if(line[i+1] == '\0' && labelPresent){
        instructions.push_back(instruction(-1, label_name, "", pc, lineNumber));
        return;
    }
    if(labelPresent){
        instructions.push_back(instruction(-1, label_name, "", pc, lineNumber));
        if(line[i+1] == ' '){
            i+=2;
        }else{
            i++;
        }
        for(i; line[i] != ' ' && line[i] != '\0';i++){
            inst = inst + line[i];
        }
        i++;
        for(i;line[i] != '\0';i++){
            operand = operand + line[i];
        }
    }else{
        for(i= 0;line[i] != ' ' && line[i] != '\0';i++){
            inst = inst + line[i];
        }
        if(line[i] != '\0'){i++;}
        for(i;line[i] != '\0';i++){
            operand = operand + line[i];
        }
    }
    auto iter = Mnemonics.find(inst);
    if(iter != Mnemonics.end()){
        if(Mnemonics[inst].second == 0){
            if(operand.length() == 0){
                
            }else{
                pass1Errors.push_back("Unexpected operand at Line "  + to_string(lineNumber));
            }
        }else{
            if(operand.length() == 0){
                pass1Errors.push_back("Expected operand at Line : " +  to_string(lineNumber));
            }else{

            }
        }
        if(inst == "SET"){
            int set_val = stringToInt(operand);
            auto ptr = labels.find(label_name);
            (*ptr).second.first = set_val;
            instructions.push_back(instruction((*iter).second.second, inst, operand, pc++, lineNumber));
            return;
        }
        instructions.push_back(instruction( (*iter).second.second,inst, operand, pc++, lineNumber));
    }else{
        string error = "Bogus Mnemonic `" + inst + "` at line Number : " + to_string(lineNumber);
        pass1Errors.push_back(error);
    }
}
void pass2(){ //Goes through each instruction and set the values of labels and find error if one doesn't exist 
    for(int a = 0;a<instructions.size();a++){
        auto instruction = instructions[a];
        if(instruction.type == -1){
            continue;
        }
        string machine = "";
        int op = (*Mnemonics.find(instruction.name)).second.first;
        string operand = instruction.operand;
        if(operand.size() == 0){
            if(op == 18){
                haltExist = true;
            }
            machine = decToHex(op, 8);
            MachineCode.push_back(machine);
            instructions[a].machineCode = machine;
            continue;
        }
        if(operandIsNumber(operand)){
            if(::illegalNumber){
                pass2Errors.push_back("Illegal Number `" + operand + "` at line " + to_string(instruction.lineNumber));
                MachineCode.push_back("NNNNNNNN");
                instructions[a].machineCode = "NNNNNNNN";
                continue;
            }
            if(::overFlow){
                pass2Errors.push_back("Integer size overflow at line " + to_string(instruction.lineNumber));
                continue;
            }
        }else{
            auto iter = labels.find(operand);
            if(iter == labels.end()){
                pass2Errors.push_back("Undefined label `"+ operand + "`" + " at line " + to_string(instruction.lineNumber));
                continue;
            }else{
                string temp = instruction.name;
                if(Mnemonics[temp].second == 2){
                    operand = decToHex((*iter).second.first - instruction.pc-1);
                }else{
                    operand = decToHex((*iter).second.first);
                }
                (*iter).second.second = 1;
            }

        }
        machine = operand + decToHex(op, 2);
        instructions[a].machineCode = machine;
        MachineCode.push_back(machine);
    }
}
void generateWarnings(ofstream &logfile){  //Function to Write warnings in the log File 
    auto iter = labels.begin();
    if(!haltExist){
        warnings.push_back("Warning : HALT instruction missing");
    }
    while(iter != labels.end()){
        if((*iter).second.second == 0){
            warnings.push_back("Label `" + (*iter).first + "` unused");
        }
        iter++;
    }
    if(warnings.size() != 0){
        logfile<<"-----------Warnings-----------\n";
    }
    for(int i = 0;i<warnings.size();i++){
        string msg = "Warning : "  + warnings[i] + '\n';
        logfile<<msg;
    }
}
bool run(ofstream &logfile){   //Write down the errors in Log File if exist. If no error returns true;
    int Error = 1;
    if(pass1Errors.size() != 0 || pass2Errors.size() != 0){
        cout<<"Failed to assemble : "<<endl<<"-------CHECK LOG FILE---------";
        logfile<<"            Failed to assemble :(\n\n";
        for(int i = 0;i<pass1Errors.size();i++){
            string msg = "Error " + to_string(Error) + ": " + pass1Errors[i] + '\n';
            logfile<<msg;
            Error++;
        }
        cout<<endl;
        for(int i =0;i<pass2Errors.size();i++){
            string msg = "Error " + to_string(Error) + ": " + pass2Errors[i] + '\n';
            logfile<<msg;
            Error++;
        }
        generateWarnings(logfile);
        logfile.close();
        return false;
    }
    generateWarnings(logfile);
    return true;
}
void writeMachine(ofstream &binFile){  //Write the machineCode generated in .o file 
    unsigned int y ;
	for(auto & x : MachineCode){
        cout<<x<<endl;
		stringstream ss ;
		ss<<hex<< x ;
		ss>>y ;
		static_cast<int>(y);
		binFile.write((const char*)&y , sizeof(unsigned )) ;
	}
    binFile.close() ;
}
string getLeftPaddingString(string const &str, int n, char paddedChar = '0')  //Add 0s at starting of string for Padding
{
    std::ostringstream ss;
    ss <<right<<setfill(paddedChar) <<std::setw(n)<<str;
    return ss.str();
}
void generateListing(ofstream &listfile){  //Function to write Listing file 
    for(int i = 0;i<instructions.size();i++){
        auto iter = instructions[i];
        if(iter.type != -1){
            string msg = getLeftPaddingString(to_string(iter.pc), 8) +  '\t' + iter.machineCode + '\t' + iter.name + ' '+ iter.operand + '\n';
            listfile<<msg;
        }else{
            if(i+1 == instructions.size()){ //If last instruction
                string msg = getLeftPaddingString(to_string(iter.pc), 8) + '\t' + '\t' + '\t' + iter.name + '\n';
                listfile<<msg;
                continue;
            }else{
                if(instructions[i+1].name == "SET"){
                    string msg = getLeftPaddingString(to_string(iter.pc), 8) + '\t' + 
                                instructions[i+1].machineCode + '\t' + iter.name + ": " +
                                instructions[i+1].name + ' ' + instructions[i+1].operand + '\n';
                    listfile<<msg;
                    i++;
                }else{
                     string msg = getLeftPaddingString(to_string(iter.pc), 8) + '\t' + '\t' + '\t' + iter.name + '\n';
                     listfile<<msg;
                }
            }
        }
    }
}
ifstream asmfile;
string file_name = "";
int main(int argc , char* argv[]) //Accepts File name to be assembled 
{	
	if(argc != 2){
        cout<<".asm file expected for compilation :(";
        exit(1);
    }
    file_name = argv[1];
    string fname = file_name; //File name without extension 
    string extension = file_name.substr(file_name.length()-3, 3); //Extension of file 
    if(extension != "asm"){  //Disallow any file other than .asm 
        cout<<"Only .asm files allowed";
        exit(1);
    }
    fname.erase(fname.length()-4, 4);
    asmfile.open(file_name);
    if(asmfile.fail()){
        cout<<"Something went wrong while opening file.. Please try again";
        exit(1);
    }
    initialize(); //Initializing the mnemonics table 
    string s;
    int i = 1;
    while(getline(asmfile, s)){ //Processing each line one by one 
        processLine(s, i);
        i++;
    }
    pass2();  //Pass2 
    ofstream logfile(fname+".log");
    ofstream listfile(fname + ".lst");
    ofstream mcfile(fname+".o", ios::out | ios::binary);
    generateListing(listfile);
    if(run(logfile)){  //Write down the log file. And if no error return true
        if(warnings.size() == 0){
            cout<<"-----------Assembled Successfully :)-----------------------"<<endl;
            cout<<"-----------Check "<<fname<<".o"<<" for machine code--------------"<<endl;
        }else{
            cout<<"-----------Assembled Successfully with warnings :)-----------"<<endl;
            cout<<"-----------Check "<<fname<<".o"<<" for machine code----------"<<endl;
        }
        writeMachine(mcfile);
        if(!mcfile.good()){
            cout<<"Something went wrong while writing machine code file :("<<endl;
            return 1;
        }
    }
    return 0;
}