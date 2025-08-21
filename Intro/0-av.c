#include <stdio.h>

int main(int ac __attribute__((unused)), char **av)
{
	while (*av){
		printf("%s\n", (*av++));
	}
	return (0);
}
