/* Implementation of Recursive-Descent Parser
 * parse.cpp
*/

#include "parse.h"

map<string, bool> defVar;
map<string, Token> SymTable;

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			// cout<<"aborting";
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}


//Decl = Type IdentList 
//Type = INTEGER | REAL | CHAR
bool DeclStmt(istream& in, int& line) {
	bool status = false;
	LexItem tok;
	//cout << "in Decl" << endl;
	LexItem t = Parser::GetNextToken(in, line);
	
	if(t == INT || t == FLOAT ) {
		status = IdentList(in, line, t);
		//cout<< "returning from IdentList" << " " << (status? 1: 0) << endl;
		if (!status)
		{
			ParseError(line, "Incorrect variable in Declaration Statement.");
			return status;
		}
		}
	else{
		Parser::PushBackToken(t);
		ParseError(line, "Incorrect Type.");
		return false;
	}
	return true;
}

bool Stmt(istream& in, int& line){
	bool status=true;
	//cout << "in Stmt" << endl;
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {
	case INT: case FLOAT: 
		Parser::PushBackToken(t);
		status = DeclStmt(in, line);
		if(!status)
		{
			ParseError(line, "Incorrect Declaration Statement.");
			return status;
		}
		break;
	case IF: case WRITE: case IDENT: 
		Parser::PushBackToken(t);
		status = ControlStmt(in, line);
		if(!status)
		{
			ParseError(line, "Incorrect control Statement.");
			return status;
		}
		break;
	default:
		Parser::PushBackToken(t);
	}
	return status;
}

//WriteStmt:= wi, ExpreList 
bool WriteStmt(istream& in, int& line) {
	//LexItem t;
	//cout << "in WriteStmt" << endl;
		
	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing expression after Write");
		return false;
	}
	
	//Evaluate: print out the list of expressions values

	return ex;
}

bool IdentList(istream& in, int& line, LexItem tok) {
	LexItem t = Parser::GetNextToken(in, line);
	if(t == IDENT){
		if(defVar.find(t.GetLexeme()) != defVar.end()){
			ParseError(line, "Variable Redefinition");
			return false;
		}
		defVar[t.GetLexeme()] = true;
		SymTable[t.GetLexeme()] = tok.GetToken();
		t = Parser::GetNextToken(in, line);
		while(t == COMMA){
			t = Parser::GetNextToken(in, line);
			if(t == IDENT){
				if(defVar.find(t.GetLexeme()) != defVar.end()){
					ParseError(line, "Variable Redefinition");
					return false;
				}
				defVar[t.GetLexeme()] = true;
				SymTable[t.GetLexeme()] = tok.GetToken();
				t = Parser::GetNextToken(in, line);
			} else{
				Parser:: PushBackToken(t);
				return false;
			}
		}
		Parser::PushBackToken(t);
		return true;
		
	} else{
		Parser::PushBackToken(t);
		// ParseError(line, "Incorrect Identifiers");
		return false;
	}
}

bool ExprList(istream& in, int& line){
	bool exp = Expr(in,line);
	if(exp){
		LexItem t = Parser::GetNextToken(in, line);
		while(t == COMMA){
			exp = Expr(in, line);
			if(exp){
				t = Parser::GetNextToken(in,line);
			}
			else{
				ParseError(line, "Expression needed after cooma");
				return false;
			}
		}
		Parser::PushBackToken(t);
		return true;
	}
	else {
		ParseError(line, "Invalid expression");
		return false;
	}
}

bool Expr(istream& in, int& line){
	bool term = Term(in,line);
	if(term){
		LexItem t = Parser::GetNextToken(in,line);
		while(t == PLUS || t == MINUS){
			term = Term(in,line);
			if(term){
				t = Parser::GetNextToken(in,line);
			}
			else{
				ParseError(line, "Term needed after plus/minus operator");
				return false;
			}
		}
		Parser::PushBackToken(t);
		return true;
	}
	else {
		ParseError(line, "Invalid term");
		return false;
	}
}

bool Term(istream& in, int& line){
	bool sfact = SFactor(in,line);
	if(sfact){
		LexItem t = Parser::GetNextToken(in,line);
		while(t == MULT || t == DIV || t == REM){
			sfact = SFactor(in,line);
			if(sfact){
				t = Parser::GetNextToken(in,line);
			}
			else {
				ParseError(line, "SFactor needed after mult/div/rem operator");
				return false;
			}
		}
		Parser::PushBackToken(t);
	} 
	return sfact;
}

bool SFactor(istream& in, int& line){
	LexItem t = Parser::GetNextToken(in,line);
	if(t == PLUS|| t == MINUS){
		int sign;
		if(t == PLUS)
			sign = 1;
		else 
			sign = -1;
		
		bool fact= Factor(in, line, sign);
		if(fact){
			return true;
		}
		else{
			ParseError(line,"Incomplete factor after plus/minus");
			return false;
		}
	}
	else {
		Parser::PushBackToken(t);
		bool fact = Factor(in,line, 0);
		if(fact) {
			return true;
		}
		else{
			ParseError(line, "Incorrect Sfactor");
			return false;
		}
	}
	
}

