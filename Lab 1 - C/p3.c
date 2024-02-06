#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

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

int main(int args, char**argv)
{
	int i;
	char *end;
	int num = 0;
	long *numbers = (long *)malloc(args * sizeof(long));
	for (i = 1; i < args; i++)
	{
		long n = strtol(argv[i], &end, 10);
		if (*end != '\0')
		{
			printf("Argumento %d = NaN \n", i);
		}
		else
		{
			numbers[num] = n;
			num++;
		}
	}
	
	ObtenerMinMax(numbers, num);
		
	free(numbers);
	return 0;
}
