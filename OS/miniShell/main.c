#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>        // getpwuid
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>  // uid_t

#define MAX_PATH_LEN 96
#define LINE_LEN 80
#define MAX_ARGS 64

static char command_char = NULL;
static char command_line[LINE_LEN];
static int  buf_chars = 0;

struct command {
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

void check_command() {

    if(strcmp("cd", command.arg[0]) == 0) {
        call_cd();
    }
}

int main()
{
    hello();

    while(1) {
        printPrompt();

        command_char = getchar();
        if(command_char == '\n') continue;
        else {
            read_command(command_line, &command_char);
        }

        parse_command(command_line, &command);

        check_command();
    }
    return 0;
}
