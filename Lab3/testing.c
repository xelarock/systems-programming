//
// Created by Alex Welsh on 11/1/20.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <fcntl.h>
#include "tokens.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
int main(int argc, char **argv){
    char * cmdargv;
    char * fgets_return;

    while(1){
        printf("mysh:");
        cmdargv = malloc(1024);                                    // set space for cmd
        do{                                                             // check if fgets fails, call again if interrupt
            fgets_return = fgets(cmdargv, 1024, stdin);
        }while(fgets_return == NULL && errno == EINTR);

        cmdargv[strlen(cmdargv) - 1] = '\0';                            // remove training newline char

        char** cmds = get_tokens(cmdargv, "|");
        int num_cmds = 0;
        while(cmds[num_cmds] != NULL){
            num_cmds++;
        }

        int error = 0;
        char** tokens = get_tokens(cmdargv, " \t\n");
        int counter = 0;
        int input_num = 0, output_num = 0;
        if (strcmp(&cmdargv[strlen(cmdargv) - 1], "|") == 0){
            printf("ambiguous output redirection\n");
        }else{
            for (int i = 0; tokens[i] != NULL; i++){
                if (!error){
                    if (strcmp(tokens[i], "<") == 0){
                        input_num++;
                        if (input_num >= 2){
                            printf("ambiguous input redirection\n");
                            error = 1;
                        }
                    }else if (strcmp(tokens[i], ">") == 0){
                        output_num++;
                        if (output_num >= 2){
                            printf("ambiguous output redirection\n");
                            error = 1;
                        }
                    }else if (strcmp(tokens[i], ">>") == 0){
                        output_num++;
                        if (output_num >= 2){
                            printf("ambiguous output append redirection\n");
                            error = 1;
                        }
                    }else if (strcmp(tokens[i], "|") == 0){
                        output_num++;
                        if (output_num >= 2){
                            printf("ambiguous output redirection\n");
                            error = 1;
                            output_num = 0;
                        }else if (input_num >= 2 ){
                            printf("ambiguous input redirection\n");
                            error = 1;
                            input_num = 0;
                        }else{
                            counter++;
                            input_num = 1;
                            output_num = 0;
                        }
                    }
                }
            }
        }
    }
}

#pragma clang diagnostic pop