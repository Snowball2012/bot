/*	Made by Snowball 29.04.2013
 *	poliz.cpp */

#include <ctype.h>
#include <stdlib.h>
#include "poliz.h"
#include "lex.h"
#include "parser.h"
#include "client.h"

//PolizStack realization

PolizStack::PolizStack()
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

PolizItem * PolizStack::GetHead()
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
		(queue->label)->SetPoint(new_item);
		queue = queue->next;
		delete temp;

	}
}

void PolizStack::PutLabel(Label * label)
{
	LabelQueue * new_elem = new LabelQueue();
	new_elem->label = label;
	new_elem->next = queue;
	queue = new_elem;
}



































