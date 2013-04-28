/* Lexic part, made by Snowball 1.04.2013 */

#ifndef LEX_H
#define LEX_H

#include "buffer.h"

#define STATE_COUNT 8

class Lex
{
public:
	enum TypeLex {
		lex_null,
		lex_num,
		lex_var,
		lex_func,
		lex_label,
		lex_if,
		lex_then,
		lex_goto,
		lex_print,
		lex_buy,
		lex_sell,
		lex_prod,
		lex_build,
		lex_endturn,
		lex_semicolon,
		lex_comma,
		lex_colon,
		lex_assign,
		lex_lfbrace,
		lex_rfbrace,
		lex_rrbrace,
		lex_lrbrace,
		lex_lsbrace,
		lex_rsbrace,
		lex_eq,
		lex_lssr,
		lex_grtr,
		lex_plus,
		lex_minus,
		lex_times,
		lex_slash,
		lex_leq,
		lex_geq		
	};	
	Lex(TypeLex type, const char * str, int line_num);
	Lex(const Lex & cp_lex);
	~Lex();
	void Print() const;
	TypeLex GetType() const;
	const char * GetText() const;
private:
	TypeLex type;
	char * text_repres;
	int line;
};

class Scanner
{
public:
	Scanner();
	Lex Step(const char c);
	enum States {
		start,
		number,
		identificator,
		keyword,
		assignment,
		string,
		separator,
		error
	};

private:
	States state;
	Buffer buff;
}

#endif //LEX_H
