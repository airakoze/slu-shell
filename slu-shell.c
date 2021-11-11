/* 
    Group members: Kaitlyn Ashabranner, Andy Irakoze, and Vishnu Sivaprasad
    C Program for a simple command-line interpreter "slush"
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>
#define bufferSize 256

/* 
    Function to execute any command input 
*/
void execute(char input[]){
    char* cmd;
    char* myArgv[17]; // 15 characters + cmd + NULL
    myArgv[0] = strtok(input, " ");
    cmd = myArgv[0];

    if (strcmp(cmd, "cd") == 0){
        // Handling cd dir
        myArgv[1] = strtok(NULL, "\n");
        int retCd = chdir(myArgv[1]);
        if (retCd == -1){
            perror("Error changing directory");
            exit(-1);
        }
    } else {
        int i = 1;
        while(1){
            myArgv[i] = strtok(NULL, " ");
            i++;
            if (myArgv[i] == NULL) {
                break;
            }
        }
        int retExe = execvp(cmd, myArgv);
        if (retExe == -1){
            perror("Program not found");
            exit(-1);
        }
    }
}

/* 
    Checking if a pipeline input is valid 
*/
int isInvalidInput(char* cmd){
    int i = 0;
    while(cmd[i] != '\0'){
        if (!isspace(cmd[i])) return 0;
        i++;
    }
    return 1;
}

/*  
    Pipeline function to run commands in a pipeline 
*/
void pipeline(char input[], int numChild) {
    char* cmds[15];
    cmds[0] = strtok(input, "(");
    int i = 0;
    while(cmds[i] != NULL){
        i++;
        cmds[i] = strtok(NULL, "(");
    }
    
    int pipefd[2];
    int writeEnd;
    int readEnd;
    int oldfd;
    int retNull;
    int retChild;
    while ((i-1) >= 0){

        //Checking if a input is valid
        retNull= isInvalidInput(cmds[i-1]);
        if (retNull == 1 || cmds[i-1] == NULL){
            perror("Invalid Null Command");
            break;
        }
        
        if((i-1) == numChild){
            // First command
            int retPipe = pipe(pipefd);
            if (retPipe == -1){
                perror("Could not pipe");
                exit(-1);
            }
            writeEnd = pipefd[1];
            oldfd = pipefd[0];
        }
        if((i-1) != 0 && (i-1) != numChild){
            // Middle command
            int retPipe = pipe(pipefd);
            if (retPipe == -1){
                perror("Could not pipe");
                exit(-1);
            }
            writeEnd = pipefd[1];
            readEnd = oldfd;
            oldfd = pipefd[0];
        }
        
        retChild = fork();
        if (retChild == 0){
            if ((i-1) == numChild){
                // First command
                int ret1 = dup2( writeEnd, STDOUT_FILENO );
                if( ret1 == -1 ){
                    perror("Could not dup2 write pipe");
                    exit(-1);
                }
                close(readEnd);
            } else if ((i-1) == 0) {
                // Last command
                int ret2 = dup2( oldfd, STDIN_FILENO );
                if( ret2 == -1 ){
                    perror("Could not dup2 read pipe2");
                    exit(-1);
                }
            }  else {
                // Middle command
                int ret1 = dup2( writeEnd, STDOUT_FILENO );
                if( ret1 == -1 ){
                    perror("Could not dup2 write pipe");
                    exit(-1);
                }
                int ret2 = dup2( readEnd, STDIN_FILENO );
                if( ret2 == -1 ){
                    perror("Could not dup2 read pipe2");
                    exit(-1);
                }
                close(readEnd);
            }
            execute(cmds[i-1]);
        } 

        // Parent closing pipe's ends
        if ((i-1) == numChild){
            close(writeEnd);
        } else if ((i-1) == 0) {
            close(oldfd);
        } else {
            close(readEnd);
			close(writeEnd);
        }
        i--;
    }
    waitpid(retChild, NULL, 0);
} 

/* 
    Checking if a input is to be executed as a pipeline
    Returns the number of commands in a pipeline starting from 0 
*/
int isPipeline(char input[]) {
    int count = 0;
    int i = 0;
    while (input[i] != '\n'){
        if (input[i] == '(') count++;
        i++;
    }
    return count;
}

/* 
    Ctrl-C Signal Handling 
*/
void signalHandler(int signum) {}

int main (int argc, char* argv[]) {

    signal(SIGINT, signalHandler);

    char input[bufferSize];
    char* retVal;
    while (1){
        // Prompt
        printf("$ "); 

        retVal = fgets(input, bufferSize, stdin);
        if (retVal == NULL){
            break;
        }

        int retPipe = isPipeline(input);
        strtok(input, "\n");

        if(retPipe == 0){
            // In case there is only 1 command
            pid_t child = fork();
            if (child == 0) execute(input);
            waitpid(child, NULL, 0);
        } else {
            // In case it's a pipeline
            pipeline(input, retPipe);
        }
    }

    return 0;
}