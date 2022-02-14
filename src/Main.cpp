#include <iostream>
#include <stack>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

vector<string> readFile(string fileName){   // Reading file function
    fstream new_file;
    vector<string> str;
    new_file.open(fileName, ios::in);
    string line;
    if(!new_file) {
        cout<<"No such file";
    }
    else {
        while (getline(new_file, line)) {
            str.push_back(line);
        }
        new_file.close();
    }
    return str;
}
vector<string> split (string str, string delimiter) {  // Splitting the strings that in input file by delimeter
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> splitArray;

    while ((pos_end = str.find (delimiter, pos_start)) != string::npos) {
        token = str.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        splitArray.push_back (token);
    }
    splitArray.push_back (str.substr (pos_start));
    return splitArray;
}
bool ifExist(string search,vector<string> key){  // Checks whether the string taken as a parameter exists in the vector taken as a parameter.
    for(string str:key){
        if(str==search){
            return 1;
        }
    }
    return 0;
}
void print_stack(stack<string> Stack,fstream &outputFile) // Prints Stack
{
    stack<string> temp;
    while (!Stack.empty())
    {
        temp.push(Stack.top());
        Stack.pop();
    }
    int counter;
    int size=temp.size();
    while (!temp.empty())
    {   counter++;
        outputFile<<temp.top();
        if(counter==size){
            break;}
        outputFile<<",";
        Stack.push(temp.top());
        temp.pop();
    }
}
void print_output(string starting_state,string input_symbol, string pop_symbol, string next_state, string push ,fstream &outputFile){  // Simplified function for printing to output
    outputFile<<starting_state<<","<<input_symbol<<","<<pop_symbol<<" => "<<next_state<<","<<push<<" [STACK]:";
}

int main(int argc, char *argv[]) {
    vector<string> dpda= readFile(argv[1]);   
    vector<string> input= readFile(argv[2]);   
    vector<string> states;
    vector<string> input_alphabet;
    vector<string> stack_alphabet;
    vector<string> command;
    vector<vector<string>> transitions;
    map<string,vector<string>> state_map;
    vector<string> initial_final;

    fstream output;
    output.open(argv[3],ios::out);
    if(!output)
    {cout<<"File creation failed";
    }
    int error=0;

    for(string str:dpda){

        if(str[0]=='Q'){
            states= split(split(str.substr(2)," => ")[0],",");
            initial_final=split(split(str.substr(2),"=> ")[1],",");
            vector<string> final;
            vector<string> initial;
            for (string state:initial_final){
                if(state[0]=='('){
                    state=state.substr(1,state.size()-2);
                    initial.push_back(state);
                    state_map.insert(pair<string,vector<string>>("initial",initial));
                }
                else if(state[0]=='['){
                    state=state.substr(1,state.size()-2);
                    final.push_back(state);
                }
            }
            state_map.insert(pair<string,vector<string>>("final",final));
        }

        else if(str[0]=='A'){
            input_alphabet= split(str.substr(2),",");
            input_alphabet.push_back("e");
        }
        else if(str[0]=='Z'){
            stack_alphabet= split(str.substr(2),",");
            stack_alphabet.push_back("e");


        }
        else if(str[0]=='T'){
            command= split(str.substr(2),",");
            string starting_state=command[0];
            string input_symbol=command[1];
            string next_state=command[3];
            string write=command[4];

            if((!ifExist(starting_state,states)) || (!ifExist(next_state,states)) || (!ifExist(input_symbol,input_alphabet)) || !ifExist(write,stack_alphabet)){
                output<<"Error [1]:DPDA description is invalid!"<<endl;
 	            error=1;
                output.close();
                break;
            }
            transitions.push_back(command);
        }
    }
    for(string line:input){
        vector<string> symbols_list= split(line,",");
        for(string symbol:symbols_list){
            if(symbol!=""){
                if(!ifExist(symbol,input_alphabet)){
                    output<<" Error [1]:DPDA description is invalid!"<<endl;
	                error=1;
                    output.close();
                    break;
                }
            }
        }
    }

    string inital=state_map.at("initial")[0];
    if(input.size()==0){
        if(ifExist(inital,state_map.at("final"))){
            output<<"ACCEPT"<<endl;
        }
        else{
            output<<"REJECT"<<endl;
        }
    }


    for(string line:input){

        if(line.length()==0){
            if(ifExist(inital,state_map.at("final"))){
                output<<"ACCEPT\n"<<endl;
                continue;
            }
            else{
                output<<"REJECT\n"<<endl;
                continue;
            }
        }
        stack<string> Stack;
        vector<string> symbols= split(line,",");
        string curr_symbol=symbols[0];
        string curr_state=inital;
        int counter=0;
        bool key=true;
        int key2=0;
        int key3=2;
        while(key && !error){

            for(vector<string> transition: transitions){

                string start_state=transition[0];
                string input_symbol=transition[1];
                string pop_symbol=transition[2];
                string next_state=transition[3];
                string push_symbol=transition[4];

                if((start_state==curr_state) && (input_symbol==curr_symbol || input_symbol=="e") && ((!Stack.empty() && (pop_symbol==Stack.top())) || (pop_symbol=="e"))){

                    if(!Stack.empty() && (pop_symbol==Stack.top())){
                        Stack.pop();
                    }
                    if(push_symbol!="e"){
                        Stack.push(push_symbol);
                    }

                    print_output(start_state,input_symbol,pop_symbol,next_state,push_symbol,output);
                    print_stack(Stack,output);
                    output<<endl;
                    key2++;
                    curr_state=next_state;

                    if(input_symbol==curr_symbol){
                        counter++;
                        if(counter<symbols.size()){
                            curr_symbol=symbols[counter];
                        }
                        if(counter>=symbols.size()){
                            curr_symbol="e";
                            key3--;
                        }
                    }break;
                }
                if((counter>=symbols.size())&&(key3==0)){
                    if(input_symbol=="e" && (pop_symbol=="e" || pop_symbol=="$")){
                        if(ifExist(curr_state,state_map.at("final"))) {
                            if ((Stack.empty() || (Stack.size()==1 && Stack.top()=="$"))){
                                output << "ACCEPT\n" << endl;
                                key = false;
                                break;
                            }
                        }
                        else{
                            output << "REJECT\n" << endl;
                            key = false;
                            break;
                        }
                    }
                }
                if(counter<symbols.size() && key2!=1){
                    if(input_symbol=="e" && (pop_symbol=="e" || pop_symbol=="$")){
                        if(ifExist(curr_state,state_map.at("final"))){
                            output << "REJECT\n" << endl;
                            key = false;
                            break;
                        }
                    }
                }
            }
        }
    }
    output.close();
    return 0;
}