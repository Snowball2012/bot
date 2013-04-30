#include <stdio.h>
#include "buffer.h"

#define MAX_BUFF_SIZE 128

void Buffer::AddChar(char c)
{
	buff[index] = c;
	index++;
	if(index >=size) {
		size += MAX_BUFF_SIZE;
		char * ext_buff = new char[size];
		for(int i(0); i<index; ++i)
			ext_buff[i] = buff[i];
		delete buff;
		buff = ext_buff;
	}
	buff[index] = '\0';
}

void Buffer::AddLine(const char * line) 
{
	while (*line != '\0')
	{
		AddChar(*line);
		line++;
	}
}

const char * Buffer::GetBuff() const
{
	return buff;
}

void Buffer::Clear()
{
	delete [] buff;
	size = MAX_BUFF_SIZE;
	index = 0;
	buff = new char[size];
	buff[0] = '\n';
}

Buffer::Buffer()
{
	buff = new char[MAX_BUFF_SIZE];
	buff[0] = '\n';
	index = 0;
	size = MAX_BUFF_SIZE;
}

Buffer::Buffer(const Buffer & cbuff)
{
	buff = new char[cbuff.size];
	index = cbuff.index;
	size = cbuff.size;
	char * temp = buff;
	char * temp2 = cbuff.buff;
	while ((*temp++ = *temp2++)!='\0');
}

Buffer::~Buffer()
{
	if(buff) 
		delete buff;
}
