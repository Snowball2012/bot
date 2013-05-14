#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "id_tables.h"

Parser::Parser():file_ended(false), cur_lex(NULL), prog(), stack()
{
}

void Parser::Analyse()
{
	GetLex();
	Prog();
	prog.PushBack(new PolizEnd());
}

void Parser::Interpret()
{
	Analyse();
	PolizItem * cur_cmd = prog.GetHead();
	while(cur_cmd) {
		(cur_cmd->elem)->Evaluate(&stack,&cur_cmd);
	}
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
			{
				GetLex();
				ExpCmp();
				Label *	else_label = new Label();
				prog.PushBack(new PolizLabel(else_label));
				prog.PushBack(new PolizIf());
				CheckLex(Lex::lex_then, "_then_ expected");
				GetLex();
				Oper();
				prog.PutLabel(else_label);
			}
			break;
		case Lex::lex_while : 
			{
				GetLex();
				Label * while_label = new Label();
				lab_t.Add(while_label);
				prog.PutLabel(while_label);
				ExpCmp();
				Label * continue_l = new Label();
				lab_t.Add(continue_l);
				prog.PushBack(new PolizLabel(continue_l));
				prog.PushBack(new PolizIf());
				CheckLex(Lex::lex_do, "_do_ expected");
				GetLex();
				Oper();
				prog.PushBack(new PolizLabel(while_label));
				prog.PushBack(new PolizOpGo());
				prog.PutLabel(continue_l);
			}
			break;
		case Lex::lex_label :
			prog.PutLabel(lab_t.Get(lab_t.Find(*cur_lex)));
			GetLex();
			CheckLex(Lex::lex_colon, "_:_ expected");
			GetLex();
			Oper();
			break;
		case Lex::lex_endturn :
			GetLex();
			CheckLex(Lex::lex_semicolon, "_;_ expected");
			prog.PushBack(new PolizEndturn());
			GetLex();
			break;
		case Lex::lex_prod :
			GetLex();
			ExpCmp();
			CheckLex(Lex::lex_semicolon, "_;_ expected");
			prog.PushBack(new PolizProd());
			GetLex();
			break;
		case Lex::lex_build : 
			GetLex();
			ExpCmp();
			CheckLex(Lex::lex_semicolon, "_;_ expected");
			prog.PushBack(new PolizBuild());
			GetLex();
			break;
		case Lex::lex_buy :
			GetLex();
			ExpCmp();
			CheckLex(Lex::lex_comma, "second operand expected");
			GetLex();
			ExpCmp();
			CheckLex(Lex::lex_semicolon, "_;_ expected");
			prog.PushBack(new PolizBuy());
			GetLex();
			break;
		case Lex::lex_sell : 
			GetLex();
			ExpCmp();
			CheckLex(Lex::lex_comma, "second operand expected");
			GetLex();
			ExpCmp();
			CheckLex(Lex::lex_semicolon, "_;_ expected");
			prog.PushBack(new PolizSell());
			GetLex();
			break;
		case Lex::lex_var :
			Var();
			CheckLex(Lex::lex_assign, 
				"Only assignment allowed in operator, started with variable");
			GetLex();
			ExpCmp();
			CheckLex(Lex::lex_semicolon, "_;_ expected");
			prog.PushBack(new PolizAssign());
			GetLex();
			break;
		case Lex::lex_print :
			GetLex();
			prog.PushBack(new PolizArgEnd());
			PrintList();
			CheckLex(Lex::lex_semicolon, "_;_ expected");
			prog.PushBack(new PolizPrint());
			GetLex();
			break;
		case Lex::lex_goto :
			GetLex();
			CheckLex(Lex::lex_label, "no label after _goto_");
			prog.PushBack(new PolizLabel(lab_t.Get(lab_t.Find(*cur_lex))));
			prog.PushBack(new PolizOpGo());
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
		PolizVar * var = new PolizVar(var_t.Get(var_t.Find(*cur_lex)));
		GetLex();
		EofCheck();
		prog.PushBack(var);
		if(cur_lex->GetType() == Lex::lex_lsbrace) {
			GetLex();
			ExpCmp();
			CheckLex(Lex::lex_rsbrace, "_]_ expected");
			prog.PushBack(new PolizIndex());
			GetLex();
		}
	} else {
		throw SyntErr(cur_lex, "Variable expected");
	}	
}

