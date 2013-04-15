/* Lexic part, made by Snowball 1.04.2013 */

#ifndef LEX_H
#define LEX_H

#include "buffer.h"

#define STATE_COUNT 8

class Scanner
{
public:
	Scanner();
	Lex * Step(const char c);
private:
	enum states {
		start,
		number,
		identificator,
		keyword,
		assignment,
		string,
		separator,
		error
	} state;
	Buffer buff;
	static states transitions[STATE_COUNT][256];
}

#endif //LEX_H
