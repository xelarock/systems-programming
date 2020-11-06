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
#include <stdbool.h>
#include <fcntl.h>
#include "tokens.h"


typedef struct {                // defines the options structure to hold the options for the program.
    int promptType;             // 0 = no prompt, 1 = default, 2 = custom
    char* promptString;
}Options;

typedef struct {
    bool append;
    char *input_file_name, *output_file_name, **cmd;

}Cmd;

typedef struct {
    bool process_location; // true is foreground, false is backgrounf
    Cmd *cmd_list;
    int num_commands;
}CmdSet;

Options parseArgs(int argc, char *argv[]);
CmdSet parseCommands(char* cmd_line_args);
char** removeOperators(char **tokens, char *operator, char* filename);
int error = 0;

int main(int argc, char **argv){
    char * cmdargv;
    char * fgets_return;
    cmdargv = malloc(1024);
    char **cmd_tokens;
    int pid, wpid,status;
    Options opts = parseArgs(argc, argv);

    printf("\n%s", "opts.promptString");
    while(1){
        printf("\n%s!!", opts.promptString);
        fgets_return = fgets(cmdargv, 1024, stdin);
        cmdargv[strlen(cmdargv) - 1] = '\0';                            // remove training newline char
//        cmd_tokens = get_tokens(cmdargv);

        error = 0;
        CmdSet set_of_commands = parseCommands(cmdargv);
        for (int i = 0; i < set_of_commands.num_commands; i++){
            printf("command %d\n", i);
            for (int j = 0; set_of_commands.cmd_list[i].cmd[j] != NULL; j++)
                printf("Cmd: %s, %s, %s\n", set_of_commands.cmd_list[i].cmd[j],  set_of_commands.cmd_list[i].input_file_name,
                       set_of_commands.cmd_list[i].output_file_name);
        }

        if( fgets_return == NULL || strcmp(set_of_commands.cmd_list[0].cmd[0], "exit") == 0){
            if (errno != EINTR){
                printf("Goodbye!\n");
                exit(0);
            }
        }
        if (error == 0){
            pid = fork();
            if (pid == 0 ){
//            printf("child process!**************************** pid: %d, ppid: %d\n", getpid(), getppid());
                int output_fd;
                printf("Running command with output file: %s\n", set_of_commands.cmd_list[0].output_file_name);
                printf("output truth: %d\n", strcmp(set_of_commands.cmd_list[0].output_file_name, ""));
                if (strcmp(set_of_commands.cmd_list[0].output_file_name, "") != 0){
                    output_fd = open(set_of_commands.cmd_list[0].output_file_name, O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP |
                                                                                            S_IROTH);
                    int dup_out = dup2(output_fd, 1);
                    fprintf(stderr, "dup out: %d\n", dup_out);
                    close(dup_out);
                }
                if( execvp(set_of_commands.cmd_list[0].cmd[0], set_of_commands.cmd_list[0].cmd ) == -1 ) {
                    perror(set_of_commands.cmd_list[0].cmd[0]);
                }
                exit(-1);
            }else{
//            struct rusage ru;
//            printf("parent before wait\n");
//            printf("boolean: %d\n", set_of_commands.process_location);
                if (set_of_commands.process_location){
//                printf("waiting for: %d, pid: %d, ppid: %d\n", pid, getpid(), getppid());
                    wait(&status);
                }
//            printf("parent after wait************************\n");
            }
        }

    }
}