bool Factor(istream& in, int& line, int sign){
	bool status = false;
	LexItem t = Parser::GetNextToken(in,line);
	switch(t.GetToken()){
		case IDENT : case ICONST : case RCONST : case SCONST:
			return true;

		case LPAREN:
			status = Expr(in,line);
			if(!status){
				ParseError(line, "Invalid expression");
				return status;
			}
			t = Parser::GetNextToken(in, line);
			if(t == RPAREN){
				return status;
			}
			else{
				ParseError(line, "Missing Right Parenthesis");
				return false;
			}
			break;

		default:
			Parser::PushBackToken(t);
	}
	return status;
}


bool LogicExpr(istream& in, int& line){
	bool expr = Expr(in,line);
	if(expr){
		LexItem t = Parser::GetNextToken(in,line);
		if(t == GTHAN || t == EQUAL){
			expr = Expr(in,line);
			if(expr){
				return true;
			}
			else{
				return false;
			}
		}
		else {
			ParseError(line, "Invalid logical expression");
			return false;
		}
	}
	else{
		ParseError(line,"Expression needed");
		return false;
	}
}

bool AssignStmt(istream& in, int& line){
	bool curr = Var(in,line);

	if(curr){
		LexItem t = Parser::GetNextToken(in,line);
		if(t == ASSOP){
			 curr = Expr(in,line);
			 return curr;
		}
	}
	//ParseError(line, "Invalid assignment");
	return false;
}

bool Var(istream& in, int& line){
	LexItem t = Parser::GetNextToken(in,line);
	if(t == IDENT){
		return true;
	}
	else {
		Parser::PushBackToken(t);
		return false;
	}
}

bool IfStmt(istream& in, int& line){
	LexItem t = Parser::GetNextToken(in,line);
	if(t == LPAREN){
		bool logicexp = LogicExpr(in,line);
		// cout<<"logic done";
		if(!logicexp){
			ParseError(line,"Incorrect Logical Expression inside IF");
			return false;
		}
		t = Parser::GetNextToken(in,line);
		// cout<<t;
		if(t == RPAREN){
			bool ctrl = ControlStmt(in,line);
			if(!ctrl){
				ParseError(line,"Missing control statement");
			}
			return ctrl;
		} else{
			ParseError(line,"Missing Right Parenthesis for IF");
			Parser::PushBackToken(t);
			return false;
		}
	}else{
		ParseError(line, "Missing Left Parenthesis for IF");
		Parser::PushBackToken(t);
		return false;
	}
}


bool ControlStmt(istream& in, int& line){
	bool status=true;
	//cout << "in Stmt" << endl;
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {
	case IF:  
		//Parser::PushBackToken(t);
		status = IfStmt(in, line);
		if(!status)
		{
			ParseError(line, "Incorrect if Statement.");
			return status;
		}
		break;
	case WRITE: 
		//Parser::PushBackToken(t);
		status = WriteStmt(in, line);
		if(!status)
		{
			ParseError(line, "Incorrect write Statement.");
			return status;
		}
		break;
	case IDENT: 
		Parser::PushBackToken(t);
		status = AssignStmt(in,line);
		if(!status){
			ParseError(line,"Incorrect assignment statement");
			return status;
		}
		break;
	default:
		Parser::PushBackToken(t);
	}
	return status;
}

bool StmtList(istream& in, int& line){
	LexItem t = Parser::GetNextToken(in,line);
    
	// cout<<t;
	if(t == END){
		Parser::PushBackToken(t);
		return true;
	}
	Parser::PushBackToken(t);
	bool stmt = Stmt(in,line);
	if(stmt){
		LexItem next = Parser::GetNextToken(in,line);
		if(next == SEMICOL){
			// cout<<"1 statment done"<<endl;
			stmt = StmtList(in,line);
			// cout<<stmt;
			return stmt;
		}
		else {
			Parser::PushBackToken(next);
			// cout<<"m"<<endl;
			ParseError(line, "Missing a semicolon");
			return false;
		}
	}
	else {
		ParseError(line, "Missing a semicolon");
		return false;
	}
}

bool Prog(istream& in, int& line){
	bool status = true;
	LexItem t = Parser::GetNextToken(in,line);
    if(t== DONE){
        ParseError(line,"Empty File");
        return false;
    }
    
	if(t == PROGRAM){
		t = Parser::GetNextToken(in,line);
		if(t == IDENT){
			status = StmtList(in,line);
			// cout<<"statement list successfult"<<endl;
			if(!status){
				ParseError(line,"Incorrect Syntax in the Program");
				return status;
			}
			t = Parser::GetNextToken(in,line);
			if(t == END){
				t = Parser::GetNextToken(in,line);
				if(t == PROGRAM){
					return true;
				} 
				else {
					Parser::PushBackToken(t);
					ParseError(line,"Missing PROGRAM at the End");
					return false;
				}
			} else{
				Parser::PushBackToken(t);
				ParseError(line, "END statement missing");
				return false;
			}
		} else {
			Parser::PushBackToken(t);
			ParseError(line, "Missing Program Name.");
			return false;
		}
	} else{
		Parser::PushBackToken(t);
		ParseError(line,"Missing PROGRAM");
		return false;
	}
}