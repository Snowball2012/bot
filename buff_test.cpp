#include <stdio.h>
#include "buffer.h"

int main(int argc, char ** argv)
{
	Buffer buff;
	char c;
	printf("Syntax analisys\n");
	while((c = getchar())!= EOF)
		buff.AddChar(c);
	buff.AddLine("Hello, world!\n");
	printf(buff.GetBuff());
	buff.Clear();
	while((c = getchar())!= EOF)
		buff.AddChar(c);
	buff.AddLine("#2 Hello, world!\n");
	printf(buff.GetBuff());
	return 0;
}
