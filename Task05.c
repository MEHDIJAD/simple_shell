#include <stddef.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>

/**
 * print_prompt - prints the shell prompt
 */
void printPrompt(void)
{
	write(STDOUT_FILENO, "\033[1;34m$\033[0m ", strlen("\033[1;34m$\033[0m "));
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
 * get_path - gets the PATH environment variable
 * @env: environment variables
 * Return: PATH string or NULL
 */
char *get_path(char **env)
{
	int i = 0;
	
	while (env[i])
	{
		if (strncmp(env[i], "PATH=", 5) == 0)
			return (env[i] + 5);
		i++;
	}
	return (NULL);
}

/**
 * find_command - finds the full path of a command
 * @cmd: command to find
 * @env: environment variables
 * Return: full path to command or NULL if not found
 */
char *find_command(char *cmd, char **env)
{
	char *path, *path_copy, *dir;
	char *full_path;
	struct stat st;
	
	/* If command contains '/', it's already a path */
	if (strchr(cmd, '/'))
	{
		if (stat(cmd, &st) == 0)
			return (cmd);
		return (NULL);
	}
	
	path = get_path(env);
	if (!path)
		return (NULL);
	
	path_copy = malloc(strlen(path) + 1);
	if (!path_copy)
		return (NULL);
	
	strcpy(path_copy, path);
	dir = strtok(path_copy, ":");
	
	while (dir != NULL)
	{
		full_path = malloc(strlen(dir) + strlen(cmd) + 2);
		if (!full_path)
		{
			free(path_copy);
			return (NULL);
		}
		
		strcpy(full_path, dir);
		strcat(full_path, "/");
		strcat(full_path, cmd);
		
		if (stat(full_path, &st) == 0)
		{
			free(path_copy);
			return (full_path);
		}
		
		free(full_path);
		dir = strtok(NULL, ":");
	}
	
	free(path_copy);
	return (NULL);
}

/**
 * is_builtin - checks if command is a built-in
 * @cmd: command to check
 * Return: 1 if built-in, 0 otherwise
 */
int is_builtin(char *cmd)
{
	if (strcmp(cmd, "exit") == 0)
		return (1);
	if (strcmp(cmd, "env") == 0)
		return (1);
	return (0);
}

/**
 * handle_builtin - handles built-in commands
 * @argv: argument array
 * @env: environment variables
 * Return: 1 to continue shell, 0 to exit shell
 */
int handle_builtin(char **argv, char **env)
{
	int i;
	
	if (strcmp(argv[0], "exit") == 0)
	{
		return (0); /* Exit the shell */
	}
	if (strcmp(argv[0], "env") == 0)
	{
		/* Print all environment variables */
		i = 0;
		while (env[i])
		{
			write(STDOUT_FILENO, env[i], strlen(env[i]));
			write(STDOUT_FILENO, "\n", 1);
			i++;
		}
		return (1); /* Continue shell */
	}
	return (1); /* Continue shell */
}

/**
 * executeCommand - executes a command with arguments
 * @cmd: the command line string
 * @env: environment variables
 * Return: 1 to continue shell, 0 to exit shell
 */
int executeCommand(char *cmd, char **env)
{
	pid_t pid;
	int status;
	char **argv;
	char *command_path;
	
	/* Skip empty commands */
	if (strlen(cmd) == 0)
		return (1);
	
	argv = parse_command(cmd);
	if (!argv)
		return (1);
	
	/* Check if it's a built-in command */
	if (is_builtin(argv[0]))
	{
		int result = handle_builtin(argv, env);
		free_argv(argv);
		return (result);
	}
	
	/* Find the command in PATH */
	command_path = find_command(argv[0], env);
	if (!command_path)
	{
		write(STDERR_FILENO, "./shell: No such file or directory\n", 35);
		free_argv(argv);
		return (1);
	}
	
	pid = fork();
	if (pid == -1){
		perror("fork");
		if (command_path != argv[0])
			free(command_path);
		free_argv(argv);
		return (1);
	}
	
	if (pid == 0) {
		/* Child process */
		if (execve(command_path, argv, env) == -1){
			write(STDERR_FILENO, "./shell: No such file or directory\n", 35);
			exit(EXIT_FAILURE);
		}
	}
	else {
		/* Parent process - wait for child */
		waitpid(pid, &status, 0);
	}
	
	if (command_path != argv[0])
		free(command_path);
	free_argv(argv);
	return (1);
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
		if (executeCommand(cmd, env) == 0) {
			free(cmd);
			break;
		}
		free(cmd);
	}
	return (0);
}
