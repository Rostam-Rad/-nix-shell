#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define inputSize 1024

int main(int argc, char *argv[]){
    /* Checks to make sure the program takes no arguments when it first executes.*/
    if (argc > 1){
        fprintf(stderr, "ERROR: shell program should take no arguments. You entered:\n");
	int i;
	for (i = 0; i < argc; i++){
	    printf("%s ", argv[i]);
	}
	printf("\n");
        return 1;
    }
    /* Displays the Linux menu */
    char* input = malloc(inputSize);
    char* userInput[inputSize/2 +1];
    int exitCommand = 1;
    int returnValue = 0;
    do{
	flush_input(stdout);
	printf("$ ");
	fgets(input, inputSize, stdin);
	if (strcmp(input, "exit/n") == 0){
	    exitCommand = 0;
	}else{
	    input[strlen(input) - 1] = '\0';
	    char* temp;
            temp = unescape(input, stderr);
	    userInput[0] = strtok(temp, " ");
	    int i = 0;
	    while (userInput[i] != NULL){
		userInput[++i] = strtok(NULL, " ");
	    }
	    if(strcmp(userInput[0], "exit") == 0){
		if (userInput[1]){
		    char* oldUserInput = malloc(inputSize);
                    strcpy(oldUserInput, userInput[1]);
		    char* token = strtok(oldUserInput, "");
		    returnValue = atoi(token);
		    free(oldUserInput);
		}
		free(temp);
		exitCommand = 0;
	    }else if(strcmp(userInput[0], "proc") == 0){
		if(userInput[1]){
	            if(strcmp(userInput[1], "cpuinfo") ==0){
		        FILE* cpuinfo = fopen("/proc/cpuinfo", "rb");
		        char* arg = 0;
		        size_t size = 0;
		        while(getdelim(&arg, &size, 0, cpuinfo) != -1){
			    puts(arg);
		        }
		        free(arg);
		        free(temp);
		        fclose(cpuinfo);
		    }else if(strcmp(userInput[1], "loadavg") ==0){
                        FILE* loadavg = fopen("/proc/loadavg", "rb");
                        char* arg = 0;
                        size_t size = 0;
                        while(getdelim(&arg, &size, 0, loadavg) != -1){
                            puts(arg);
                        }
                        free(arg);
		        free(temp);
                        fclose(loadavg);
		    }else{
	                char* oldUserInput = malloc(inputSize);
                        strcpy(oldUserInput, userInput[1]);
                        char* token = strtok(oldUserInput, "/");
                        char* pid = malloc(inputSize);
                        char* dest = malloc(inputSize);
                        strcpy(pid, "/proc/");
                        strcat(pid, token);
                        strcat(pid, "/");
                        token = strtok(NULL, " ");
                        strcpy(dest, token);
                        strcat(pid,dest);
                        free(oldUserInput);
		        if(strcmp(dest, "status") ==0){
		            FILE* status = fopen(pid, "rb");
			    if (status == NULL){
                                printf("%s : No such file in directory./n", pid);
			        fflush(stdout);
			    }else{
                                char* arg = 0;
                                size_t size = 0;
                                while(getdelim(&arg, &size, 0, status) != -1){
                                    puts(arg);
                                }
                                free(arg);
			        fclose(status);
			    }
			    free(temp);
                            free(pid);
                            free(dest);
                        }else if(strcmp(dest, "environ") ==0){
			    FILE* environ = fopen(pid, "rb");
                            if (environ == NULL){
                                printf("%s: No such file in directory. \n", pid);
			        fflush(stdout);
                            }else{
                                char* arg = 0;
                                size_t size = 0;
                                while(getdelim(&arg, &size, 0, environ) != -1){
                                    puts(arg);
                                }
                                free(arg);
			        fclose(environ);
                            }
                            free(temp);
                            free(pid);
                            free(dest);
		        }else if(strcmp(dest, "sched") == 0){
			    FILE* sched = fopen(pid, "rb");
                            if (sched == NULL){
                                printf("%s : No such file in directory. /n", pid);
			        fflush(stdout);
                            }else{
                                char* arg = 0;
                                size_t size = 0;
                                while(getdelim(&arg, &size, 0, sched) != -1){
                                    puts(arg);
                                }
                                free(arg);
			        fclose(sched);
                            }
                            free(temp);
                            free(pid);
                            free(dest);
                        }else{
		            printf("Something went wrong trying to read: %s", pid);
			    free(pid);
			    free(dest);
			    free(temp);
       	                }
		    }
		}else{
	            printf("Command not Found.\n");	
		    free(temp);
		}
	    }else{
	        pid_t pid = fork();
                if(pid == 0){
                    execvp(userInput[0], userInput);
                    printf("Command not found here.\n");
                    fflush(stdout);
		    free(temp);
		    free(input);
	            _exit(0);
	        }else if (pid != -1){
	            wait(NULL);
	        }else{
	            perror("error occured in calling function fork()");
	        }
	        free(temp);
	    }
	}
    }while(exitCommand);

    free(input);
    return returnValue;
}