void Parser::ExpCmp()
{
	ExpAdd();
	EofCheck();
	PolizElem * comp;
	Lex::Type ltxt = cur_lex->GetType();
	if(ltxt!=Lex::lex_eq && ltxt!=Lex::lex_lssr && ltxt!=Lex::lex_grtr 
		&& ltxt!=Lex::lex_leq && ltxt!=Lex::lex_geq)
		return;
	switch (cur_lex->GetType()) {
		case Lex::lex_eq :
			comp = new PolizFunEq();
			break;
		case Lex::lex_lssr :
			comp = new PolizFunLssr();
			break;
		case Lex::lex_grtr :
			comp = new PolizFunGrtr();
			break;
		case Lex::lex_leq :
			comp = new PolizFunLeq();
			break;
		case Lex::lex_geq : 
			comp = new PolizFunGeq();
			break;
		default:
			break;
	}
	GetLex();
	ExpAdd();
	prog.PushBack(comp);
}

void Parser::ExpAdd()
{
	ExpMul();
	EofCheck();
	while(cur_lex->GetType() == Lex::lex_plus || cur_lex->GetType() == Lex::lex_minus
		|| cur_lex->GetType() == Lex::lex_or) 
	{
		PolizElem * add;
		switch(cur_lex->GetType()) {
			case Lex::lex_plus:
				add = new PolizFunPlus();
				break;
			case Lex::lex_minus:
				add = new PolizFunMinus();
				break;
			case Lex::lex_or:
				add = new PolizFunOr();
				break;
			default:
				SyntErr(cur_lex,"unknown error");
		}
		GetLex();
		ExpMul();
		EofCheck();
		prog.PushBack(add);
	}
}

void Parser::ExpMul()
{
	Opnd();
	EofCheck();
	while(cur_lex->GetType() == Lex::lex_times || cur_lex->GetType() == Lex::lex_slash
		|| cur_lex->GetType() == Lex::lex_and) {
		PolizElem * mul;
		switch(cur_lex->GetType()) {
			case Lex::lex_times: mul = new PolizFunMul(); break;
			case Lex::lex_slash: mul = new PolizFunDiv(); break;
			case Lex::lex_and: mul = new PolizFunAnd(); break;
			default: break;
		}
		GetLex();
		Opnd();
		EofCheck();
		prog.PushBack(mul);
	}
}

void Parser::Opnd()
{
	EofCheck();
	switch(cur_lex->GetType()) {
		case Lex::lex_num :
			prog.PushBack(new PolizInt(cur_lex->GetText()));
			GetLex();
			break;
		case Lex::lex_minus :
			GetLex();
			Opnd();
			prog.PushBack(new PolizFunMinusAlone());
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
	prog.PushBack(new PolizArgEnd());
	CheckLex(Lex::lex_func, "function name expected");
	PolizElem * func_p = CreateFunc(cur_lex);
	if(!func_p)
		throw SyntErr(cur_lex, "No such function");
	GetLex();
	EofCheck();
	if (cur_lex->GetType() == Lex::lex_lrbrace) {
		GetLex();
		Args();
		CheckLex(Lex::lex_rrbrace, "_)_ expected");
		GetLex();
	}
	prog.PushBack(func_p);
}

PolizElem * Parser::CreateFunc(Lex * lex)
{
	PolizElem * elem = NULL;
	const char * linf = lex->GetText();
//big if ... else ... statement	
	if (!strcmp(linf, "?my_id")) elem = new PolizId();
	else if (!strcmp(linf, "?turn")) elem = new PolizTurn();
	else if (!strcmp(linf, "?players")) elem = new PolizPlayers();
	else if (!strcmp(linf, "?active_players")) elem = new PolizActPlayers();
	else if (!strcmp(linf, "?supply")) elem = new PolizSupply();
	else if (!strcmp(linf, "?raw_price")) elem = new PolizRawPrice();
	else if (!strcmp(linf, "?demand")) elem = new PolizDemand();
	else if (!strcmp(linf, "?production_price")) elem = new PolizProdPrice();
	else if (!strcmp(linf, "?money")) elem = new PolizMoney();
	else if (!strcmp(linf, "?raw")) elem = new PolizRaw();
	else if (!strcmp(linf, "?production")) elem = new PolizProduction();
	else if (!strcmp(linf, "?factories")) elem = new PolizFacts();
	else if (!strcmp(linf, "?auto_factories")) elem = new PolizAutoFacts();
	else if (!strcmp(linf, "?manufactured")) elem = new PolizManuf();
	else if (!strcmp(linf, "?result_raw_sold")) elem = new PolizRawSold();
	else if (!strcmp(linf, "?result_raw_price")) elem = new PolizResRawPrice();
	else if (!strcmp(linf, "?result_prod_bought")) elem = new PolizProdBought();
	else if (!strcmp(linf, "?result_prod_price")) elem = new PolizResProdPrice();
	return elem;
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
		prog.PushBack(new PolizString(cur_lex->GetText()));
		GetLex();
	} else {
		ExpCmp();
	}
}
