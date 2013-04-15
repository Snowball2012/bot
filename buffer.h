#ifndef BUFFER_H
#define BUFFER_H

class Buffer
{
	char * buff;
	int index;
	int size;
public:
	void AddChar(char c);
	void AddLine(const char * line); 
	const char * GetBuff() const;
	void Clear();
	Buffer();
	Buffer(const Buffer & cbuff);
	~Buffer();
};

#endif