CmdSet parseCommands(char* cmd_line_args){
    CmdSet set_of_commands;
//    printf("Command Line: %s\n", cmd_line_args);
    char *background_char;
    background_char = strchr(cmd_line_args, '&');
//    printf("background!: %s\n", background_char);

    if (strcmp(&cmd_line_args[strlen(cmd_line_args)-1], "&") == 0) {
//        printf("run in background!\n");
        cmd_line_args[strlen(cmd_line_args)-1] = '\0';
        set_of_commands.process_location = false;
    }else{
        set_of_commands.process_location = true;
    }

    if ((strchr(cmd_line_args, '&') != NULL) && strcmp(background_char, "&")){
        fprintf(stderr, "Error: \"&\" must be last token on command line\n");
        error = 1;
    }else {
        char **pipe_tokens = get_tokens(cmd_line_args, "|");
        int num_cmds;
//        set_of_commands.cmd_list = malloc(sizeof(Cmd));

        int counter = 0;
        while (pipe_tokens[counter] != NULL)                            // number of commands
            counter++;
        printf("Number of commands!: %d\n", counter);
        set_of_commands.cmd_list = malloc(counter * sizeof(Cmd));

        for (num_cmds = 0; pipe_tokens[num_cmds] != NULL; num_cmds++) {

            char **cmd_tokens = get_tokens(pipe_tokens[num_cmds], " \t\n");
            Cmd new_cmd;
            new_cmd.append = false;
            new_cmd.input_file_name = malloc(1024 * sizeof(char));
            new_cmd.output_file_name = malloc(1024 * sizeof(char));
            new_cmd.cmd = cmd_tokens;
            printf("hello!\n");
            for (int num_args = 0; new_cmd.cmd[num_args] != NULL; num_args++) {
                printf("token cmd %d: %s\n", num_args, cmd_tokens[num_args]);

                if (strcmp(new_cmd.cmd[num_args], ">") == 0) {
                    printf("output name %s\n", new_cmd.output_file_name);
                    if (new_cmd.cmd[num_args + 1] == NULL) {
                        fprintf(stderr, "Error: Missing filename for output redirection.\n");
                        error = 1;
                    } else if (strcmp(new_cmd.output_file_name, "") != 0) {
                        fprintf(stderr, "Error: Ambiguous output redirection.\n");
                        error = 1;
                    } else {
                        printf("Output File %s\n", new_cmd.cmd[num_args + 1]);
                        new_cmd.output_file_name = new_cmd.cmd[num_args + 1];
                        new_cmd.cmd = removeOperators(new_cmd.cmd, ">", new_cmd.output_file_name);
//                        printf("result: %s %s %s\n", new_cmd.cmd[0], new_cmd.cmd[1], new_cmd.cmd[2]);
                        num_args -= 1;
                    }
                }

//                printf("check here! %d\n", num_args);
                if (strcmp(new_cmd.cmd[num_args], "<") == 0) {
                    printf("input name %s\n", new_cmd.input_file_name);
                    if (new_cmd.cmd[num_args + 1] == NULL) {
                        fprintf(stderr, "Error: Missing filename for input redirection.\n");
                        error = 1;
                    } else if (strcmp(new_cmd.input_file_name, "") != 0) {
                        fprintf(stderr, "Error: Ambiguous input redirection.\n");
                        error = 1;
                    } else {
                        printf("input File %s\n", new_cmd.cmd[num_args + 1]);
                        new_cmd.input_file_name = new_cmd.cmd[num_args + 1];
                        new_cmd.cmd = removeOperators(new_cmd.cmd, "<", new_cmd.input_file_name);
                        num_args -= 1;
                    }
                }

//                printf("check before append\n");
                if (strcmp(new_cmd.cmd[num_args], ">>") == 0) {
                    printf("output append name %s\n", new_cmd.output_file_name);
                    if (new_cmd.cmd[num_args + 1] == NULL) {
                        fprintf(stderr, "Error: Missing filename for output redirection.\n");
                        error = 1;
                    } else if (strcmp(new_cmd.output_file_name, "") != 0) {
                        fprintf(stderr, "Error: Ambiguous output redirection.\n");
                        error = 1;
                    } else {
                        new_cmd.output_file_name = new_cmd.cmd[num_args + 1];
                        new_cmd.append = true;
                        printf("Output append File %s, %d\n", new_cmd.output_file_name, new_cmd.append);
                        new_cmd.cmd = removeOperators(new_cmd.cmd, ">>", new_cmd.output_file_name);
                        num_args -= 1;
                    }
                }

            }
            set_of_commands.cmd_list[num_cmds] = new_cmd;
            set_of_commands.num_commands = counter;
        }
    }
    return set_of_commands;
}

char** removeOperators(char **tokens, char *operator, char* filename){
    int counter = 0;
    while(tokens[counter] != NULL){
        counter++;
    }

//    printf("counter out: %d\n", counter);
    char **cmd_tokens = (char**) malloc((counter - 1) * sizeof(char*));

    int offset = 0;
    for (int i = 0; tokens[i] != NULL; i++){
        if ((offset != 2) && ((strcmp(tokens[i], operator) == 0) ||  (strcmp(tokens[i], filename) == 0))) {
//            printf("removed token!: %s\n", tokens[i]);
            offset++;
        }else{
//            printf("saved token!: %s\n", tokens[i]);
            cmd_tokens[i - offset] = tokens[i];
        }
    }
//    printf("done with that\n");
    cmd_tokens[counter-offset] = NULL;
    return cmd_tokens;
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
