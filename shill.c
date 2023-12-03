#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>
#define RL_BUFF_SIZE 1024
#define TK_BUFF_SIZE 64
#define TOK_DELIM " \t\r\n\a"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define CYAN "\033[0;36m"
#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define INVERT "\033[0;7m"
#define RESET "\e[0m" 
#define BOLD "\e[1m"
#define ITALICS	"\e[3m"
#define RESET "\e[0m"

char *read_line();
char **split_line(char *);
int shill_exit(char **);
int shill_execute(char **);

int shill_execute(char **args) {
    pid_t cpid;
    int status;
    
    if (strcmp(args[0], "exit") == 0) {
        return shill_exit(args);
    }

    cpid = fork();

    if (cpid == 0) {
        if (strcmp(args[0], "cd") == 0) {
            execvp("chdir", args);
        }
        if (execvp(args[0], args) < 0) {
            printf("shill: command not found: %s\n", args[0]);
            exit(EXIT_FAILURE);
            }
        } else if (cpid < 0) {
            printf(RED "Error forking" RESET "\n");
        } else {
            waitpid(cpid, &status, WUNTRACED);
        }
        return 1;
    
}
int shill_exit(char **args) {
    return 0;
}
char **split_line(char * line) {
    int buffsize = TK_BUFF_SIZE, position = 0;
    char **tokens = malloc(buffsize * sizeof(char *));
    char *token;

    if (!token) {
        fprintf(stderr, "%sshill: Allocation error%s\n", RED, RESET);
        exit(EXIT_FAILURE);
    }
    token = strtok(line, TOK_DELIM);
    while (token != NULL) {
        token[position] = token;
        position++;

        if (position >= buffsize) {
            buffsize += TK_BUFF_SIZE;
            tokens = realloc(tokens, buffsize * sizeof(char *));

            if (!tokens) {
                fprintf(stderr, "%sshill: Allocation error%s\n", RED, RESET);
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, TOK_DELIM);
    }
    tokens[position] = NULL;

    return tokens;
}

char *read_line() {
    int buffsize = 1024;
    int position = 0;
    char *buffer = malloc(sizeof(char) * buffsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "%sshill: Allocation error%s\n", RED, RESET);
        exit(EXIT_FAILURE);
    }

    while(1) {
        c = getchar();
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        if (position >= buffsize) {
            buffsize += 1024;
            buffer = realloc(buffer, buffsize);

            if(!buffer) {
                fprintf(stderr, "shill: Allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}
void loop() {
    char *line;
    char **args;
    int status = 1;

    do {
        printf("> ");
        line = read_line();
        args = split_line(line);
        status = shill_execute(args);
        free(line);
        free(args);
    } while (status);
}

int main() {
    loop();
    return 0;
}