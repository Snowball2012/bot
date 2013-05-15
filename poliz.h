/* 	Made by Snowball 29.04.2013
 *	Poliz class tree */
#ifndef POLIZ_H
#define POLIZ_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lex.h"
#include "id_tables.h"
#include "client.h"

class PolizElem;
//Poliz list/stack class
class PolizStack;

struct PolizItem 
{
	PolizElem * elem;
	PolizItem * next;
};


class PolizStack
{
public:

private:
	PolizItem * head;
	struct LabelQueue
	{
		Label * label;
		LabelQueue * next;
	};
	LabelQueue * queue;
public:
	PolizStack();
	~PolizStack();
	PolizElem * Pop();
	void Push(PolizElem * new_elem);
	void PushBack(PolizElem * new_elem);
	PolizItem * GetHead();
	void PutLabel(Label * label); // makes label to point to next poliz elem
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

//poliz exeptions

class PolizEx
{
	PolizElem * ex;
public:
	PolizEx(PolizElem * elem) 
	{
		ex = elem;
	}
	virtual void PrintErr() const
	{
		printf("poliz exeption has caught\n");
	}
};

class PolizNotInt
{
public:
	PolizNotInt(const PolizElem* elem) {}
};

class PolizNotLabel
{
public:
	PolizNotLabel(const PolizElem * elem) {}
};

class PolizInfPrint
{
public:
	PolizInfPrint(const PolizElem* elem) {}
};

class PolizInfArgs
{
public:
	PolizInfArgs(const PolizElem* elem) {}
};

class PolizUnprintable
{
public:
	PolizUnprintable(const PolizElem* elem) {}
};

class PolizNotVar
{
public:
	PolizNotVar(const PolizElem * elem) {}
};





//Poliz Interpret classes

class PolizElem
{
public:
	virtual void Evaluate(PolizStack * stack, PolizItem ** cur_cmd, GameClient * client) const = 0;
	virtual ~PolizElem() {}
};

class PolizConst:public PolizElem
{
public:
	virtual PolizElem * Clone() const = 0;
	void Evaluate(PolizStack * stack, PolizItem ** cur_cmd, GameClient * client) const
	{
		stack->Push(Clone());
		*cur_cmd = (*cur_cmd)->next;
	}
	virtual void Print() const = 0;
};

class PolizFunction:public PolizElem
{
public:
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const = 0;
	void Evaluate(PolizStack * stack, PolizItem ** cur_cmd, GameClient * client) const
	{
		PolizElem * res = EvaluateFun(stack, client);
		if(res)
			stack->Push(res);
		*cur_cmd = (*cur_cmd)->next;
	}
};


//PolizConst children

class PolizInt:public PolizConst
{
	int val;
public:
	PolizInt(const char * t_value)
	{
		val = atoi(t_value);
	}
	PolizInt(int a)
	{
		val = a;
	}
	virtual PolizElem * Clone() const
	{
		return new PolizInt(val);
	}
	virtual ~PolizInt() {}
	virtual int Get() const
	{
		return val;
	}
	virtual void Print() const
	{
		printf("%i", val);
	}
};

class PolizString:public PolizConst
{
	char * str;
public:
	PolizString(const char * astr)
	{
		str = new char[strlen(astr)+1];
		strcpy(str, astr);
	}
	virtual ~PolizString()
	{
		delete[]str;
	}
	PolizString(const PolizString & pstr)
	{
		str = new char[strlen(pstr.str)+1];
		strcpy(str, pstr.str);
	}
	const char * Get()const
	{
		return str;
	}
	virtual PolizElem * Clone() const
	{
		return new PolizString(*this);
	}
	virtual void Print() const
	{
		printf("%s", str);
	}
};


class PolizLabel:public PolizConst
{
	Label * label;
public:
	PolizLabel(Label * lab)
	{
		label = lab;
	}
	virtual ~PolizLabel() {}
	PolizItem * Get() const
	{
		return label->GetPoint();
	}
	virtual PolizElem * Clone() const
	{
		return new PolizLabel(label);
	}
	virtual void Print() const
	{
		throw PolizUnprintable(this);
	}
};


//Special const for various arg lists
class PolizArgEnd:public PolizConst
{
public:
	PolizArgEnd() {}
	virtual ~PolizArgEnd() {}
	virtual PolizElem * Clone() const
	{
		return new PolizArgEnd();
	}
	virtual void Print() const
	{
		throw PolizUnprintable(this);
	}
};

class PolizEnd:public PolizConst
{
public:
	PolizEnd() {}
	virtual ~PolizEnd() {}
	virtual PolizElem * Clone() const
	{
		return new PolizEnd();
	}
	virtual void Print() const
	{
		throw PolizUnprintable(this);
	}
};

//PolizFunction children

class PolizVar:public PolizInt
{
	Variable * var;
public:
	PolizVar(Variable * avar):PolizInt(0)
	{
		var = avar;
	}
	virtual ~PolizVar() {}
	virtual int Get() const
	{
		return var->GetValue();
	}
	virtual PolizElem * Clone() const
	{
		return new PolizVar(var);
	}
	Variable * GetVar() const
	{
		return var;
	}
	virtual void Print() const
	{
		printf("%i", var->GetValue());
	}
};

class PolizSell:public PolizFunction
{
public:
	PolizSell() {}
	virtual ~PolizSell() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * pc = dynamic_cast<PolizInt *>(op1);
		if(!pc)
			throw PolizNotInt(op1);
		PolizElem * op2 = stack->Pop();
		PolizInt * pv = dynamic_cast<PolizInt *>(op2);
		if(!pv)
			throw PolizNotInt(op2);
		int count, value;
		count = pc->Get();
		value = pv->Get();
		client->Sell(value, count);
		delete pc;
		delete pv;
		return NULL;
	}
};

