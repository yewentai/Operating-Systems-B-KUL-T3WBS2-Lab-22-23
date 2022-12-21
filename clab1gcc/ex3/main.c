#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 20 // max number of characters in a string

int main()
{
	char first[MAX], second[MAX], name[MAX], str[MAX];
	int ret, year;

	printf("Please input your first name:\n");
	scanf("%s", first);
	printf("Please input your second name:\n");
	scanf("%s", second);

	/*
	* Convert your second name to all upper case chars
	* and store the result in str.
	*/
	strcpy(str, second);
	for (int i = 0; i < strlen(str); i++)
	{
		if (str[i] >= 'a' && str[i] <= 'z')
		{
			str[i] = str[i] - 32;
		}
	};
	printf("Your second name in upper case is:%s\n", str);

	ret = strcmp(second, str);

	if (ret < 0)
	{
		printf("second is less than str\n");
	}
	else if (ret > 0)
	{
		printf("second is greater than str\n");
	}
	else
	{
		printf("second is equal to str\n");
	}
	strcpy(name, first);
	strcat(name, " ");
	strcat(name, second);
	printf("Your full name is:%s\n", name);

	printf("Please input your year of birth(like: 1999):\n");
	scanf("%d", &year);
	strcpy(name, "");
	snprintf(name, MAX, "%s", first);
	snprintf(name + strlen(name), MAX - strlen(name), " %s", second);
	snprintf(name + strlen(name), MAX - strlen(name), " %d", year);
	printf("Your full name is:%s\n", name);
	sscanf(name, "%s %s %d", first, second, &year);
	printf("Your first name is:%s\n", first);
	printf("Your second name is:%s\n", second);
	printf("Your year of birth is:%d\n", year);
	return 0;
}
