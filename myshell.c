#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

int fd, std, redirection;
char storage1[50001], storage2[50001], *pth;
FILE* adv;

void myPrint(char *msg)
{
    write(STDOUT_FILENO, msg, strlen(msg));
}

char error_message[25] = "An error has occurred\n";

char *removeSpace(char *str)
{
    int i, counter = 0, len = strlen(str);
    char *result = (char *)malloc(sizeof(char)*len);
    for (i = 0; i < len; i++) {
        if (str[i] == ' ')
            i++;
        else {
            result[counter] = str[i];
            counter++;
        }
    }
    
    result[counter] = '\0';
    return result;
}

char **indToken(char *str)
{
    int i = 0;
    char **result = malloc(sizeof(char *)* 1000);
    char *tmp = strtok(str, " ");
    while (tmp != NULL) {
        result[i] = strdup(tmp);
        i++;
        tmp = strtok(NULL, " ");
    }
    result[i] = NULL;
    
    if(result[0] == NULL){
        result[0] = strdup(" ");
        result[1] = NULL;
    }
        
    
    return result;
}

//function that parses commands + arguments = token, lying between semi-colons.
char  **parseToken(char *str){
    int size = 1000;
    int i = 0/*, j = 0*/;
    char **tokens = malloc(sizeof(char *) * size);
    char *tmp = strtok(str, ";");
    while (tmp != NULL) {
        tokens[i] = strdup(tmp);
        i++;
        tmp = strtok(NULL, ";");
    }
    tokens[i] = NULL;
    return tokens;
    
}

//removes tabs and new lines
char *addSpace(char *str)
{
    int index = 0;
    while (str[index] != '\0') {
        if (str[index] == '\t' || str[index] == '\n') {
            str[index] = ' ';
        }
        index++;
    }
    return str;
}

int redirectCheck (char *cmd) {
    int i, len, result = 0, c=0;
    len = strlen(cmd);
    for(i=0; i < len; i++) {
        if(cmd[i] == '>') {
            c++;
        }
    }
    if(c>1) {
        return -1;
    }
    for (i = 0; i < len; i++) {
        if (cmd[i] == '>') {
            if (cmd[i+1] == '+') {
                result = 2;
                break;
            }
            else {
                result = 1;
                break;
            }
        }
    }
    return result;
}

int checkSpace (char *line)
{
    //int len = strlen(line);
    int i;
    for (i = 0; line[i]; i++) {
        if(line[i] != ' ' && line[i] != '\t' && line[i] != '\n')
            //myPrint("returns 1");
            return 1;
    }
    //myPrint("iuoiu");
    return 0;
}

//function that checks if token has redirections (>, >+)
char **redirect(char *cmd){
    int counter = 0;
    char **result = (char **)malloc(sizeof(char *)*3);
    char *tmp = strtok(cmd, ">");
    //char *tmpResult0, *tmpResult1;
    while (tmp != NULL) {
        result[counter] = strdup(tmp);
        counter++;
        tmp = strtok(NULL, ">");
    }
    result[counter] = NULL;
    if (result[2] != NULL) {
        write(STDOUT_FILENO, error_message, strlen(error_message));
    }
    
    while(result[1][0] == ' ') {
        result[1]++;
    }
    
    for(counter = 0; result[1][counter]; counter++) {
        if (result[1][counter] == ' ' || result[1][counter] == '\t' || result[1][counter] == '\n') {
            result[1][counter] = '\0';
        }
    }
    
    //tmpResult0 = strdup(result[0]);
    //result[0] = removeSpace(tmpResult0);
    //tmpResult1 = strdup(result[1]);
    //result[1] = removeSpace(tmpResult1);
    
    //free(tmpResult0);
    //free(tmpResult1);
    //free(tmp);
    return result;
}

char **advredirect(char* cmd) {
    int counter = 0;
    char **result = (char **)malloc(sizeof(char *)*3);
    char *tmp = strtok(cmd, ">");
    //char *tmpResult0, *tmpResult1;
    while (tmp != NULL) {
        result[counter] = strdup(tmp);
        counter++;
        tmp = strtok(NULL, ">");
    }
    result[counter] = NULL;
    if (result[2] != NULL) {
        write(STDOUT_FILENO, error_message, strlen(error_message));
    }
    
    result[1]++;
    
    while(result[1][0] == ' ') {
        result[1]++;
    }
    
    for(counter = 0; result[1][counter]; counter++) {
        if (result[1][counter] == ' ' || result[1][counter] == '\t' || result[1][counter] == '\n') {
            result[1][counter] = '\0';
        }
    }
    
    //myPrint(result[0]);
    //myPrint("\n");
    //myPrint(result[1]);
    //myPrint("\n");
    
    //tmpResult0 = strdup(result[0]);
    //result[0] = removeSpace(tmpResult0);
    //tmpResult1 = strdup(result[1]);
    //result[1] = removeSpace(tmpResult1);
    
    //free(tmpResult0);
    //free(tmpResult1);
    //free(tmp);
    return result;

}

