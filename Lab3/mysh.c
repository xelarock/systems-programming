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

typedef struct {                // defines the options structure to hold the options for the program.
    int promptType;             // 0 = no prompt, 1 = default, 2 = custom
    char* promptString;
}Options;

typedef struct {
    char *input_file_name, *output_file_name, **cmd;
}Cmd;

typedef struct {
    char process_location[1024]; // 0 is foreground, 1 is background
    Cmd cmd_list[1024];
}CmdSet;

Options parseArgs(int argc, char *argv[]);
CmdSet parseCommands(char* cmd_line_args);

int main(int argc, char **argv){
    char * cmdargv;
    char * fgets_return;
//    cmdargv[1]=NULL;
    cmdargv = malloc(1024);
    char **cmd_tokens;
    int pid, wpid,status;
    Options opts = parseArgs(argc, argv);

    while(1){
        printf("%s", opts.promptString);
        fgets_return = fgets(cmdargv, 1024, stdin);
        cmdargv[strlen(cmdargv) - 1] = '\0';                            // remove training newline char
//        cmd_tokens = get_tokens(cmdargv);

        CmdSet set_of_commands = parseCommands(cmdargv);
        Cmd input_cmd;
        input_cmd.cmd = get_tokens(cmdargv, " \t\n");
        input_cmd.input_file_name = "";
        input_cmd.output_file_name = "";

        set_of_commands.cmd_list[0] = input_cmd;
        set_of_commands.process_location[0] = 1;



        printf("input: %s, processLoc: %d\n", set_of_commands.cmd_list[0].cmd[0], set_of_commands.process_location[0]);

        if( fgets_return == NULL || strcmp(set_of_commands.cmd_list[0].cmd[0], "exit") == 0){
            if (errno != EINTR){
                printf("Goodbye!\n");
                exit(0);
            }
        }
        pid = fork();
        if (pid == 0 ){
            printf("child process!\n");
//            if( execvp(set_of_commands.cmd_list[0].cmd[0], set_of_commands.cmd_list[0].cmd ) == -1 ) {
//                perror(set_of_commands.cmd_list[0].cmd[0]);
//            }
            exit(-1);
        }else{
            printf("parent before wait\n");
            wait(&status);
            printf("parent after wait\n");
        }
    }
}

CmdSet parseCommands(char* cmd_line_args){
    printf("Command Line: %s\n", cmd_line_args);
    char *background_char;
    background_char = strchr(cmd_line_args, '&');
    printf("background!: %s", background_char);
    if ((strchr(background_char, '&') != NULL) && strcmp(background_char, "&")){
        fprintf(stderr, "Error: \"&\" must be last token on command line\n");
    }else {
        char **pipe_tokens = get_tokens(cmd_line_args, "|");
        for (int i = 0; pipe_tokens[i] != NULL; i++) {
            char **cmd_tokens = get_tokens(pipe_tokens[i], " \t\n");
            printf("token %d: %s, size: %lu %lu\n", i, pipe_tokens[i], sizeof(*cmd_tokens), sizeof(*cmd_tokens[0]));
            for (int j = 0; cmd_tokens[j] != NULL; j++) {
                if (strcmp(cmd_tokens[j], "&") == 0)
                    printf("run in background!\n");
                printf("token cmd %d: %s\n", j, cmd_tokens[j]);
            }
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
    fprintf(stderr, "Error: Usage: %s [prompt]\n", "mysh");
    exit(1);
}
