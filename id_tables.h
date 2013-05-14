#ifndef ID_TABLES_H
#define ID_TABLES_H


class Lex;
struct PolizItem;
class RuntimeErr
{
	const char * msg;
public:
	RuntimeErr(const char *amsg):msg(amsg) {}
};

class Variable
{
public:
	int GetValue() const;
	void SetValue(int avalue);
	Variable();
	Variable * GetByIndex(int index);
	~Variable();
private:
	int value;
	bool initialized;
	bool array;
	int size;
	Variable ** arr;
};

class Label
{
public:
	PolizItem * GetPoint() const;
	void SetPoint(PolizItem * apoint);
	Label();
private:
	PolizItem * point;
};


template <class Item> 
class IdTable
{
public:
	int Find(Lex lexem);
	Item * Get(int num) const;
	IdTable();
	int Add(Item * it);
private:
	int size;
	int max_size;
	Item ** table;
	const char ** info;
};





#endif
