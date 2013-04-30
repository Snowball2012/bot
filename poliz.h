/* 	Made by Snowball 29.04.2013
 *	Poliz class tree */
#ifndef POLIZ_H
#define POLIZ_H

#include "lex.h"

class PolizElem
{
};

class PolizConst:public PolizElem
{
};

class PolizFunction:public PolizElem
{
};

class PolizOpGo:public PolizElem
{
};

//PolizConst children

class PolizInt:public PolizConst
{
};

class PolizString:public PolizConst
{
};

class PolizLabel:public PolizConst
{
};

//Special const for print ending
class PolizPrintEnd:public PolizConst
{
};

//PolizFunction children

class PolizVar:public PolizFunction
{
};

class PolizSell:public PolizFunction
{
};

class PolizBuy:public PolizFunction
{
};

class PolizProd:public PolizFunction
{
};

class PolizBuild:public PolizFunction
{
};

class PolizUpgrade:public PolizFunction
{
};

class PolizEndturn:public PolizFunction
{
};

class PolizFunPlus:public PolizFunction
{
};

class PolizFunMinus:public PolizFunction
{
};

class PolizFunMinusAlone:public PolizFunction
{
};

class PolizFunMul:public PolizFunction
{
};

class PolizFunDiv:public PolizFunction
{
};

class PolizFunOr:public PolizFunction
{
};

class PolizFunAnd:public PolizFunction
{
};

class PolizFunEq: public PolizFunction 
{
};

class PolizFunLssr: public PolizFunction 
{
};

class PolizFunGrtr: public PolizFunction 
{
};

class PolizFunLeq: public PolizFunction 
{
};

class PolizFunGeq: public PolizFunction 
{
};

class PolizIf:public PolizFunction
{
};

class PolizAssign:public PolizFunction
{
};

class PolizPrint:public PolizFunction
{
};

class PolizIndex:public PolizFunction
{
};

class PolizTurn:public PolizFunction
{
};

class PolizRaw:public PolizFunction
{
};

class PolizId:public PolizFunction
{
};

class PolizPlayers:public PolizFunction
{
};

class PolizActPlayers:public PolizFunction
{
};

class PolizSupply:public PolizFunction
{
};

class PolizRawPrice:public PolizFunction
{
};

class PolizDemand:public PolizFunction
{
};

class PolizProdPrice:public PolizFunction
{
};

class PolizMoney:public PolizFunction
{
};

class PolizFacts:public PolizFunction
{
};

class PolizAutoFacts:public PolizFunction
{
};

class PolizManuf:public PolizFunction
{
};

class PolizResRawSold:public PolizFunction
{
};

class PolizProdBought:public PolizFunction
{
};

//Poliz list/stack class

class PolizIdTable;

class PolizStack
{
public:
	struct PolizItem 
	{
		PolizElem * elem;
		PolizItem * next;
	};
private:
	PolizItem * head;
	PolizIdTable * idents;
	struct LabelQueue
	{
		PolizLabel * label;
		LabelQueue * next;
	};
	LabelQueue * queue;
public:
	PolizStack(PolizIdTable * aidents);
	~PolizStack();
	PolizElem * Pop();
	void Push(PolizElem * new_elem);
	void PushBack(PolizElem * new_elem);
	const PolizItem * GetHead();
	void PutLabel(Lex label); // makes label to point to next poliz elem
};

class PolizIdTable
{
	struct ltabl_item 
	{
		//TODO
	}
	char * iflabel_count;     //ATTENTION:though it's pointer, it's used like integer
	PolizElem * CreateRealLabel(const char * ltext, Lex::Type lex_type);
	PolizElem * CreateVar(const char * ltext);
	PolizElem * CreateFunc(const char * ltext);
	PolizStack::PolizItem *& FindLabel(const char *,Lex::Type);
public:
	PolizElem * CreateElem(Lex lexem);
	Lex CreateLabel(); /*Creates original label lexem for _if_ and _while_
						Honestly, it's not a label actually, but it works like one
						ATTENTION: DO NOT PRINT THIS LEXEM!*/
	void InitLabel(PolizLabel * label, PolizStack::PolizItem * point); // defines a label
	PolizIdTable():iflabel_count(0){}
};

//Error classes

class NotLabelEx
{
	Lex lexem;
public:	
	NotLabelEx(Lex lex):lexem(Lex::lex_null, NULL, 0)
	{
		lexem = lex;
	}
};

#endif


















































