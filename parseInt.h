/*
 * parse.h
*/

#ifndef PARSE_H_
#define PARSE_H_

#include <iostream>

using namespace std;

#include "lex.h"
#include "val.h"


extern bool Prog(istream& in, int& line);
extern bool StmtList(istream& in, int& line);
extern bool Stmt(istream& in, int& line);
extern bool ControlStmt(istream& in, int& line);
extern bool DeclStmt(istream& in, int& line);
extern bool WriteStmt(istream& in, int& line);
extern bool IfStmt(istream& in, int& line);
extern bool IdentList(istream& in, int& line, LexItem tok);
extern bool Var(istream& in, int& line, LexItem& tok);
extern bool AssignStmt(istream& in, int& line);
extern bool ExprList(istream& in, int& line);
extern bool LogicExpr(istream& in, int& line, Value& retVal);
extern bool Expr(istream& in, int& line, Value& retVal);
extern bool Term(istream& in, int& line, Value& retVal);
extern bool SFactor(istream& in, int& line, Value& retVal);
extern bool Factor(istream& in, int& line, int sign, Value& retVal);
extern int ErrCount();

#endif /* PARSE_H_ */
