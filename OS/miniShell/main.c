#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>        // getpwuid
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>  // uid_t

#define MAX_PATHS 64
#define MAX_PATH_LEN 96
#define LINE_LEN 80
#define MAX_ARGS 64

static char command_char = NULL;
static char command_line[LINE_LEN];
static int  buf_chars = 0;
static char *pathv[MAX_PATHS];

struct command {
    char* name;
    int num_arg;
    char* arg[MAX_ARGS];
};

struct command command;

void hello() {
    printf("////////////////////////////////////////\n");
    printf("//Welcome to miniShell                //\n");
    printf("////////////////////////////////////////\n");
}

void printPrompt() {
    char buffer[MAX_PATH_LEN];
    char* cur_path;
    time_t timer;
    struct tm *t;
    uid_t          user_id;
    struct passwd *user_pw;

    cur_path = getcwd(buffer,MAX_PATH_LEN);

    timer = time(NULL);
    t = localtime(&timer);

    user_id  = getuid();             // 사용자 아이디를 구하고
    user_pw  = getpwuid( user_id);   // 아이디로 사용자 정보 구하기

    printf("[%02d:%02d:%02d]%s@%s$",t->tm_hour,t->tm_min,t->tm_sec,user_pw->pw_name,cur_path);

}

void read_command(char* buffer, char* command_input) {
    buf_chars = 0;

    while((*command_input != '\n') && (buf_chars < LINE_LEN)) {
        buffer[buf_chars++] = *command_input;
        *command_input = getchar();
    }
    buffer[buf_chars] = '\0';
}

void parse_command(char* commandline, struct command* command)
{
    char* ptr;
    ptr = strtok(commandline, " ");
    int i = 0;

    while(ptr != NULL) {
        command->arg[i] = ptr;
        ptr = strtok(NULL, " ");
        i++;
    }

    command->num_arg = i;
    command->arg[i++] = NULL;
}

void call_cd() {
    if(command.arg[1] == NULL) chdir(getenv("HOME"));
    else {
        if (chdir(command.arg[1]) == -1) {
            printf("%s: 그런 파일이나 디렉터리가 없습니다.\n",command.arg[1]);
        }
    }
}

int process_command() {
    int i;
    for(i=0; i<command.num_arg; i++) {
        if(strcmp(command.arg[i], ">")==0) {
        return file_out_command(i);
        }
        else if(strcmp(command.arg[i], "<")==0) {
        return file_in_command(i);
        }
    }
    return excute_command();
}

int file_out_command(int i) {

    char* arg_t[5];
    int j;

    for(j=0; j<i; j++) {
        arg_t[j] = command.arg[j];
    }
    arg_t[j] = NULL;

    int fd = dup(1);

    int file = open(command.arg[i+1], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRGRP | S_IROTH);
    if(file < 0) return 1; // file open fails

    if(dup2(file,1) < 0) return 1;

    pid_t pid;
    if(pid = fork() == 0) {
        close(file);
        close(fd);
        execve(command.name, arg_t, 0);
        return 0;
    }
    dup2(fd,1);
    close(file);
    close(fd);
    wait(NULL);
    return 0;
}

int file_in_command(int i) {

    char* arg_t[5];
    int j;

    for(j=0; j<i; j++) {
        arg_t[j] = command.arg[j];
    }
    arg_t[j] = NULL;

    int fd = dup(0);
    FILE * fp;
    int file = open(command.arg[i+1], O_RDONLY);
    if(file < 0) return 1; // file open fails

    if(dup2(file,STDIN_FILENO) < 0) return 1;

    pid_t pid;
    if(pid = fork() == 0) {
        close(file);
        close(fd);
        execve(command.name, arg_t, 0);
        return 0;
    }
    dup2(fd,0);
    close(file);
    close(fd);
    wait(NULL);
    return 0;
}

