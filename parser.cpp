#include <stdio.h>
#include "parser.h"


void Parser::Analyse()
{
	GetLex();
	Prog();
}

void Parser::GetLex()
{
	if(cur_lex)
		delete cur_lex;
	cur_lex = NULL;
	int c;
	while(!cur_lex && !file_ended) {
		c = getchar();
		if (c == EOF) {
			file_ended = true;
			c = ' ';
		} 
		lex_scan.Step(c);
		cur_lex = lex_scan.GetLex();
	}
}

void Parser::CheckLex(Lex::Type type, const char * errmsg)
{
	if(cur_lex) {
		if(cur_lex->GetType() != type)
			throw SyntErr(cur_lex, errmsg);
	} else {
		throw SyntErr(NULL, errmsg);
	}
}

inline void Parser::EofCheck()
{
	if(!cur_lex)
		throw SyntErr(NULL, "EOF has been reached too soon");
}

void Parser::Prog()
{
	Oper();
	if(cur_lex !=  NULL)
		Prog();
}

void Parser::Oper()
{
	if(!cur_lex)
		throw SyntErr(NULL, "End of file reached too soon");
	switch (cur_lex->GetType()) {
		case Lex::lex_lfbrace :
			GetLex();
			OpSet();
			CheckLex(Lex::lex_rfbrace, "_}_ expected");
			GetLex();
			break;
		case Lex::lex_if :
			GetLex();
			ExpCmp();
			CheckLex(Lex::lex_then, "_then_ expected");
			GetLex();
			Oper();
			break;
		case Lex::lex_while :
			GetLex();
			ExpCmp();
			CheckLex(Lex::lex_do, "_do_ expected");
			GetLex();
			Oper();
			break;
		case Lex::lex_label :
			GetLex();
			CheckLex(Lex::lex_colon, "_:_ expected");
			GetLex();
			Oper();
			break;
		case Lex::lex_endturn :
			GetLex();
			CheckLex(Lex::lex_semicolon, "_;_ expected");a
			GetLex();
			break;
		case Lex::lex_prod :
		case Lex::lex_build :
			GetLex();
			ExpCmp();
			CheckLex(Lex::lex_semicolon, "_;_ expected");
			GetLex();
			break;
		case Lex::lex_buy :
		case Lex::lex_sell :
			GetLex();
			ExpCmp();
			CheckLex(Lex::lex_comma, "second operand expected");
			GetLex();
			ExpCmp();
			CheckLex(Lex::lex_semicolon, "_;_ expected");
			GetLex();
			break;
		case Lex::lex_var :
			Var();
			CheckLex(Lex::lex_assign, 
				"Only assignment allowed in operator, started with variable");
			GetLex();
			ExpCmp();
			CheckLex(Lex::lex_semicolon, "_;_ expected");

			GetLex();
			break;
		case Lex::lex_print :
			GetLex();
			PrintList();
			CheckLex(Lex::lex_semicolon, "_;_ expected");
			GetLex();
			break;
		case Lex::lex_goto :
			GetLex();
			CheckLex(Lex::lex_label, "no label after _goto_");
			GetLex();
			break;
		case Lex::lex_semicolon :
			GetLex();
			break;
		default:
			throw SyntErr(cur_lex, "operator expected");
	}
}

void Parser::OpSet()
{
	EofCheck();
	while (cur_lex->GetType()!=Lex::lex_rfbrace) {
		Oper();
		if(!cur_lex)
			throw SyntErr(NULL, "EOF reached too soon");
	}
}

void Parser::Var()
{
	EofCheck();
	if(cur_lex->GetType() == Lex::lex_var) {
		GetLex();
		EofCheck();
		if(cur_lex->GetType() == Lex::lex_lsbrace) {
			GetLex();
			ExpCmp();
			CheckLex(Lex::lex_rsbrace, "_]_ expected");
			GetLex();
		}
	} else {
		throw SyntErr(NULL, "Variable expected");
	}	
}

void Parser::ExpCmp()
{
	ExpAdd();
	EofCheck();
	switch (cur_lex->GetType()) {
		case Lex::lex_eq :
		case Lex::lex_lssr :
		case Lex::lex_grtr :
		case Lex::lex_leq :
		case Lex::lex_geq :
			GetLex();
			ExpAdd();
			break;
		default:
			break;
	}
}

void Parser::ExpAdd()
{
	ExpMul();
	EofCheck();
	while(cur_lex->GetType() == Lex::lex_plus || cur_lex->GetType() == Lex::lex_minus
		|| cur_lex->GetType() == Lex::lex_or) {
		ExpMul();
		EofCheck();
	}
}

void Parser::ExpMul()
{
	Opnd();
	EofCheck();
	while(cur_lex->GetType() == Lex::lex_times || cur_lex->GetType() == Lex::lex_slash
		|| cur_lex->GetType() == Lex::lex_and) {
		Opnd();
		EofCheck();
	}
}

void Parser::Opnd()
{
	EofCheck();
	switch(cur_lex->GetType()) {
		case Lex::lex_num :
			GetLex();
			break;
		case Lex::lex_minus :
			GetLex();
			Opnd();
			break;
		case Lex::lex_lrbrace :
			GetLex();
			ExpCmp();
			CheckLex(Lex::lex_rrbrace, "_)_ expected");
			GetLex();
			break;
		case Lex::lex_var :
			Var();
			break;
		case Lex::lex_func :
			Func();
			break;
		default:
			throw SyntErr(cur_lex, "Not an operand");
	}
}

void Parser::Func()
{
	CheckLex(Lex::lex_func, "function name expected");
	GetLex();
	EofCheck();
	if (cur_lex->GetType() == Lex::lex_lrbrace) {
		GetLex();
		Args();
		CheckLex(Lex::lex_rrbrace, "_)_ expected");
		GetLex();
	}
}

void Parser::Args()
{
	ExpCmp();
	EofCheck();
	while(cur_lex->GetType() == Lex::lex_comma) {
		GetLex();
		ExpCmp();
		EofCheck();
	}
}

void Parser::PrintList()
{
	PrintElem();
	EofCheck();
	while(cur_lex->GetType() == Lex::lex_comma) {
		GetLex();
		PrintElem();
		EofCheck();
	}
}

void Parser::PrintElem()
{
	EofCheck();
	if (cur_lex->GetType() == Lex::lex_str) {
		GetLex();
	} else {
		ExpCmp();
	}
}
























