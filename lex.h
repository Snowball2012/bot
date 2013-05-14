/* Lexic part, made by Snowball 1.04.2013 */

#ifndef LEX_H
#define LEX_H

#include "buffer.h"

#define KEYWORDS_COUNT 11

class Lex
{
public:
	enum Type {
		lex_null,
		lex_num,
		lex_var,
		lex_func,
		lex_label,
		lex_str,
		lex_if,
		lex_then,
		lex_goto,
		lex_print,
		lex_buy,
		lex_sell,
		lex_prod,
		lex_build,
		lex_endturn,
		lex_while,
		lex_do,
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
		lex_geq,
		lex_leq,
		lex_and,
		lex_or,
		lex_iflbl
	};	
	Lex(Type type, const char * str, int line_num);
	Lex(const Lex & cp_lex);
	~Lex();
	void Print() const;
	Type GetType() const;
	const char * GetText() const;
	int GetLine() const { return line; }
	const Lex & operator= (const Lex & lexop);
	int GetPos() const;
private:
	Type type;
	char * text_repres;
	int line;
	int pos;
};

struct LexList 
{
	Lex * lex;
	LexList * next;
};

class Scanner
{
public:
	Scanner();
	void Step(const char c);
	Lex * GetLex();
	enum States {
		start,
		number,
		identificator,
		keyword,
		assignment,
		string,
	};

private:
	States state;
	Buffer buff;
	LexList * lexems;
	int line;
	void Start(const char c);
	void Number(const char c);
	void Ident(const char c);
	void Keyword(const char c);
	void Assign(const char c);
	void String(const char c);
	void AddLexem(Lex * lexem);
	Lex::Type FindSepType(const char c) const;
	Lex::Type FindKeywordType(const char * word) const;
	inline bool InSeps(const char c) const;
	static const char * const keywords[KEYWORDS_COUNT];
	static const Lex::Type kw_types[KEYWORDS_COUNT];
};

class LexError
{
	int line;
public:
	LexError(int aline):line(aline) {}
	int GetLine() const
	{
		return line;
	}
};

#endif //LEX_H
