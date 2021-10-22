#include "lex.h"
#include <sstream>
#include <string>
#include <algorithm>

static map<Token,string> tokenPrint = {
		{PROGRAM, "PROGRAM"},
		{WRITE, "WRITE"},
		{INT, "INT"},
		{ END, "END" },
		{ IF, "IF" },
		{FLOAT, "FLOAT"},
		{STRING, "STRING"},
		{ REPEAT, "REPEAT" },
		
		
		{BEGIN, "BEGIN"},

		{ IDENT, "IDENT" },

		{ ICONST, "ICONST" },
		{ RCONST, "RCONST" },
		{ SCONST, "SCONST" },
		
		{ PLUS, "PLUS" },
		{ MINUS, "MINUS" },
		{ MULT, "MULT" },
		{ DIV, "DIV" },
		{REM, "REM"},
		{ ASSOP, "ASSOP" },
		{ LPAREN, "LPAREN" },
		{ RPAREN, "RPAREN" },
            
		{ COMMA, "COMMA" },
		{ EQUAL, "EQUAL" },
		{ GTHAN, "GTHAN" },
		
		{ SEMICOL, "SEMICOL" },
		
		{ ERR, "ERR" },

		{ DONE, "DONE" },
};

//Keywords mapping
static map<string,Token> kwmap = {
		{"PROGRAM", PROGRAM},
		{"WRITE", WRITE},
		{ "INT", INT},
		{ "FLOAT", FLOAT},
		{ "STRING", STRING},
		{ "REPEAT", REPEAT },
		{ "BEGIN", BEGIN },
		{"IF", IF},
		{ "END", END },
};

LexItem getNextToken(istream& in, int& linenumber){
	enum TokState{
		START,INID,INSTRING,ININT,INREAL,INCOMMENT,VALIDREAL
	} state = START;
	string lexeme;
	char ch;
	while(in.get(ch)){
		//cout<<"reading- "<<ch<<" line " << linenumber<<endl;
		switch(state){
			case START:
				if(ch =='\n'){
					linenumber++;
					continue;
				}
				else if(isspace(ch)){
					continue;
				}
				else if(ch == '#'){
					state = INCOMMENT;
					continue;
				}

				lexeme += ch;
				if(isalpha(ch)){
					state = INID;
				}
				else if(isdigit(ch)){
					state = ININT;
				}
				else if(ch == '\"'){
					state = INSTRING;
				}
				else if(ch == '.'){
					state = INREAL;
				}
				else{
					switch(ch){
						case '+': return LexItem(PLUS, lexeme, linenumber);
						case '-': return LexItem(MINUS, lexeme, linenumber);
						case '*': return LexItem(MULT, lexeme, linenumber);
						case '/': return LexItem(DIV, lexeme, linenumber);
						case '%': return LexItem(REM, lexeme, linenumber);
						case '=':
							if(in.peek() == '='){
								lexeme += in.get();
								return LexItem(EQUAL, lexeme, linenumber);
							} else{
								return LexItem(ASSOP, lexeme, linenumber);
							}
						case '(': return LexItem(LPAREN, lexeme, linenumber);
						case ')': return LexItem(RPAREN, lexeme, linenumber);
						case ',': return LexItem(COMMA, lexeme, linenumber);
						case '>': return LexItem(GTHAN, lexeme, linenumber);
						case ';': return LexItem(SEMICOL, lexeme, linenumber);
						
						default: return LexItem(ERR, lexeme, linenumber);
					}
				}
				break;

			case INID:
				if(isalnum(ch) || ch == '_'){
					lexeme += ch;
					continue;
				}	else {
					if(ch == '\n')
						linenumber++;
					else if(!isspace(ch))
						in.putback(ch);
					return id_or_kw(lexeme, linenumber);
				}
				break;

			case ININT:
				if(isdigit(ch)){
					lexeme += ch;
					continue;
				}
				else if (ch == '.'){
					lexeme += ch;
					state= INREAL;
					continue;
				} else if(isalpha(ch)){
					return LexItem(ERR, lexeme, linenumber);
				}
				else {
					//lexeme += ch;
					in.putback(ch);
					return LexItem(ICONST, lexeme, linenumber);
				}
				break;

			case INREAL:
				if(isdigit(ch)) {
					lexeme += ch;
					state = VALIDREAL;
				}
				else {
					return LexItem(ERR, lexeme, linenumber);
				}
				break;

			case VALIDREAL:
				if(isdigit(ch)){
					lexeme += ch;
					continue;
				}
				else {
					in.putback(ch);
					return LexItem(RCONST, lexeme, linenumber);
				}
				break;
			
			case INSTRING:
				if(ch == EOF || ch == '\n'){
					return LexItem(ERR, lexeme, linenumber);
				} 
				else if (ch == '\"'){
					// lexeme += ch;
					return LexItem(SCONST, lexeme.substr(1), linenumber);
				}
				else {
					lexeme += ch;
					continue;
				}
				break;

			case INCOMMENT:
				if(ch == '\n'){
					state = START;
					linenumber++;
				}
				else{
					continue;
				}	
				break;
		}
	}
	// if(ch == EOF)
	return LexItem(DONE, lexeme, linenumber);
	
}

LexItem id_or_kw(const string& lexeme, int linenum){
	string s = lexeme;
	transform(s.begin(), s.end(), s.begin(), ::toupper);
	if(kwmap.find(s) == kwmap.end()){
		LexItem	result(IDENT, lexeme, linenum);
		return result;
	}

	LexItem result(kwmap[s], lexeme, linenum);
	return result;
	
}

ostream& operator<< (ostream& out,const LexItem& tok){
	Token t = tok.GetToken();
	out<<tokenPrint[t];
	if(t == IDENT || t== ICONST || t == RCONST || t == SCONST){
		string s= tok.GetLexeme();
        if(t != SCONST)
		    transform(s.begin(), s.end(), s.begin(), ::toupper);
		out<<"("<<s<<")";
	}
	//out<<tok.GetLinenum();
	return out;
}