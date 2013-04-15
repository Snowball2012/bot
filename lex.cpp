// Lexic part, made by Snowball 1.04.2013

#include "lex.h"

/* state transitions table */

static states Scanner::transitions[STATE_COUNT][256] = {
	error,
};

Scanner::Scanner()
{
	state = start;
}

Lex * Step(const char c)
{
	switch (state) {
		case start:
			if (c >= '0' && c <= '9')
				state = number;
			if (c == '?' || c == '@' || c == '$')
				state = identificator;
			if (c >= 'A' && c <= 'Z' || c >= 'a' && c <='z')
				state = keyword;
			if (c == ':')
				state = assignment;
			if (c == '\"')
				state = string;
			if (c == '+' || c == '-')
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
