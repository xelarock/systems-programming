//
// Created by Alex Welsh on 10/30/20.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include "tokens.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

typedef struct {                // defines the options structure to hold the options for the program.
    int promptType;             // 0 = no prompt, 1 = default, 2 = custom
    char* promptString;
}Options;

Options parseArgs(int argc, char *argv[]);

int main(int argc, char **argv){
    char * cmdargv;
//    cmdargv[1]=NULL;
    cmdargv = malloc(1024);
    char **cmd_tokens;
    int pid, wpid,status, input;
    Options opts = parseArgs(argc, argv);

    while(1){
        printf("%s", opts.promptString);
        fgets(cmdargv, 1024, stdin);
        cmdargv[strlen(cmdargv) - 1] = '\0';                            // remove training newline char
        cmd_tokens = get_tokens(cmdargv);
        if( input==EOF || strcmp(cmdargv, "exit") == 0){
            printf("Goodbye!\n");
            exit(0);
        }
        pid = fork();
        if (pid == 0 ){
            printf("child process!\n");
            if( execvp(cmd_tokens[0], cmd_tokens ) == -1 ) {
                perror(cmd_tokens[0]);
            }
            exit(-1);
//            execvp(cmd_tokens[0], cmd_tokens);
        }else{
            printf("parent before wait\n");
            wait(&status);
            printf("parent after wait\n");
        }
    }
}

Options parseArgs(int argc, char *argv[]){
    Options opts;
    if (argc == 1){
        printf("default shell prompt\n");
        opts.promptType = 1;
        opts.promptString = "mysh:";
        return opts;
    }else if (argc == 2){
        if (strcmp(argv[1], "-") == 0){
            printf("no shell prompt\n");
            opts.promptType = 0;
            opts.promptString = "";
            return opts;
        }else {
            printf("custom shell prompt: %s\n", argv[1]);
            opts.promptType = 2;
            opts.promptString = argv[1];
            return opts;
        }
    }
    fprintf(stderr, "Error: Usage: %s [prompt]\n", "myshell");
    exit(1);
}


#pragma clang diagnostic pop