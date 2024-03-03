#include <stdio.h>
#include <stdlib.h>

int main(int args, char**argv)
{
	int i;
	char *end;
	for (i = 1; i < args; i++)
	{
		long num = strtol(argv[i], &end, 10);
		if (*end != '\0')
		{
			printf("Argumento %d = Error de conversión. \n", i);
		}
		else
		{
			printf("Argumento %d = %ld\n", i, num);
		}
	}
	return 0;
}
