#include <stdio.h>
#include <readline/readline.h>
#include <locale.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pwd.h>
#include <sys/types.h>

#include "history.h"
#include "logger.h"
#include "ui.h"

static const char *good_str = "😌";
static const char *bad_str  = "🤯";
static bool scripting = false;
static char hostname[256];
// struct passwd pwd;

static int readline_init(void);

// void init_ui(void)
// {
//     LOGP("Initializing UI...\n");

//     char *locale = setlocale(LC_ALL, "en_US.UTF-8");
//     LOG("Setting locale: %s\n",
//             (locale != NULL) ? locale : "could not set locale!");

//     rl_startup_hook = readline_init;

//     if (isatty(STDIN_FILENO)) {
//         LOGP("stdin is a TTY; entering interactive mode\n");
//         scripting = true;
//     } 
// }

void destroy_ui(void)
{
    // TODO cleanup code, if necessary
}

char *prompt_line(void)
{
    const char *status = prompt_status() ? bad_str : good_str;

    char cmd_num[25];
    snprintf(cmd_num, 25, "%d", prompt_cmd_num());

    char *user = prompt_username();
    char *host = prompt_hostname();
    char *cwd = prompt_cwd();

    char *format_str = ">>-[%s]-[%s]-[%s@%s:%s]-> ";

    size_t prompt_sz
        = strlen(format_str)
        + strlen(status)
        + strlen(cmd_num)
        + strlen(user)
        + strlen(host)
        + strlen(cwd)
        + 1;

    char *prompt_str =  malloc(sizeof(char) * prompt_sz);

    snprintf(prompt_str, prompt_sz, format_str,
            status,
            cmd_num,
            user,
            host,
            cwd);

    return prompt_str;
}

char *prompt_username(void)
{
    uid_t uid = getuid();
    struct passwd *pwd = getpwuid(uid);
    return pwd->pw_name;

}

char *prompt_hostname(void)
{
    // gethostname function -> returns local stuff
    // create a static variable to store the hostname
    // 
    // char hostname[10];
    gethostname(hostname, 10);
    printf("The hostname is: %s\n", hostname);
    return hostname;
}

char *prompt_cwd(void)
{
    //might need to allocate memory and free
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) == NULL){
        perror("getcwd() error");
    } else {
        printf("The current working directory is: %s\n", cwd);
        return cwd;
    }
    return "/unknown/path";
}

int prompt_status(void)
{
    return -1;
}

unsigned int prompt_cmd_num(void)
{
    //use last_cnum from history.c to get appropriate number
    return 0;
}

// char *read_command(void)
// {

//     //implement scripting support here
//     //if we are receiving commands from a user, then do the following:
//     char *prompt = prompt_line();
//     char *command = readline(prompt);
//     free(prompt);

//     //if we are receinvng commnds form a **script**, then do the following
//     //<insert code that uses getline instead of readline here>
//     return command;
// }

// int readline_init(void)
// {
//     rl_variable_bind("show-all-if-ambiguous", "on");
//     rl_variable_bind("colored-completion-prefix", "on");
//     return 0;
// }

int main(void)
{
    //test the functions here
    prompt_hostname();
    prompt_cwd();




}
