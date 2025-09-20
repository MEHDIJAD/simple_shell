#include <stddef.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

/**
 * print_prompt - prints the shell prompt
 */
void printPrompt(void)
{
	write(STDOUT_FILENO, "#cisfun$ ", 9);
}

char *readCommand(void)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	
	read = getline(&line, &len, stdin);
	if (read == -1){
		/* Handle EOF (Ctrl+D) */
		if (line)
			free(line);
		return (NULL);
	}
	/* Remove newline character */
	if (read > 0 && line[read - 1] == '\n')
		line[read - 1] = '\0';
	return (line);
}

/**
 * count_tokens - counts the number of tokens in a string
 * @str: the string to count tokens in
 * Return: number of tokens
 */
int count_tokens(char *str)
{
	int count = 0;
	char *token;
	char *str_copy = malloc(strlen(str) + 1);
	
	if (!str_copy)
		return (0);
	
	strcpy(str_copy, str);
	token = strtok(str_copy, " \t");
	while (token != NULL)
	{
		count++;
		token = strtok(NULL, " \t");
	}
	free(str_copy);
	return (count);
}

/**
 * parse_command - parses command line into tokens
 * @cmd: the command line string
 * Return: array of tokens (arguments)
 */
char **parse_command(char *cmd)
{
	int token_count = count_tokens(cmd);
	char **argv;
	char *token;
	int i = 0;
	
	if (token_count == 0)
		return (NULL);
	
	argv = malloc(sizeof(char *) * (token_count + 1));
	if (!argv)
		return (NULL);
	
	token = strtok(cmd, " \t");
	while (token != NULL)
	{
		argv[i] = malloc(strlen(token) + 1);
		if (!argv[i])
		{
			/* Free previously allocated memory */
			while (i > 0)
			{
				i--;
				free(argv[i]);
			}
			free(argv);
			return (NULL);
		}
		strcpy(argv[i], token);
		i++;
		token = strtok(NULL, " \t");
	}
	argv[i] = NULL;
	return (argv);
}

/**
 * free_argv - frees the argv array
 * @argv: array of strings to free
 */
void free_argv(char **argv)
{
	int i = 0;
	
	if (!argv)
		return;
	
	while (argv[i])
	{
		free(argv[i]);
		i++;
	}
	free(argv);
}

/**
 * executeCommand - executes a command with arguments
 * @cmd: the command line string
 * @env: environment variables
 */
void executeCommand(char *cmd, char **env)
{
	pid_t pid;
	int status;
	char **argv;
	
	/* Skip empty commands */
	if (strlen(cmd) == 0)
		return;
	
	argv = parse_command(cmd);
	if (!argv)
		return;
	
	pid = fork();
	if (pid == -1){
		perror("fork");
		free_argv(argv);
		return;
	}
	
	if (pid == 0) {
		/* Child process */
		if (execve(argv[0], argv, env) == -1){
			write(STDERR_FILENO, "./shell: No such file or directory\n", 35);
			exit(EXIT_FAILURE);
		}
	}
	else {
		/* Parent process - wait for child */
		waitpid(pid, &status, 0);
	}
	
	free_argv(argv);
}

/**
 * main - simple shell main function
 * @ac: argument count (unused)
 * @av: argument vector (unused) 
 * @env: environment variables
 * Return: 0 on success
 */
int main(int ac __attribute__((unused)), char **av __attribute__((unused)), char **env)
{
	char *cmd;
	
	while (true) {
		printPrompt();
		cmd = readCommand();
		if (!cmd){
			write(STDOUT_FILENO, "\n", 1);
			break;
		}
		executeCommand(cmd, env);
		free(cmd);
	}
	return (0);
}
