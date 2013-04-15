// Lexic part, made by Snowball 1.04.2013

#include "lex.h"
#include <ctype.h>


Scanner::Scanner()
{
	state = start;
}

Lex * Step(const char c)
{
	switch (state) {
		case start:
			if (isdigit(c))
				state = number;
			if (c == '?' || c == '@' || c == '$')
				state = identificator;
			if (isalpha(c))
				state = keyword;
			if (c == ':')
				state = assignment;
			if (c == '\"')
				state = string;
			if (c == '+' || c == '-' || c == ) //TODO
			break;
		case number:
			break;
		case identificator:
			break;
		case keyword:
			break;
		case assignment:
			break;
		case string:
			break;
		case separator:
			break;
	}
}
