# Simple Shell

A simple UNIX command line interpreter written in C as part of the ALX Software Engineering program.

## Description

This project implements a basic shell that can execute commands, handle arguments, search the PATH, and provide built-in commands. The shell operates in both interactive and non-interactive modes, mimicking the behavior of `/bin/sh`.

## Compilation

```bash
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh
```

## Usage

### Interactive Mode
```bash
$ ./hsh
$ /bin/ls
file1 file2 file3
$ ls -l
total 8
-rwxr-xr-x 1 user user 8760 Feb 15 10:30 hsh
$ exit
```

### Non-Interactive Mode
```bash
$ echo "/bin/ls" | ./hsh
file1 file2 file3
$ echo -e "/bin/ls\n/bin/pwd" | ./hsh
file1 file2 file3
/home/user
```

## Project Tasks

### Task 0: Betty would be proud
**Objective**: Write beautiful code that passes Betty style checks.
- Ensure all code follows Betty coding standards
- Proper indentation, spacing, and documentation

### Task 1: Simple shell 0.1
**Objective**: Write a basic UNIX command line interpreter.

**Features**:
- Display prompt `#cisfun$ `
- Execute single-word commands with absolute paths
- Handle "end of file" condition (Ctrl+D)
- Display error messages for non-existent commands
- No PATH resolution, arguments, or built-ins

**Functions Used**:
- `getline()` - Read command input
- `fork()` - Create child process  
- `execve()` - Execute commands
- `waitpid()` - Wait for child process
- `write()` - Output text and errors

### Task 2: Simple shell 0.2
**Objective**: Add support for command line arguments.

**Features**:
- Handle commands with multiple arguments
- Parse command lines using spaces as delimiters
- Dynamic memory allocation for argument arrays

**Additional Functions Used**:
- `strtok()` - Parse command line into tokens
- `malloc()` - Allocate memory for arguments
- `free()` - Free allocated memory

### Task 3: Simple shell 0.3
**Objective**: Implement PATH resolution and avoid unnecessary forking.

**Features**:
- Search for commands in PATH directories
- Use both absolute paths and command names
- Check command existence before forking
- Prompt changed to `:) `

**Additional Functions Used**:
- `stat()` - Check if file exists
- `strncmp()` - Compare PATH prefix
- `strchr()` - Find '/' in command
- `strcat()` - Build full command paths

### Task 4: Simple shell 0.4
**Objective**: Implement the `exit` built-in command.

**Features**:
- `exit` command to terminate shell
- Built-in command detection and handling
- Clean memory management before exit

**Additional Functions Used**:
- `strcmp()` - Compare command strings for built-ins

### Task 5: Simple shell 1.0
**Objective**: Implement the `env` built-in command.

**Features**:
- `env` command to display environment variables
- Print all environment variables line by line
- Prompt changed to `$ `

## Function Prototypes

```c
/* Core Shell Functions */
void printPrompt(void);                          /* Display shell prompt */
char *readCommand(void);                         /* Read user input */
int executeCommand(char *cmd, char **env);       /* Execute commands and built-ins */

/* Command Parsing */
int count_tokens(char *str);                     /* Count arguments in command */
char **parse_command(char *cmd);                 /* Parse command into argument array */
void free_argv(char **argv);                     /* Free argument array memory */

/* PATH Resolution */
char *get_path(char **env);                      /* Extract PATH from environment */
char *find_command(char *cmd, char **env);       /* Find command in PATH directories */

/* Built-in Commands */
int is_builtin(char *cmd);                       /* Check if command is built-in */
int handle_builtin(char **argv, char **env);     /* Execute built-in commands */

/* Main Function */
int main(int ac, char **av, char **env);         /* Main shell loop */
```

## Allowed Functions and System Calls

| Function | Purpose |
|----------|---------|
| `access` | Check file permissions |
| `chdir` | Change directory |
| `close` | Close file descriptor |
| `closedir` | Close directory stream |
| `execve` | Execute program |
| `exit` | Exit program |
| `fflush` | Flush stream buffer |
| `fork` | Create child process |
| `free` | Free allocated memory |
| `getcwd` | Get current directory |
| `getline` | Read line from stream |
| `getpid` | Get process ID |
| `isatty` | Test if file descriptor is terminal |
| `kill` | Send signal to process |
| `malloc` | Allocate memory |
| `open` | Open file |
| `opendir` | Open directory stream |
| `perror` | Print error message |
| `read` | Read from file descriptor |
| `readdir` | Read directory entry |
| `signal` | Handle signals |
| `stat` | Get file status |
| `lstat` | Get symbolic link status |
| `fstat` | Get file status by descriptor |
| `strtok` | Parse string tokens |
| `wait` | Wait for child process |
| `waitpid` | Wait for specific child |
| `wait3` | Wait with resource usage |
| `wait4` | Wait with options |
| `write` | Write to file descriptor |

## File Structure

```
simple_shell/
├── README.md           # Project documentation
├── AUTHORS            # Contributors list
├── main.c             # Main shell loop
├── shell.c            # Core shell functions
├── builtin.c          # Built-in command handlers
├── path.c             # PATH resolution functions
├── parser.c           # Command parsing functions
├── utils.c            # Utility functions
└── shell.h            # Header file with prototypes
```

## Error Handling

The shell handles various error conditions:
- Command not found: `./hsh: 1: command: not found`
- Fork failure: Uses `perror()` to display system error
- Memory allocation failure: Returns appropriate error codes
- EOF condition: Cleanly exits shell

## Built-in Commands

### `exit`
Exits the shell cleanly.
```bash
$ exit
```

### `env`
Prints all environment variables.
```bash
$ env
USER=username
HOME=/home/username
PATH=/usr/bin:/bin
...
```

## Memory Management

The shell implements proper memory management:
- All `malloc()` calls have corresponding `free()` calls
- Dynamic argument arrays are properly deallocated
- No memory leaks in normal operation
- Clean exit procedures free all allocated memory

## Authors

 - 🎓 42 Intra: [eel-garo](https://profile.intra.42.fr/users/eel-garo)
 - 🐙 GitHub: [MEHDIJAD](https://github.com/MEHDIJAD/)
 - 💼 LinkedIn: [Connect with me](https://www.linkedin.com/in/el-mehdi-el-garouaz-a028aa287/)

## Repository

GitHub repository: [simple_shell](https://github.com/your-username/simple_shell)

---

*This project is part of the ALX Software Engineering program curriculum.*