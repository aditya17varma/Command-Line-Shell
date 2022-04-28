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
static char cwd[256];
static char spliced_cwd[256];
// struct passwd pwd;

static int readline_init(void);

void init_ui(void)
{   
//     printf("\n\n\n\n******************"
//         "****************************"
//         "******************************\n");
//     printf("\n\n\t****  ARE YOU READY TO ENTER THE DRAGON??  ****\n");
//     // printf("\n\n\t\t   -USE AT YOUR OWN RISK-\n");
//     printf("\n\n");
            
//     puts("\t\t\t\t^    ^\n"
//         "\t\t               / \\  //\\ \n"
//        "\t\t |\\___/|      /   \\//  \\ \n"
//             "\t\t /0  0  \\__  /    //  | \\ \\ \n"   
//            "\t\t/     /  \\/_/    //   |  \\  \\ \n"  
//            "\t\t@_^_@'/   \\/_   //    |   \\   \\ \n"
//            "\t\t//_^_/     \\/_ //     |    \\    \\ \n"
//         "\t     ( //) |        \\///      |     \\     \\ \n"
//       "\t   ( / /) _|_ /   )  //       |      \\     _\\ \n"
//     "\t ( // /) '/,_ _ _/  ( ; -.    |    _ _\\.-~        .-~~~^-. \n"
//   "        (( / / )) ,-{        _      `-.|.-~-.           .~         `. \n"
//  "      (( // / ))  '/\\      /                 ~-. _ .-~      .-~^-.  \\ \n"
//  "      (( /// ))      `.   {            }                   /      \\  \\ \n"
//   "       (( / ))     .----~-.\\        \\-'                 .~         \\  `. \\^-. \n"
//              "\t\t  ///.----..>        \\             _ -~             `.  ^-`  ^-__ \n"
//                "\t\t    ///-._ _ _ _ _ _ _}^ - - - - ~                     ~-- ,.-~ \n"
//                                             "\t\t\t\t\t\t\t\t\t/.-~\n");

//     printf("\n************************"
//         "***********************"
//         "*****************************\n");
    
    LOGP("Initializing UI...\n");

    char *locale = setlocale(LC_ALL, "en_US.UTF-8");
    LOG("Setting locale: %s\n",
            (locale != NULL) ? locale : "could not set locale!");

    rl_startup_hook = readline_init;

    if (isatty(STDIN_FILENO)) {
        LOGP("stdin is a TTY; entering interactive mode\n");
        scripting = true;
    } 
}

void destroy_ui(void)
{
    // TODO cleanup code, if necessary
}

char *prompt_line(void)
{
    const char *status = prompt_status() ? good_str : bad_str;

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

    // printf("Got till here\n");

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
    // printf("The uid is: %d\n", uid);
    struct passwd *pwd = getpwuid(uid);
    // printf("The username is: %s\n", pwd->pw_name);
    return pwd->pw_name;

}

char *prompt_hostname(void)
{
    // gethostname function -> returns local stuff
    // create a static variable to store the hostname
    // 
    // char hostname[10];
    gethostname(hostname, 10);
    // printf("The hostname is: %s\n", hostname);
    return hostname;
}

char *prompt_cwd(void)
{
    //might need to allocate memory and free
    // char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) == NULL){
        perror("getcwd() error");
        
    } else {
        // printf("The current working directory is: %s\n", cwd);
        char *env = getenv("HOME");
        int resultCmp = strncmp(cwd, env, strlen(env));
        if (resultCmp == 0){
            //splice
            char spliced[strlen(cwd) - strlen(env)];
            memcpy(spliced, &cwd[strlen(env)], strlen(cwd) - strlen(env));
            // spliced[strlen(spliced) - 1] = '\0';
            // printf("%s\n", spliced);
            // char result[strlen(spliced)];
            sprintf(spliced_cwd, "~%s", spliced);
            spliced_cwd[strlen(spliced_cwd)] = '\0';
            // printf("%s\n", spliced_cwd);
            
            return spliced_cwd;
        } else {
            return cwd;
        }

        // return 0;
    }
    return "/unknown/path";
}

int prompt_status(void)
{
    bool tempF = false;
    bool tempT = true;
    // printf("true: %i false: %i\n", tempT, tempF);
    return tempT;
}

unsigned int prompt_cmd_num(void)
{
    //use last_cnum from history.c to get appropriate number
    int num = 100;
    return num;
}

char *read_command(void)
{

    //implement scripting support here
    //if we are receiving commands from a user, then do the following:
    if (scripting){
        char *line = NULL;
        size_t buf_sz = 0;
        ssize_t read_sz = getline(&line, &buf_sz, stdin);
        if (read_sz == -1){
            perror("getline");
            return NULL;
        }
        line[read_sz - 1] = '\0';
        return line;
    } else {
        char *prompt = prompt_line();
        char *command = readline(prompt);
        free(prompt);
        return command;
    }
   
    //if we are receinvng commnds form a **script**, then do the following
    //<insert code that uses getline instead of readline here>
    // return command;
}

int readline_init(void)
{
    rl_variable_bind("show-all-if-ambiguous", "on");
    rl_variable_bind("colored-completion-prefix", "on");
    return 0;
}

// int main(void)
// {
//     // test the functions here
//     // char *test_dir = prompt_cwd();
//     // char *env = getenv("HOME");
//     // printf("%s\n", cwd);
//     // printf("%d\n", strlen(cwd));
//     // printf("%s\n", env);
//     // printf("%d\n", strlen(env));

//     // int resultCmp = strncmp(cwd, env, strlen(env));
//     // printf("%d\n", resultCmp);

//     prompt_cwd();
//     // printf("%s\n", prompt_cwd());
//     // uid_t test = getuid();
//     // printf("The test uid: %d\n", test);
//     // prompt_username();
//     // const char *test_status = prompt_status() ? good_str : bad_str;
//     // printf("Test status: %s\n", test_status);
//     // printf("Test cmd_num: %i\n", prompt_cmd_num());

//     // printf("Size test: %i\n", strlen(prompt_cwd()));
    
    
    
//     // printf("Testing prompt: %s\n", prompt_line());




// }
