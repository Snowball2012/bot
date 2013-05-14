#include <stdio.h>
#include "parser.h"


int main(int argc, char ** argv)
{
	try{
		Parser prs;
		prs.Interpret();
	}
	catch(SyntErr error) {
		printf("(%i, %s) %s\n",error.GetLine(), error.GetLexText(), error.GetMsg());
	}
	catch(LexError err) {
		printf("Lexic error on %i line, buff state: %s\n", err.GetLine(), err.GetLex());
	}
	return 0;
}
