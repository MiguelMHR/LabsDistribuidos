#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

void ObtenerMinMax(long numbers[], int num)
{
	long min = LLONG_MAX;
	long max = LONG_MIN;
	for (int i = 0; i < num; i++)
	{
		min = (numbers[i] < min) ? numbers[i] : min;
		max = (numbers[i] > max) ? numbers[i] : max;
	}
	printf("Max value = %ld\n", max);
	printf("Min value = %ld\n", min);
}

void ImprimirCadenas(char **strings, int cadenas)
{
	for (int i = 0; i < cadenas; i++)
	{
		printf("Cadena: %s\n", strings[i]);
	}
}

int main(int args, char**argv)
{
	int i;
	char *end;
	int num = 0;
	int cadenas = 0;
	long *numbers = (long *)malloc(args * sizeof(long));
	char **strings = (char **)malloc(args * sizeof(char *));
	for (i = 1; i < args; i++)
	{
		long n = strtol(argv[i], &end, 10);
		if (*end != '\0')
		{
			strings[cadenas] = (char *)malloc(strlen(argv[i]) + 1);
			strcpy(strings[cadenas], argv[i]);
			cadenas++;
		}
		else
		{
			numbers[num] = n;
			num++;
		}
	}
	
	ObtenerMinMax(numbers, num);
	ImprimirCadenas(strings, cadenas);
	free(numbers);
	for (i = 0; i < cadenas; i++)
	{
		free(strings[i]);
	}
	free(strings);
	return 0;
}