class PolizBuy:public PolizFunction
{
public:
	PolizBuy() {}
	virtual ~PolizBuy() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * pc = dynamic_cast<PolizInt *>(op1);
		if(!pc)
			throw PolizNotInt(op1);
		PolizElem * op2 = stack->Pop();
		PolizInt * pv = dynamic_cast<PolizInt *>(op2);
		if(!pv)
			throw PolizNotInt(op2);
		int count, value;
		count = pc->Get();
		value = pv->Get();
		client->Buy(value,count);
		delete pc;
		delete pv;
		return NULL;
	}
};

class PolizProd:public PolizFunction
{
public:
	PolizProd() {}
	virtual ~PolizProd() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * pc = dynamic_cast<PolizInt *>(op1);
		if(!pc)
			throw PolizNotInt(op1);
		int count;
		count = pc->Get();
		client->Prod(count);
		delete pc;
		return NULL;
	}
};

class PolizBuild:public PolizFunction
{
public:
	PolizBuild() {}
	virtual ~PolizBuild() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * pc = dynamic_cast<PolizInt *>(op1);
		if(!pc)
			throw PolizNotInt(op1);
		int count;
		count = pc->Get();
		client->Build(count);
		delete pc;
		return NULL;
	}
};

class PolizUpgrade:public PolizFunction
{
	PolizUpgrade() {}
	virtual ~PolizUpgrade() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		printf("Upgraded\n");
		return NULL;
	}
};

class PolizEndturn:public PolizFunction
{
public:
	PolizEndturn() {}
	virtual ~PolizEndturn() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		printf("Turn ended\n");
		return NULL;
	}
};

class PolizFunPlus:public PolizFunction
{
public:
	PolizFunPlus() {}
	virtual ~PolizFunPlus() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * p1 = dynamic_cast<PolizInt *>(op1);
		if(!p1)
			throw PolizNotInt(op1);
		PolizElem * op2 = stack->Pop();
		PolizInt * p2 = dynamic_cast<PolizInt *>(op2);
		if(!p2)
			throw PolizNotInt(op2);
		int i1, i2;
		i2 = p2->Get();
		i1 = p1->Get();
		delete p1;
		delete p2;
		return new PolizInt(i1+i2);
	}
};

class PolizFunMinus:public PolizFunction
{
public:
	PolizFunMinus() {}
	virtual ~PolizFunMinus() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * p1 = dynamic_cast<PolizInt *>(op1);
		if(!p1)
			throw PolizNotInt(op1);
		PolizElem * op2 = stack->Pop();
		PolizInt * p2 = dynamic_cast<PolizInt *>(op2);
		if(!p2)
			throw PolizNotInt(op2);
		int i1, i2;
		i2 = p2->Get();
		i1 = p1->Get();
		delete p1;
		delete p2;
		return new PolizInt(i2-i1);
	}
};


