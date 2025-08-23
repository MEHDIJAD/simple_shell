# Notes

## Everything you need to know to start coding your own shell 

### 1- getline

```c
ssize_t getline(char ** restrict linep, size_t * restrict linecapp, FILE * restrict stream);
```

### 2- execve

#### SYNOPSIS
```c
    #include <unistd.h>

    int execve(const char *pathname, char *const _Nullable argv[], char *const _Nullable envp[]);
```

#### DESCRIPTION
    execve() executes the program referred to by pathname.  This
    causes the program that is currently being run by the calling
    process to be replaced with a new program, with newly initialized
    stack, heap, and (initialized and uninitialized) data segments.





