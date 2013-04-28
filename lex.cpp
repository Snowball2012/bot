// Lexic part, made by Snowball 1.04.2013

#include "lex.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


//---------------------------------------------------------
//Lex methods definition
//---------------------------------------------------------

Lex::Lex(TypeLex atype, const char * str, int line_num):type(atype), line(line_num)
{
	text_repres = new char[strlen(str)+1];
	strcpy(text_repres, str);
}

Lex::~Lex()
{
	delete [] text_repres;
}

Lex::Lex(const Lex & cp_lex)
{
	line=cp_lex.line;
	type=cp_lex.type;
	text_repres = new char[strlen(cp_lex.text_repres)+1];
	strcpy(text_repres, cp_lex.text_repres);
}

void Lex::Print()
{
	printf(text_repres);
}

TypeLex Lex::GetType()
{
	return type;
}

const char * Lex::GetText()
{
	return text_repres;
}

//---------------------------------------------------------



//---------------------------------------------------------
//Scanner methods definition
//---------------------------------------------------------

Scanner::Scanner()
{
	state = start;
}


Lex Scanner::Step(const char c)
{
	Lex * lexem = NULL;
	switch (state) {
		case start:
			if (isdigit(c))
				state = number;
			else if (c == '?' || c == '@' || c == '$') {
				state = identificator;
				switch (c) {
					case '?': 
						lex_type = Lex::types::function;
						break;
					case '@':
						lex_type = Lex::types::label;
					case '$':
						lex_type = Lex::types::variable;
				}
			}
			else if (isalpha(c))
				state = keyword;
			else if (c == ':')
				state = assignment;
			else if (c == '\"')
				state = string;
			else if (c == '+' || c == '-' || c == '%' || c == '*' || c == '/' || c == ':' || c == ';' || c == '&' || c == '|' || c == '{' || c == '}' || c == ';' || c == '(' || c== ')' || c == '>' || c == '<' || c == '=' || c == '[' || c == ']') 
				state = separator;
			else state = error;

			if(state != error) {
				buff.AddChar(c);
			}
			if(state == separator) {
				lexem = new Lex(Lex::types::separator, buff.GetBuff());
				buff.Clear();
				state = start;
			}
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
	return lexem;
}

//------------------------------------------------------
