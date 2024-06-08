#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>


void command_handler(char *args[],char is_background) {

    pid_t pid = fork();

    if(pid == 0){
        execvp(args[0], args);
        exit(0);
    }else{
        
        if(is_background == 0 && waitpid(pid, NULL, 0) == -1){
            printf("Some error occured\n");
        }
        
    }



}`


void remove_quotes(char *str) {
    int i, j;
    for (i = 0, j = 0; str[i] != '\0'; i++) {
        if (str[i] != '"') {
            str[j] = str[i];
            j++;
        }
    }
    str[j] = '\0'; // Terminate the string after removing quotes
}



void handle_change_dir(char *path){
    if(chdir(path) != 0){
        printf("Some error occured\n");
    }
}

void handle_export(char *key, char *value){
    setenv(key, value, 1);
}



void handle_echo(char *input){
    char output[100] = "";
    int index = 0;



    for(int i=0; i < strlen(input);i++){
        if(input[i] == '$'){
            i++;
            int j=0;
            char variable[10];
            while(input[i] != '\0' && input[i] != ' '){
                variable[j++] = input[i];
                i++;
                
            }

            variable[j] = '\0';
            char* value = getenv(variable);

            
            if(value == NULL){
                strcpy(&output[index], "undefined");
                index += strlen("undefined");
            }else{
                strcpy(&output[index], value);
                index += strlen(value);
            }
            
        }else{
            output[index++] = input[i];
        }

    }

    printf("%s\n", output);
}

int main() {
    while(1){
        char command[100];
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;
        char* token = strtok(command, " ");
   
        if(strcmp(token, "exit") == 0){
            break;
        }else if(strcmp(token, "cd") == 0){
            char* path = strtok(NULL, " ");
            handle_change_dir(path);
     
        }else if(strcmp(token, "export") == 0){
            char* key = strtok(NULL, "=");
            char* value = strtok(NULL, "=");

            remove_quotes(value);
            handle_export(key, value);
            
        }else if(strcmp(token, "echo") == 0){
            char* input = strtok(NULL, "");
            remove_quotes(input);
            handle_echo(input);

        }else{
            char* args[10];
            args[0] = token;
            char is_background = 0;
            char* temp = strtok(NULL, " ");
            if(temp != NULL){
                is_background = strcmp(temp,"&") == 0;
                char index = 1;
                if(is_background == 0){
                    args[index++] = temp;
                }
                temp = strtok(NULL, " ");
                while(temp != NULL){
                    args[index++] = temp;
                    temp = strtok(NULL, " ");
                }
                args[index] = NULL;
            }else{
                args[1] = NULL;
            }


            command_handler(args, is_background);
        }

    }
    printf("have a nice day!\n");

    return 0;
}