class PolizFunMinusAlone:public PolizFunction
{
public:
	PolizFunMinusAlone() {}
	virtual ~PolizFunMinusAlone() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * p1 = dynamic_cast<PolizInt *>(op1);
		if(!p1)
			throw PolizNotInt(op1);
		int i1;
		i1 = p1->Get();
		delete p1;
		return new PolizInt(-i1);
	}
};

class PolizFunMul:public PolizFunction
{
public:
	PolizFunMul() {}
	virtual ~PolizFunMul() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * p1 = dynamic_cast<PolizInt *>(op1);
		if(!p1)
			throw PolizNotInt(op1);
		PolizElem * op2 = stack->Pop();
		PolizInt * p2 = dynamic_cast<PolizInt *>(op2);
		if(!p2)
			throw PolizNotInt(op2);
		int i1, i2;
		i2 = p2->Get();
		i1 = p1->Get();
		delete p1;
		delete p2;
		return new PolizInt(i1*i2);
	}
};

class PolizFunDiv:public PolizFunction
{
public:
	PolizFunDiv() {}
	virtual ~PolizFunDiv() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * p1 = dynamic_cast<PolizInt *>(op1);
		if(!p1)
			throw PolizNotInt(op1);
		PolizElem * op2 = stack->Pop();
		PolizInt * p2 = dynamic_cast<PolizInt *>(op2);
		if(!p2)
			throw PolizNotInt(op2);
		int i1, i2;
		i1 = p1->Get();
		i2 = p2->Get();
		delete p1;
		delete p2;
		return new PolizInt(i2/i1);
	}
};

class PolizFunOr:public PolizFunction
{
public:
	PolizFunOr() {}
	virtual ~PolizFunOr() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * p1 = dynamic_cast<PolizInt *>(op1);
		if(!p1)
			throw PolizNotInt(op1);
		PolizElem * op2 = stack->Pop();
		PolizInt * p2 = dynamic_cast<PolizInt *>(op2);
		if(!p2)
			throw PolizNotInt(op2);
		int i1, i2;
		i2 = p2->Get();
		i1 = p1->Get();
		delete p1;
		delete p2;
		return new PolizInt(i1||i2);
	}
};

class PolizFunAnd:public PolizFunction
{
public:
	PolizFunAnd() {}
	virtual ~PolizFunAnd() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * p1 = dynamic_cast<PolizInt *>(op1);
		if(!p1)
			throw PolizNotInt(op1);
		PolizElem * op2 = stack->Pop();
		PolizInt * p2 = dynamic_cast<PolizInt *>(op2);
		if(!p2)
			throw PolizNotInt(op2);
		int i1, i2;
		i2 = p2->Get();
		i1 = p1->Get();
		delete p1;
		delete p2;
		return new PolizInt(i1&&i2);
	}
};

class PolizFunEq: public PolizFunction 
{
public:
	PolizFunEq() {}
	virtual ~PolizFunEq() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * p1 = dynamic_cast<PolizInt *>(op1);
		if(!p1)
			throw PolizNotInt(op1);
		PolizElem * op2 = stack->Pop();
		PolizInt * p2 = dynamic_cast<PolizInt *>(op2);
		if(!p2)
			throw PolizNotInt(op2);
		int i1, i2;
		i2 = p2->Get();
		i1 = p1->Get();
		delete p1;
		delete p2;
		return new PolizInt(i1==i2);
	}
};

class PolizFunLssr: public PolizFunction 
{
public:
	PolizFunLssr() {}
	virtual ~PolizFunLssr() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * p1 = dynamic_cast<PolizInt *>(op1);
		if(!p1)
			throw PolizNotInt(op1);
		PolizElem * op2 = stack->Pop();
		PolizInt * p2 = dynamic_cast<PolizInt *>(op2);
		if(!p2)
			throw PolizNotInt(op2);
		int i1, i2;
		i2 = p2->Get();
		i1 = p1->Get();
		delete p1;
		delete p2;
		return new PolizInt(i1>i2);
	}
};

class PolizFunGrtr: public PolizFunction 
{
public:
	PolizFunGrtr() {}
	virtual ~PolizFunGrtr() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * p1 = dynamic_cast<PolizInt *>(op1);
		if(!p1)
			throw PolizNotInt(op1);
		PolizElem * op2 = stack->Pop();
		PolizInt * p2 = dynamic_cast<PolizInt *>(op2);
		if(!p2)
			throw PolizNotInt(op2);
		int i1, i2;
		i2 = p2->Get();
		i1 = p1->Get();
		delete p1;
		delete p2;
		return new PolizInt(i1<i2);
	}
};