// excuteCommand() executes regular command, commands which doesn't have > < |
// rediractions
//
// example: ls -il, cat filname
//
// @return 0 if exec if successful
int excute_command() {

	int child_pid;
	int status;
	pid_t thisChPID;


	child_pid = fork();
	if(child_pid < 0 ) {
		fprintf(stderr, "Fork fails: \n");
		return 1;
	}
	else if(child_pid==0) {
		/* CHILD */
		execve(command.name, command.arg,0);
		printf("Child process failed\n");
		return 1;
	}
	else if(child_pid > 0) {
		/* PARENT */

		do {
			thisChPID = waitpid(child_pid, &status, WUNTRACED | WCONTINUED);
            if (thisChPID == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }

            if (WIFEXITED(status)) {
                //printf("exited, status=%d\n", WEXITSTATUS(status));
                return 0;
            } else if (WIFSIGNALED(status)) {
                printf("killed by signal %d\n", WTERMSIG(status));
            } else if (WIFSTOPPED(status)) {
                printf("stopped by signal %d\n", WSTOPSIG(status));
            } else if (WIFCONTINUED(status)) {
                printf("continued\n");
            }
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		return 0;
	}
}


int check_command() {

    if(strcmp("cd", command.arg[0]) == 0) {
        call_cd();
        return 1;
    }
    else if(strcmp("clear", command.arg[0]) == 0) {
        printf("\033[2J\033[1;1H");
        return 1;
    }
    return 0;
}

// this function populates "pathv" with environment variable PATH
int parsePath(char* dirs[]){
	int debug = 0;
	char* pathEnvVar;
	char* thePath;
	int i;

	for(i=0 ; i < MAX_ARGS ; i++ ) {
		dirs[i] = NULL;
		}
	pathEnvVar = (char*) getenv("PATH");
	thePath = (char*) malloc(strlen(pathEnvVar) + 1);
	strcpy(thePath, pathEnvVar);
//    printf("%s\n\n",thePath);
	char* pch;
	pch = strtok(thePath, ":");
	int j=0;
//	printf("%s\n",pathEnvVar);
//	printf("%s\n",pch);
	// loop through the thePath for ':' delimiter between each path name
	while(pch != NULL) {
        dirs[j] = pch;
		pch = strtok(NULL, ":");
//		printf("%s %d\n",dirs[j], j);
		j++;
	}

	//===================== debug ===============
	// print the directories if debugging
	if(debug){
		printf("Directories in PATH variable\n");
		for(i=0; i<MAX_PATHS; i++)
			if(dirs[i] != '\0')
				printf("    Directory[%d]: %s\n", i, dirs[i]);
	}
}

char * lookupPath(char **argv, char **dir) {
	char *result;
	char pName[MAX_PATH_LEN];
	if( *argv[0] == '/') {
        printf("%s\n",argv[0]);
		return argv[0];
	}
	else if( *argv[0] == '.') {
		if( *++argv[0] == '.') {
			 if(getcwd(pName,sizeof(pName)) == NULL)
				 perror("getcwd(): error\n");
			 else {
				 *--argv[0];
				 asprintf(&result,"%s%s%s",pName,"/",argv[0]);
			 }
			 printf("%s\n",result);
			 return result;
		}
		*--argv[0];
		if( *++argv[0] == '/') {
			if(getcwd(pName,sizeof(pName)) == NULL)
				perror("getcwd(): error\n");
			else {
				asprintf(&result,"%s%s",pName,argv[0]);
			}
			printf("%s\n",result);
			return result;
		}
	}

	// look in PAH directories, use access() to see if the
	// file is in the dir
	int i;
	for(i = 0 ; i < MAX_PATHS ; i++ ) {
		if(dir[i] != NULL) {
			asprintf(&result,"%s%s%s",  dir[i],"/",argv[0]);
			if(access(result, X_OK) == 0) {
                printf("%s\n",result);
				return result;
			}
		}
		else continue;
	}

	fprintf(stderr, "%s: command not found\n", argv[0]);
	return NULL;
}
int main()
{
    parsePath(pathv);
    hello();

    while(1) {
        printPrompt();

        command_char = getchar();
        if(command_char == '\n') continue;
        else {
            read_command(command_line, &command_char);
        }

        parse_command(command_line, &command);

        if(check_command() == 0) {
            command.name = lookupPath(command.arg, pathv);
            process_command();

        }
    }
    return 0;
}