int built_in(char **token, int redirection)
{
    pid_t pid;
    int status;
   // in main char **token = indToken(tokens[i]);
    if (!strcmp(token[0], "exit")){
        if (redirection) {
            myPrint(error_message);
            return 1;
        }
        if (token[1] == NULL) {
            exit(0);
        } else {
            myPrint(error_message);
        }
        return 1;
    }
    if (!strcmp(token[0], "cd")) {
        if (redirection) {
            myPrint(error_message);
            return 1;
        }
        if (token[1] == NULL){
            chdir(getenv("HOME"));
        } else if (token[2] ==  NULL){
            if (chdir(token[1]))
                myPrint(error_message);
        } else {
            myPrint(error_message);
        }
        return 1;
    }
    if (!strcmp(token[0], "pwd")) {
        if (redirection) {
            myPrint(error_message);
            return 1;
        }
        if (token[1] == NULL) {
            char buff[1000];
            char *output = getcwd(buff, sizeof(buff));
            if (output == NULL)
                write(STDOUT_FILENO, error_message, strlen(error_message));
            else {
                if ((pid = fork()) == 0) {
                    execvp(token[0], token);
                }
                waitpid(pid, &status, 0);
            }
        } else {
            myPrint(error_message);
        }
        return 1;
    }
    
    if (!strcmp(token[0], "&")) {
        return 1;
    }
    
        
    return 0;
        
}
        
void shell(char *command)
{
    pid_t pid;
    int status;
    char *tmpCmd;
    char **afterRedirect, **afterSpaces;
    
    tmpCmd = strdup(command);
    //myPrint(tmpCmd);
    redirection = redirectCheck(command);
    if (redirection == -1) {
        myPrint(error_message);
        return;
    }
    if (redirection) {
        if(redirection == 2) {
            afterRedirect = advredirect(tmpCmd);
            afterSpaces = indToken(afterRedirect[0]);
        } else {
            afterRedirect = redirect(tmpCmd);
            afterSpaces = indToken(afterRedirect[0]);
            //msyPrint(afterRedirect[0]);
            //myPrint("\n");
            //myPrint(afterRedirect[1]);
        }
        pth = strdup(afterRedirect[1]);
        
    } else {
        afterSpaces = indToken(command);
    }
    //myPrint(afterSpaces[0]);
    if(!built_in(afterSpaces, redirection)) {
        if((pid = fork()) == 0) {
            if (redirection>0) {
                if(redirection == 1) {
                    //regular redirection
                    fd = open(afterRedirect[1], O_CREAT|O_WRONLY|O_EXCL, 0666);
                    if (fd == -1) {
                        myPrint(error_message);
                        return;
                    }
                    std = dup(STDOUT_FILENO);
                    dup2(fd, STDOUT_FILENO);
                } else if (redirection == 2){
                    redirection = 3;
                    adv = fopen(afterRedirect[1],"r");
                    if(adv) {
                        fread(storage1, 50000, 1, adv);
                        fclose(adv);
                    } else {
                        storage1[0] = '\0';
                    }
                    fd = open(afterRedirect[1], O_CREAT|O_TRUNC|O_WRONLY, 0666);
                    std = dup(STDOUT_FILENO);
                    dup2(fd, STDOUT_FILENO);
                } else
                    myPrint(error_message);
            }

            if(!strcmp(afterSpaces[0], " ")){
                exit(0);
            }
            
            if (execvp(afterSpaces[0], afterSpaces)){
                    write(STDOUT_FILENO, error_message, strlen(error_message));
                    exit(0);
            }
        }
        waitpid(pid, &status, 0);
    }
    /*free(tmpCmd);
    for (i = 0; afterRedirect[i] != NULL; i++) {
        free(afterRedirect[i]);
    }
    for (j = 0; afterRedirect[j] != NULL; j++) {
        free(afterSpaces[j]);
    }*/
    
}
        
        
        
int main(int argc, char *argv[])
{
    redirection = 0;
    FILE *f = stdin;
    char cmd_buff[1500];
    if (argc > 2) {
        myPrint(error_message);
        exit(0);
    }
    if (argc == 2) {
        f = fopen(argv[1], "r");
        if (f == NULL) {
            myPrint(error_message);
            exit(0);
        }
    }
    
        while (1) {
            if (argc == 1)
                myPrint("myshell> ");
            char *dup;
            
            if(!fgets(cmd_buff, 1500, f)) {
                exit(0);
            }
            if (strlen(cmd_buff) > 513){
                myPrint(cmd_buff);
                myPrint(error_message);
                continue;
            }
            if (cmd_buff[0]) {
                    dup = strdup(cmd_buff);
                    if (checkSpace(dup)){
                        myPrint(cmd_buff);
                    }
                    char *newLine1 = addSpace(cmd_buff);//removes tabs and new lines
                    char **commandTokens1 = parseToken(newLine1); //removes semi colons, divides into tokens
                    int i = 0;
                    while (commandTokens1[i] != NULL) {
                        redirection = 0;
                        shell(commandTokens1[i]);
                        i++;
                    }
                    if (redirection) {
                        close(fd);
                        dup2(std,1);
                        if(redirection == 3) {
                            adv = fopen(pth,"r");
                            fread(storage2,50000,1,adv);
                            fclose(adv);
                            fd = open(pth,O_TRUNC|O_WRONLY,0666);
                            dup2(fd,1);
                            myPrint(storage2);
                            myPrint(storage1);
                            close(fd);
                            dup2(std,1);
                            
                        }
                    }
                    free(commandTokens1);
            }
            
        }
    return 0;
}