class PolizFunLeq: public PolizFunction 
{
public:
	PolizFunLeq() {}
	virtual ~PolizFunLeq() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * p1 = dynamic_cast<PolizInt *>(op1);
		if(!p1)
			throw PolizNotInt(op1);
		PolizElem * op2 = stack->Pop();
		PolizInt * p2 = dynamic_cast<PolizInt *>(op2);
		if(!p2)
			throw PolizNotInt(op2);
		int i1, i2;
		i2 = p2->Get();
		i1 = p1->Get();
		delete p1;
		delete p2;
		return new PolizInt(i1>=i2);
	}
};

class PolizFunGeq: public PolizFunction 
{
public:
	PolizFunGeq() {}
	virtual ~PolizFunGeq() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * p1 = dynamic_cast<PolizInt *>(op1);
		if(!p1)
			throw PolizNotInt(op1);
		PolizElem * op2 = stack->Pop();
		PolizInt * p2 = dynamic_cast<PolizInt *>(op2);
		if(!p2)
			throw PolizNotInt(op2);
		int i1, i2;
		i2 = p2->Get();
		i1 = p1->Get();
		delete p1;
		delete p2;
		return new PolizInt(i1<=i2);
	}
};

class PolizAssign:public PolizFunction
{
public:
	PolizAssign() {}
	virtual ~PolizAssign() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * p1 = dynamic_cast<PolizInt *>(op1);
		if(!p1)
			throw PolizNotVar(op1);
		PolizElem * op2 = stack->Pop();
		PolizVar * p2 = dynamic_cast<PolizVar *>(op2);
		if(!p2)
			throw PolizNotInt(op2);
		Variable * var = p2->GetVar();
		var->SetValue(p1->Get());
		delete p2;
		delete p1;
		return NULL;
	}
};

class PolizPrint:public PolizFunction
{
public:
	PolizPrint() {}
	virtual ~PolizPrint() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op;
		PolizConst * print_elem;
		op = stack->Pop();
		while(dynamic_cast<PolizArgEnd *>(op)== NULL && op!=NULL)
		{
			print_elem = dynamic_cast<PolizConst *>(op);
			if(!print_elem)
				throw PolizUnprintable(op);
			print_elem->Print();
			delete op;
			op = stack->Pop();
		}
		if(!op)
			throw PolizInfPrint(op);
		delete op;
		printf("\n");
		return NULL;
	}
};

class PolizIndex:public PolizFunction
{
public:
	PolizIndex() {}
	virtual ~PolizIndex() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1, * op2;
		PolizInt * index;
		PolizVar * var;
		Variable * indexed_var;
		op1 = stack->Pop();
		index = dynamic_cast<PolizInt *>(op1);
		if(!index)
			throw PolizNotInt(op1);
		op2 = stack->Pop();
		var = dynamic_cast<PolizVar *>(op2);
		if(!var)
			throw PolizNotVar(op2);
		indexed_var = (var->GetVar())->GetByIndex(index->Get());
		delete op1;
		delete op2;
		return new PolizVar(indexed_var);
	}
};

class PolizTurn:public PolizFunction
{       
public:
	PolizTurn() {}
	virtual ~PolizTurn() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op;
		op = stack->Pop();
		PolizArgEnd * end = dynamic_cast<PolizArgEnd *>(op);
		if(!end)
			throw PolizInfArgs(op);
		delete op;
		return new PolizInt(client->Turn());
	}
};

class PolizRaw:public PolizFunction
{ 
public:
	PolizRaw() {}
	virtual ~PolizRaw() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * pc = dynamic_cast<PolizInt *>(op1);
		if(!pc)
			throw PolizNotInt(op1);
		int id;
		id = pc->Get();
		delete pc;
		op1 = stack->Pop();
		PolizArgEnd * end = dynamic_cast<PolizArgEnd *>(op1);
		if(!end)
			throw PolizInfArgs(op1);;
		return new PolizInt(client->Money(id));	}	

};

class PolizId:public PolizFunction
{ 
public:
	PolizId() {}
	virtual ~PolizId() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op;
		op = stack->Pop();
		PolizArgEnd * end = dynamic_cast<PolizArgEnd *>(op);
		if(!end)
			throw PolizInfArgs(op);
		delete op;
		return new PolizInt(client->MyId());	
	}
};

