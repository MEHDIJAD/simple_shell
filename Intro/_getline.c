#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	char *line = NULL;
	size_t len = 0;
	while (1)
	{
		printf("$ ");
		if (getline(&line, &len, stdin) == -1)
			return (EXIT_FAILURE);
		else
			printf("%s", line);
	}
	free(line);
	return(EXIT_SUCCESS);
}
