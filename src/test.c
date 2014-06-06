#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int32_t strtoi(char* str);

int main(int argc, char* argv[]) {
	char* str = argv[1];
	int32_t binary = strtol(str, NULL, 2);
	printf("%d\n", binary);
}

int32_t strtoi(char* str)
{
	int32_t command = 0;
	for (int i=0; i<sizeof(str); i++)
	{	
		int b;
		b = str[i] - '0';
		command += b;
		command <<= 1;
	}
	return command;
}