class PolizPlayers:public PolizFunction
{ 
public:
	PolizPlayers() {}
	virtual ~PolizPlayers() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op;
		op = stack->Pop();
		PolizArgEnd * end = dynamic_cast<PolizArgEnd *>(op);
		if(!end)
			throw PolizInfArgs(op);
		delete op;
		return new PolizInt(client->Players());		
	}

};

class PolizActPlayers:public PolizFunction
{ 
public:
	PolizActPlayers() {}
	virtual ~PolizActPlayers() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op;
		op = stack->Pop();
		PolizArgEnd * end = dynamic_cast<PolizArgEnd *>(op);
		if(!end)
			throw PolizInfArgs(op);
		delete op;
		return new PolizInt(client->ActPlayers());		
	}	

};

class PolizSupply:public PolizFunction
{ 
public:
	PolizSupply() {}
	virtual ~PolizSupply() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op;
		op = stack->Pop();
		PolizArgEnd * end = dynamic_cast<PolizArgEnd *>(op);
		if(!end)
			throw PolizInfArgs(op);
		delete op;
		return new PolizInt(client->Supply());			
	}
};

class PolizRawPrice:public PolizFunction
{ 
public:
	PolizRawPrice() {}
	virtual ~PolizRawPrice() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op;
		op = stack->Pop();
		PolizArgEnd * end = dynamic_cast<PolizArgEnd *>(op);
		if(!end)
			throw PolizInfArgs(op);
		delete op;
		return new PolizInt(client->RawPrice());	
	}

};

class PolizDemand:public PolizFunction
{ 
public:
	PolizDemand() {}
	virtual ~PolizDemand() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op;
		op = stack->Pop();
		PolizArgEnd * end = dynamic_cast<PolizArgEnd *>(op);
		if(!end)
			throw PolizInfArgs(op);
		delete op;
		return new PolizInt(client->Demand());	
	}

};

class PolizProdPrice:public PolizFunction
{ 
public:
	PolizProdPrice() {}
	virtual ~PolizProdPrice() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op;
		op = stack->Pop();
		PolizArgEnd * end = dynamic_cast<PolizArgEnd *>(op);
		if(!end)
			throw PolizInfArgs(op);
		delete op;
		return new PolizInt(client->ProdPrice());	
	}

};

class PolizMoney:public PolizFunction
{ 
public:
	PolizMoney() {}
	virtual ~PolizMoney() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * pc = dynamic_cast<PolizInt *>(op1);
		if(!pc)
			throw PolizNotInt(op1);
		int id;
		id = pc->Get();
		delete pc;
		op1 = stack->Pop();
		PolizArgEnd * end = dynamic_cast<PolizArgEnd *>(op1);
		if(!end)
			throw PolizInfArgs(op1);
		return new PolizInt(client->Money(id));
	}
};

class PolizProduction:public PolizFunction
{ 
public:
	PolizProduction() {}
	virtual ~PolizProduction() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * pc = dynamic_cast<PolizInt *>(op1);
		if(!pc)
			throw PolizNotInt(op1);
		int id;
		id = pc->Get();
		delete pc;
		op1 = stack->Pop();
		PolizArgEnd * end = dynamic_cast<PolizArgEnd *>(op1);
		if(!end)
			throw PolizInfArgs(op1);
		return new PolizInt(client->Production(id));
	}

};

class PolizFacts:public PolizFunction
{ 
public:
	PolizFacts() {}
	virtual ~PolizFacts() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * pc = dynamic_cast<PolizInt *>(op1);
		if(!pc)
			throw PolizNotInt(op1);
		int id;
		id = pc->Get();
		delete pc;
		op1 = stack->Pop();
		PolizArgEnd * end = dynamic_cast<PolizArgEnd *>(op1);
		if(!end)
			throw PolizInfArgs(op1);
		return new PolizInt(client->Facts(id));
	}

};

class PolizAutoFacts:public PolizFunction
{ 
public:
	PolizAutoFacts() {}
	virtual ~PolizAutoFacts() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * pc = dynamic_cast<PolizInt *>(op1);
		if(!pc)
			throw PolizNotInt(op1);
		int id;
		id = pc->Get();
		delete pc;
		op1 = stack->Pop();
		PolizArgEnd * end = dynamic_cast<PolizArgEnd *>(op1);
		if(!end)
			throw PolizInfArgs(op1);
		return new PolizInt(client->AutoFacts(id));
	}

};

