#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


char *replaceProgram(char *given_ingerdient, char *given_file_name){
    char* final_command = calloc(200, sizeof(char));
    char* file_path = calloc(100,sizeof(char));
    for (int i = 0; i < strlen(given_file_name); i++) {
        file_path[i] = given_file_name[i];
    }
    int current_file = open(file_path,O_RDONLY);

    char* curr_command = calloc(200, sizeof(char));
    char curr_char;
    int cmd_len = strlen(given_ingerdient);
    int index = 0;

    while(read(current_file,&curr_char,1)==1) {
        if(curr_char != '\n'){
            curr_command[index] = curr_char;
            index++;
        } else{
            int flag = 1;
            for(int i = 0; i < cmd_len-1; i++){
                if(given_ingerdient[i] != curr_command[i]){
                    flag = 0;
                }
            }

            if(flag == 1){
                for(int i = cmd_len+2; i < 200; i++){
                    final_command[i-cmd_len-2] = curr_command[i];
                }
                break;
            }
            memset(curr_command, 0, 200);
            index = 0;
            }

        }

    return final_command;
}

char ** splitCommands(char * line){
    int count = 0;
    char ** args = NULL;
    char delims[2] = {' ', '\n'};
    line = strtok(line, delims);
    while (line){
        count++;
        args = realloc(args, count * sizeof(char*));
        args[count - 1] = line;
        line = strtok(NULL, delims);
    }
    args = realloc(args, (count + 1) * sizeof(char*));
    args[count] = NULL;

    return args;
}


void commandLine(char *single_line){
    int len = strlen(single_line);
    //printf("%s  %d  \n", single_line, len);
    int cmds_num = 0;
    for(int i = 0; i<len; i++){
        if(single_line[i] == '|'){
            cmds_num++;
        }
    }
    int fd[2][2];

    int prev_i = 0;
    int cmds_cntr = -1;
    for(int i = 0; i<len; i++){
        if(single_line[i] == '|'){
            cmds_cntr++;
            int sublen = i-prev_i-1;
            char sub_command[i - prev_i];
            memcpy(sub_command, &single_line[prev_i],sublen);
            sub_command[sublen] = '\0';
            prev_i = i+2;

            char **args = splitCommands(sub_command);

            if(cmds_cntr != 0){
                close(fd[cmds_cntr % 2][0]);
                close(fd[cmds_cntr % 2][1]);
            }

            pipe(fd[cmds_cntr%2]);

            pid_t pid = vfork();
            if(pid == 0) {
                if(cmds_cntr != cmds_num-1){
                    close(fd[cmds_cntr % 2][0]);
                    dup2(fd[cmds_cntr % 2][1], STDOUT_FILENO);
                }

                if(cmds_cntr != 0) {
                    close(fd[(cmds_cntr-1) % 2][1]);
                    dup2(fd[(cmds_cntr-1) % 2][0], STDIN_FILENO);
                }

                execvp(args[0], args);
            }

            /*
            int prev_j = 0;
            for(int j = 0; j<=sublen; j++){
                if(sub_command[j] == ' ' || sub_command[j] == '\0' ){
                    int subsublen = j - prev_j;
                    char sub_sub_command[subsublen+1];
                    memcpy(sub_sub_command, &sub_command[prev_j], subsublen);
                    sub_sub_command[subsublen] = '\0';
                    printf("%s\n", sub_sub_command);
                    prev_j = j+1;
                }
            }
            */

        }
    }
    close(fd[cmds_num % 2][0]);
    close(fd[cmds_num % 2][1]);
}


void complieFromFile(char *given_file_name){

    char* file_path = calloc(100,sizeof(char));

    for (int i = 0; i < strlen(given_file_name); i++) {
        file_path[i] = given_file_name[i];
    }


    int current_file = open(file_path,O_RDONLY);

    //max 3 programy
    char curr_char;
    char* curr_line = calloc(400,sizeof(char));
    char* prog1 = calloc(100,sizeof(char));
    char* prog2 = calloc(100,sizeof(char));
    char* prog3 = calloc(100,sizeof(char));
    int index = 0;

    int after_space_flag = 0;

    while(read(current_file,&curr_char,1)==1) {
        if(curr_char != '\n'){
            curr_line[index] = curr_char;
            index++;
        } else{
            curr_line[index] = curr_char;

            if(after_space_flag){
                int num_of_command = 1;
                int command_ind = 0;
                for(int i = 0; i <= index; i++){
                    if(curr_line[i] == '|'){
                        num_of_command++;
                        command_ind = 0;
                        i++;
                    } else{
                        if(num_of_command == 1){
                            prog1[command_ind] = curr_line[i];
                            command_ind++;
                        }
                        if(num_of_command == 2){
                            prog2[command_ind] = curr_line[i];
                            command_ind++;
                        }
                        if(num_of_command == 3){
                            prog3[command_ind] = curr_line[i];
                            command_ind++;
                        }
                    }
                }

                char to_do[400] = "";
                strcat(to_do, replaceProgram(prog1, given_file_name));

                if(strlen(prog2)>1){
                    strcat(to_do, " | ");
                    strcat(to_do, replaceProgram(prog2, given_file_name));
                }
                if(strlen(prog3)>1){
                    strcat(to_do, " | ");
                    strcat(to_do, replaceProgram(prog3, given_file_name));
                }
                strcat(to_do, " | ");

                printf("%s\n", to_do);
                commandLine(to_do);
                memset(prog1, 0, 100);
                memset(prog2, 0, 100);
                memset(prog3, 0, 100);
            }

            if(index == 0){
               after_space_flag = 1;
            }
            index = 0;
        }
    }
}


int main(int argc, char *argv[]) {
    if(argc != 2){
        printf("Program needs one argument - path to file with commands!\n");
        return 0;
    }
    const char* file_name = argv[1];

    complieFromFile((char*)file_name);

    return 0;
}