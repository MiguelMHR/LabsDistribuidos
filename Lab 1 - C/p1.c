#include <stdio.h>

int main(int args, char**argv)
{
	int i;
	for (i = 1; i < args; i++)
	{
		printf("Argumento %d = %s\n", i, argv[i]);
	}
	return 0;
}
