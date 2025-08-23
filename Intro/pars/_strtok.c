#include <string.h>
#include <stdio.h>

// char * strtok(char *restrict str, const char *restrict sep);

int main(int ac __attribute__((unused)), char **av)
{
	printf("%s\n", strtok(av[1], " "));
}

