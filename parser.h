#ifndef PARSER_H
#define PARSER_H

#include "lex.h"
#include "poliz.h"
#include "id_tables.h"

class Parser
{
	bool file_ended;
	Scanner lex_scan;
	IdTable<Variable> var_t;
	IdTable<Label> lab_t;
	Lex * cur_lex;
	PolizStack prog;
	PolizStack stack;
	void GetLex();
	void Prog();
	void Oper();
	void OpSet();
	void Var();
	void ExpCmp();
	void ExpAdd();
	void ExpMul();
	void Opnd();
	void Func();
	void Args();
	void PrintList();
	void PrintElem();
	PolizElem * CreateFunc(Lex * lex);
	void CheckLex(Lex::Type type, const char * errmsg);
	inline void EofCheck();
public:
	void Analyse();
	void Interpret();
	Parser();
};

class SyntErr
{
	Lex * err_lexem;
	const char * message;
public:
	SyntErr(Lex * lex, const char * msg):err_lexem(lex), message(msg) {}
	int GetLine() { return err_lexem?err_lexem->GetLine():0; }
	const char * GetMsg() {return message;}
	const char * GetLexText() {return err_lexem?err_lexem->GetText():"no_lex";}
};

#endif
