#include <stdlib.h>
#include <string.h>
#include "id_tables.h"
#include "lex.h"

#define MAX_TABL_SIZE 128


int Variable::GetValue() const
{
	if (initialized) {
		if(array)
			throw RuntimeErr("Can't get value of array");
		return value;
	} else {
		throw RuntimeErr("variable not initialized");
	}
	return 0;
}

void Variable::SetValue(int avalue)
{
	initialized = true;
	value = avalue;
	if(array)
		throw RuntimeErr("Assignment to array is forbidden");
}

Variable::Variable()
{
	initialized = false;
	array = false;
	arr = NULL;
}

Variable::~Variable()
{
	if(array) {
		for(int i = 0; i<size; ++i)
			delete arr[i];
		delete [] arr;
	}
}

Variable * Variable::GetByIndex(int index)
{
	if(index < 0)
		throw RuntimeErr("index is negative");
	if(!array) {
		if(initialized)
			throw RuntimeErr("Can't create an array from initialized var");
		size = index+1;
		array = true;
		arr = new Variable*[size];
		for (int i = 0; i<size; ++i)
			arr[i] = new Variable();
	} else if (index >= size) {
			Variable ** new_arr = new Variable*[index+1];
			for (int i = 0; i<size; ++i)
				new_arr[i] = arr[i];
			for (int i = size; i<=index; i++)
				new_arr[i] = new Variable();
			size = index+1;
			delete arr;
			arr = new_arr;
	} 
	return arr[index];
}

PolizItem * Label::GetPoint() const
{
	if(point)
		return point;
	else
		throw RuntimeErr("label points to nowhere");
	return NULL;
}

void Label::SetPoint(PolizItem * apoint)
{
	point = apoint;
}

Label::Label()
{
	point = NULL;

}


template<class Item> 
IdTable<Item>::IdTable()
{
	size = 0;
	table = new Item*[MAX_TABL_SIZE];
	max_size = MAX_TABL_SIZE;
	info = new const char*[MAX_TABL_SIZE];
}
	
template<class Item> 
int IdTable<Item>::Find(Lex lexem)
{
	for (int i = 0; i<size; ++i) {
		if(!strcmp(lexem.GetText(), info[i])) 
			return i;
	}
	char * line = new char[strlen(lexem.GetText())+1];
	if(size>=max_size) {
		max_size += MAX_TABL_SIZE;
		Item ** temp = new Item*[max_size];
		const char ** temp_inf = new const char*[max_size];
		for (int i = 0; i<size; ++i) {
			temp[i] = table[i];
			temp_inf[i] = info[i];
		}
		delete [] table;
		delete [] info;
		table = temp;
		info = temp_inf;
	}
	strcpy(line, lexem.GetText());
	table[size] = new Item();
	info[size] = line;
	size++;
	return size-1;
}

template<class Item> 
Item * IdTable<Item>::Get(int num) const
{
	if(num>=size)
		throw RuntimeErr("identifier not found");
	return table[num];
}

template<class Item> 
int IdTable<Item>::Add(Item * it)
{
	if(size>=max_size) {
		max_size += MAX_TABL_SIZE;
		Item ** temp = new Item*[max_size];
		const char ** temp_inf = new const char*[max_size];
		for (int i = 0; i<size; ++i) {
			temp[i] = table[i];
			temp_inf[i] = info[i];
		}
		delete [] table;
		delete [] info;
		table = temp;
		info = temp_inf;
	}
	table[size] = new Item();
	info[size] = "\0";
	size++;
	return size-1;
}


template class IdTable<Variable>;
template class IdTable<Label>;


#undef MAX_TABL_SIZE











