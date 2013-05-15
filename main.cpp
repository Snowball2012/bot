#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "client.h"


int main(int argc, char ** argv)
{
	try{
		int port;
		if(argc<5)
			throw "no params";
		else
			port = atoi(argv[2]);
		Parser prs(port, argv[1], argv[3], atoi(argv[4]));
		prs.Interpret();
	}
	catch(SyntErr error) {
		printf("(%i, %s) %s\n",error.GetLine(), error.GetLexText(), error.GetMsg());
	}
	catch(LexError err) {
		printf("Lexic error on %i line, buff state: %s\n", err.GetLine(), err.GetLex());
	}
	catch(const char * msg) {
		printf("%s\n",msg);
	}
	catch(ClientEx ex) {
		ex.Print();
	}
	return 0;
}
