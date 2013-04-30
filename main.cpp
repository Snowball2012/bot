#include <stdio.h>
#include "parser.h"


int main(int argc, char ** argv)
{
	try{
		Parser prs;
		prs.Analyse();
		printf("done\n");
	}
	catch(SyntErr error) {
		printf("(%i, %s) %s\n",error.GetLine(), error.GetLexText(), error.GetMsg());
	}
	return 0;
}
