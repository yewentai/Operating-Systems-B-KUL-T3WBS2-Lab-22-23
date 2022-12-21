#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main()
{
	int a = 1;
	int* p;
	p = &a;
	printf("\nSize of int: %ld", sizeof(int));
	printf("\nSize of float: %ld", sizeof(float));
	printf("\nSize of double: %ld", sizeof(double));
	printf("\nSize of void: %ld", sizeof(void));
	printf("\nSize of pointer: %ld", sizeof(p));
	printf("\nSize of short: %ld", sizeof(short));
	printf("\nSize of long: %ld", sizeof(long));
	printf("\nLimit of char: %d\n", CHAR_MIN);
	exit(0);
}
