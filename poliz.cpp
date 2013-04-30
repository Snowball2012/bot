/*	Made by Snowball 29.04.2013
 *	poliz.cpp */

#include <ctype.h>
#include <stdlib.h>
#include "poliz.h"
#include "lex.h"
#include "parser.h"

//PolizStack realization

PolizStack::PolizStack(PolizIdTable * aidents):idents(aidents)
{
	head = NULL;
	queue = NULL;
}

PolizStack::~PolizStack()
{
	while(head) {
		PolizItem * temp = head;
		delete head->elem;
		head = head->next;
		delete temp;
	}
}

PolizElem * PolizStack::Pop()
{
	if(!head)
		return NULL;
	PolizItem * temp = head;
	PolizElem * ret_elem = temp->elem;
	head = head->next;
	delete temp;
	return ret_elem;
}

const PolizStack::PolizItem * PolizStack::GetHead()
{
	return head;
}

void PolizStack::Push(PolizElem * new_elem)
{
	PolizItem * new_item = new PolizItem();
	new_item->elem = new_elem;
	new_item->next = head;
	head = new_item;
}

void PolizStack::PushBack(PolizElem * new_elem)
{
	PolizItem * new_item = new PolizItem();
	new_item->elem = new_elem;
	new_item->next = NULL;
	if(!head) {
		head = new_item;
	} else {
		PolizItem * temp = head;
		while(temp->next) 
			temp = temp->next;
		temp->next = new_item;
	}
	while(queue) {
		LabelQueue * temp = queue;
		idents->InitLabel(temp->label, new_item);
		queue = queue->next;
		delete temp->label;
		delete temp;
	}
}

void PolizStack::PutLabel(Lex label)
{
	if(label.GetType()!=Lex::lex_label && label.GetType()!=Lex::lex_iflbl)
		throw NotLabelEx(label);
	PolizLabel * lbl = static_cast<PolizLabel *>(idents->CreateElem(label));
	LabelQueue * new_elem = new LabelQueue();
	new_elem->label = lbl;
	new_elem->next = queue;
	queue = new_elem;
}

//---------------------------------------------------------------------------

//PolizIdTable realization --------------------------------------------------


Lex PolizIdTable::CreateLabel()
{
	return Lex(Lex::lex_iflbl, iflabel_count++, 0); 
}


//Warning:giant switch 

PolizElem * PolizIdTable::CreateElem(Lex lex)
{
	PolizElem * ret_val = NULL;
	Lex::Type lex_type = lex.GetType();
	const char * ltext = lex.GetText();
	switch (lex_type) {
		case Lex::lex_iflbl:
		case Lex::lex_label: ret_val = CreateRealLabel(ltext, lex_type); break;
		case Lex::lex_var: ret_val = CreateVar(ltext); break;
		case Lex::lex_func:	ret_val = CreateFunc(ltext); break;
		case Lex::lex_num: ret_val = new PolizInt(); break;          //THANK GOD I HAVE VIM
		case Lex::lex_str: ret_val = new PolizString(); break;
		case Lex::lex_buy: ret_val = new PolizBuy(); break;
		case Lex::lex_sell: ret_val = new PolizSell(); break;
		case Lex::lex_prod: ret_val = new PolizProd(); break;
		case Lex::lex_build: ret_val = new PolizBuild(); break;
		case Lex::lex_plus: ret_val = new PolizFunPlus(); break;
		case Lex::lex_minus: ret_val = new PolizFunMinus(); break;
		case Lex::lex_times: ret_val = new PolizFunMul(); break;
		case Lex::lex_or: ret_val = new PolizFunOr(); break;
		case Lex::lex_and: ret_val = new PolizFunAnd(); break;
		case Lex::lex_slash: ret_val = new PolizFunDiv(); break;
		case Lex::lex_eq: ret_val = new PolizFunEq(); break;
		case Lex::lex_lssr: ret_val = new PolizFunLssr(); break;
		case Lex::lex_grtr: ret_val = new PolizFunGrtr(); break;
		case Lex::lex_leq: ret_val = new PolizFunLeq(); break;
		case Lex::lex_geq: ret_val = new PolizFunGeq(); break;
		default:
			throw SyntErr(NULL, "incorrect lexem, smth strange happens");
	}
	return ret_val;
}

PolizStack::PolizItem * & PolizIdTable::FindLabel(const char * lex_inf, Lex::Type lex_type)
{
	ltabl_item * label;
	if(lex_type == Lex::lex_label) {
		label = ltabl;
	} else {
		label = ifltabl;
	}
	while(label!=NULL) {
		if(lex_type == Lex::lex_label) {
			if(!strcmp(lex_inf, label->inf))
				return label->point;
		} else {
			if(lex_inf==label->inf)
				return label->point;
		}
	}
	if(lex_type == Lex::lex_label) {
		label = ltabl;
	} else {
		label = ifltabl;
	}
//TODO:finish FindLabel, осталось только добавить новый элемент в таблицу
}

PolizElem * PolizIdTable::CreateRealLable(const char * ltext, Lex::Type lex_type)
{
	PolizLabel * lab = new PolizLabel(FindLabel(ltext, lex_type));
}

































