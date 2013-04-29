// Lexic part, made by Snowball 1.04.2013

#include "lex.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>


//---------------------------------------------------------
//Lex methods definition
//---------------------------------------------------------

Lex::Lex(Type atype, const char * str, int line_num):type(atype), line(line_num)
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

void Lex::Print() const
{
	printf("(%i, %s, %i)", type, text_repres, line);
}

Lex::Type Lex::GetType() const
{
	return type;
}

const char * Lex::GetText() const
{
	return text_repres;
}

//---------------------------------------------------------



//---------------------------------------------------------
//Scanner methods definition
//---------------------------------------------------------

const char * const Scanner::keywords[KEYWORDS_COUNT] = {
	"if",
	"then",
	"goto",
	"print",
	"buy",
	"sell",
	"prod",
	"build",
	"endturn",
	"while",
	"do"
};

const Lex::Type Scanner::kw_types[KEYWORDS_COUNT] = {
	Lex::lex_if,
	Lex::lex_then,
	Lex::lex_goto,
	Lex::lex_print,
	Lex::lex_buy,
	Lex::lex_sell,
	Lex::lex_prod,
	Lex::lex_build,
	Lex::lex_endturn,
	Lex::lex_while,
	Lex::lex_do
};

Scanner::Scanner()
{
	state = start;
	line = 1;
	lexems = NULL;
}

Lex * Scanner::GetLex()
{
	if(!lexems)
		return NULL;
	Lex * lexem = lexems->lex;
	LexList * temp = lexems;
	lexems = lexems->next;
	delete temp;
	return lexem;
}

void Scanner::AddLexem(Lex * lexem)
{
	LexList * new_elem = new LexList;
	new_elem->next = NULL;
	new_elem->lex = lexem;
	if (!lexems) {
		lexems = new_elem;
	} else {
		LexList * temp = lexems;
		while(temp->next)
			temp = temp->next;
		temp->next = new_elem;
	}	
}


void Scanner::Step(const char c)
{
	switch (state) {
		case start:
			Start(c);
			break;
		case number:
			Number(c);
			break;
		case identificator:
			Ident(c);
			break;
		case keyword:
			Keyword(c);
			break;
		case assignment:
			Assign(c);
			break;
		case string:
			String(c);
	}
}

Lex::Type Scanner::FindSepType(const char c) const
{
	switch (c) {
		case ';': return Lex::lex_semicolon;
		case ',': return Lex::lex_comma;
		case '(': return Lex::lex_lrbrace;
		case ')': return Lex::lex_rrbrace;
		case '{': return Lex::lex_lfbrace;
		case '}': return Lex::lex_rfbrace;
		case '[': return Lex::lex_lsbrace;
		case ']': return Lex::lex_rsbrace;
		case ':': return Lex::lex_colon;
		case '<': return Lex::lex_lssr;
		case '>': return Lex::lex_grtr;
		case '+': return Lex::lex_plus;
		case '-': return Lex::lex_minus;
		case '*': return Lex::lex_times;
		case '/': return Lex::lex_slash;
		case '|': return Lex::lex_or;
		case '&': return Lex::lex_and;
	}
	return Lex::lex_null;
}

Lex::Type Scanner::FindKeywordType(const char * word) const
{
	Lex::Type type = Lex::lex_null;
	for (int i = 0; i < KEYWORDS_COUNT; ++i) {
		if(!strcmp(word, keywords[i])) {
			type = kw_types[i];
			break;
		}
	}
	return type;
}

void Scanner::Start(const char c)
{
	buff.Clear();
	buff.AddChar(c);
													
	if(isalpha(c)) 
		state = keyword;
	else if(isdigit(c)) 
		state = number;
	else if(c == '?' || c == '@' || c == '$')
		state = identificator;
	else if(c == '=')
		state = assignment;
	else if(c == '\"')
		state = string;
	else if(InSeps(c)) {
		Lex::Type type = FindSepType(c);
		Lex * lexem = new Lex(type, buff.GetBuff(), line);
		AddLexem(lexem);
	} else if(c == '\n')
		line++;
	else if(!(c == ' ' || c == '\t' || c == '\r'))
		throw LexError(line);
}

void Scanner::Number(const char c)
{
	if(isdigit(c)) {
		buff.AddChar(c);
	} else {
		if (!(c == '\n' || c == '\t' || c == '\r' || c == ' ' 
			|| c == '=' || InSeps(c))) {
			throw LexError(line);
		}
		Lex * lexem = new Lex(Lex::lex_num, buff.GetBuff(), line);
		AddLexem(lexem);
		state = start;
		Step(c);
	}
}

void Scanner::Ident(const char c)
{
	if(isalnum(c)||c == '_') {
		buff.AddChar(c);
	} else {
		const char * lex_text = buff.GetBuff();
		if (lex_text[1]=='\0')
			throw LexError(line);
		if (!(c == '\n' || c == '\t' || c == '\r' || c == ' ' 
			|| c == '=' || InSeps(c))) {
			throw LexError(line);
		}
		Lex::Type type;
		switch (lex_text[0]) {
			case '@':
				type = Lex::lex_label;
				break;
			case '$':
				type = Lex::lex_var;
				break;
			case '?':
				type = Lex::lex_func;
				break;
		}
		Lex * new_lex = new Lex(type, lex_text, line);
		AddLexem(new_lex);
		state = start;
		Step(c);
	}
}

void Scanner::Keyword(const char c)
{
	if(isalpha(c)) {
		buff.AddChar(c);
	} else {
		if(!(c == '\n' || c == '\t' || c == '\r' || c == ' ' 
			|| c == '=' || InSeps(c))) {
			throw LexError(line);
		}
		Lex::Type type;
		type = FindKeywordType(buff.GetBuff());
		if (type == Lex::lex_null)
			throw LexError(line);
		Lex * new_lex = new Lex(type, buff.GetBuff(), line);
		AddLexem(new_lex);
		state = start;
		Step(c);
	}
}

//manages assignment and  some comparations (==, =>, =<)
void Scanner::Assign(const char c)
{
	Lex * new_lex;
	state = start;
	switch(c) {
		case '=':
			buff.AddChar(c);
			new_lex = new Lex(Lex::lex_eq, buff.GetBuff(), line);
			AddLexem(new_lex);
			break;
		case '>':
			buff.AddChar(c);
			new_lex = new Lex(Lex::lex_geq, buff.GetBuff(), line);
			AddLexem(new_lex);
			break;
		case '<':
			buff.AddChar(c);
			new_lex = new Lex(Lex::lex_leq, buff.GetBuff(), line);
			AddLexem(new_lex);
			break;
		default:
			new_lex = new Lex(Lex::lex_assign, buff.GetBuff(), line);
			AddLexem(new_lex);
			Step(c);
	}
}

void Scanner::String(const char c)
{
	if (c!='\"') {
		buff.AddChar(c);
	}
	else {
		state = start;
		Lex * new_lex = new Lex(Lex::lex_str, buff.GetBuff() + 1, line); 
		//+1 because of first quotes
		AddLexem(new_lex);
	}
}



inline bool Scanner::InSeps(const char c) const
{
	return c == ';' || c == ',' || c == '(' || c == '|' || c == '&'
		|| c == ')' || c == '{' || c == '}' || c == '[' 
		|| c == ']' || c == ':' || c == '<' || c == '>' 
		|| c == '+' || c == '-' || c == '*' || c == '/';
}
//------------------------------------------------------
