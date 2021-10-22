#include <iostream>
#include <map>
#include <fstream>
#include <list>
#include <algorithm>
#include <sstream>
#include "lex.h"


/*
 * CS280 - Fall 2020
 */

#include <cctype>
#include <map>
using std::map;

using namespace std;

int main(int argc, char *argv[])
{
	LexItem	tok;
	int linenumber =0;
	ifstream  file ;

	
	map<string, bool> arguments {
		{"-v", false},
		{"-iconst", false},
		{"-fconst", false},
		{"-strconst", false},
		{"-ident", false},
	};
	
	bool multipleFiles = false;

	//int i;
	
	if (argc == 1)
	{
		cerr << "NO SPECIFIED INPUT FILE NAME." << endl;
		return 0;
	}
	for( int i=1; i<argc; i++ ) {
        string arg(argv[i]);
		if(arguments.find(arg) != arguments.end()){
			arguments[arg] = true;
            continue;
		}
		else if(arg[0] == '-'){
			cerr << "UNRECOGNIZED FLAG " << arg << endl;
			return 0;
		}
		if(multipleFiles){
			cerr << "ONLY ONE FILE NAME ALLOWED" << endl;
			return 0;
		}
		file.open(arg);
		multipleFiles = true;
		if(file.is_open() == false) {
			cerr << "CANNOT OPEN the File " << arg << endl;
			return 0;
		}
	}
	//file.open("allflags");

	map<int, Token> intconsts = {};
	map<float, Token> rconsts = {};
	map<string, Token> sconsts = {};
	map<string, Token> identconsts = {};
	list<LexItem> all = {}; 

	int tokens =0;

	while((tok = getNextToken(file, linenumber)) != DONE && tok != ERR){
		if(tok == ICONST){
			int i;
			istringstream (tok.GetLexeme()) >> i;
			intconsts[i] = ICONST;
		}
		if(tok == RCONST){
			float j;
			istringstream (tok.GetLexeme()) >> j;
			rconsts[j] = RCONST;
		}
		if(tok == SCONST) {
			sconsts[tok.GetLexeme()] = SCONST;
		}
		if(tok == IDENT) {
			string s = tok.GetLexeme();
			transform(s.begin(), s.end(), s.begin(), ::toupper);
			identconsts[s] = IDENT;
		}
		tokens ++;
		all.push_back(tok);
		if(arguments["-v"]){
				cout<<tok<<endl;
			}
	}

  if(tok == ERR){
		cout<<"Error in line "<< tok.GetLinenum() + 1 <<" ("<<tok.GetLexeme()<<")"<<endl;
	}
  else{
			cout<<"Lines: "<<linenumber<<endl;
      if(linenumber!=0)
        cout<<"Tokens: "<<tokens<<endl;
      
      if(arguments["-strconst"]){
				cout<<"STRINGS:"<<endl;
				for (auto it = sconsts.cbegin(); it != sconsts.cend(); ++it) {
        	cout << "\""<<(*it).first <<"\""<<endl;
				}
    	}
      
			if(arguments["-iconst"]){
				cout<<"INTEGERS:"<<endl;
				for (auto it = intconsts.cbegin(); it != intconsts.cend(); ++it) {
        	cout << (*it).first <<endl;
				}
    	}
			if(arguments["-fconst"]){
				cout<<"REALS:"<<endl;
				for (auto it = rconsts.cbegin(); it != rconsts.cend(); ++it) {
        	cout << (*it).first <<endl;
				}
    	}
			
			if(arguments["-ident"]){
				cout<<"IDENTIFIERS:"<<endl;
				for (auto it = identconsts.cbegin(); it != identconsts.cend(); ) {
					cout<<(*it).first ;
					if(++it == identconsts.cend()){
						cout<<endl;
					}
					else {
						cout<<", ";
					}
				}
    	}
	}
}