class PolizManuf:public PolizFunction
{ 
public:
	PolizManuf() {}
	virtual ~PolizManuf() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * pc = dynamic_cast<PolizInt *>(op1);
		if(!pc)
			throw PolizNotInt(op1);
		int id;
		id = pc->Get();
		delete pc;
		op1 = stack->Pop();
		PolizArgEnd * end = dynamic_cast<PolizArgEnd *>(op1);
		if(!end)
			throw PolizInfArgs(op1);
		return new PolizInt(client->Manuf(id));
	}

};

class PolizRawSold:public PolizFunction
{ 
public:
	PolizRawSold() {}
	virtual ~PolizRawSold() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * pc = dynamic_cast<PolizInt *>(op1);
		if(!pc)
			throw PolizNotInt(op1);
		int id;
		id = pc->Get();
		delete pc;
		op1 = stack->Pop();
		PolizArgEnd * end = dynamic_cast<PolizArgEnd *>(op1);
		if(!end)
			throw PolizInfArgs(op1);
		return new PolizInt(client->RawSold(id));
	}

};

class PolizProdBought:public PolizFunction
{ 
public:
	PolizProdBought() {}
	virtual ~PolizProdBought() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * pc = dynamic_cast<PolizInt *>(op1);
		if(!pc)
			throw PolizNotInt(op1);
		int id;
		id = pc->Get();
		delete pc;
		op1 = stack->Pop();
		PolizArgEnd * end = dynamic_cast<PolizArgEnd *>(op1);
		if(!end)
			throw PolizInfArgs(op1);
		return new PolizInt(client->ProdBought(id));
	}

};

class PolizResRawPrice:public PolizFunction
{ 
public:
	PolizResRawPrice() {}
	virtual ~PolizResRawPrice() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * pc = dynamic_cast<PolizInt *>(op1);
		if(!pc)
			throw PolizNotInt(op1);
		int id;
		id = pc->Get();
		delete pc;
		op1 = stack->Pop();
		PolizArgEnd * end = dynamic_cast<PolizArgEnd *>(op1);
		if(!end)
			throw PolizInfArgs(op1);
		return new PolizInt(client->RawPrice(id));
	}

};

class PolizResProdPrice:public PolizFunction
{ 
public:
	PolizResProdPrice() {}
	virtual ~PolizResProdPrice() {}
	virtual PolizElem * EvaluateFun(PolizStack * stack, GameClient * client) const
	{
		PolizElem * op1 = stack->Pop();
		PolizInt * pc = dynamic_cast<PolizInt *>(op1);
		if(!pc)
			throw PolizNotInt(op1);
		int id;
		id = pc->Get();
		delete pc;
		op1 = stack->Pop();
		PolizArgEnd * end = dynamic_cast<PolizArgEnd *>(op1);
		if(!end)
			throw PolizInfArgs(op1);
		return new PolizInt(client->ProdPrice(id));
	}

};

class PolizOpGo:public PolizElem
{
public:
	PolizOpGo() {}
	virtual ~PolizOpGo() {}
	virtual void Evaluate(PolizStack * stack, PolizItem ** cur_cmd, GameClient * client) const 
	{
		PolizElem * op = stack->Pop();
		PolizLabel * lab = dynamic_cast<PolizLabel *>(op);
		if(!lab)
			throw PolizNotLabel(op);
		PolizItem * point = lab->Get();
		if(!point)
			throw RuntimeErr("label points to nowhere");
		*cur_cmd = point;
		delete op;
	}
};

class PolizIf:public PolizElem
{                                  
public:
	PolizIf() {}
	virtual ~PolizIf() {}
	virtual void Evaluate(PolizStack * stack, PolizItem ** cur_cmd, GameClient * client) const 
	{
		PolizElem * op = stack->Pop();
		PolizLabel * lab = dynamic_cast<PolizLabel *>(op);
		if(!lab)
			throw PolizNotLabel(op);
		PolizItem * point = lab->Get();
		if(!point)
			throw RuntimeErr("label points to nowhere");
		PolizElem * condition = stack->Pop();
		PolizInt * c = dynamic_cast<PolizInt *>(condition);
		if(!c)
			throw PolizNotInt(condition);
		if((!c->Get()))
			*cur_cmd = point;
		else
			*cur_cmd = (*cur_cmd)->next;
		delete c;
		delete op;
	}
};



#endif


















































