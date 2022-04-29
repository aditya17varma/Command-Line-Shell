#include <fcntl.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "history.h"
#include "logger.h"
#include "ui.h"
#include "elist.h"

#define DEFAULT_INIT_SZ 10

struct command_line {
    char **tokens;
    bool stdout_pipe;
    char *stdout_file;
};

static int exec_result = 0;


int execute_pipeline(struct command_line *cmds)
{
    struct command_line *current = cmds;
    exec_result = 0;
    
    if (current->stdout_pipe == true){
        int fds[2];
        pipe(fds);
        pid_t child = fork();

        if (child == 0){
            //child
            dup2(fds[1], STDOUT_FILENO);
            close(fds[0]);
            close(fds[1]);
            // close(fds[1]);
            exec_result = execvp(current->tokens[0], current->tokens);
        } else{
            dup2(fds[0], STDIN_FILENO);
            close(fds[1]);
            close(fds[0]);
            // close(fds[1]);
            exec_result = execute_pipeline(cmds + 1);
        }
    } else {
        // dup2(fds[0], STDIN_FILENO);
        // dup2(fds[1], 1);
        if (current->stdout_file != NULL){
            int output = open(current->stdout_file, O_CREAT | O_WRONLY, 0666);
            dup2(output, 1);
        }
        // close(fds[0]);
        // close(fds[1]);
        exec_result = execvp(current->tokens[0], current->tokens);

    }

    // if (exec_result != 0){
    //     prompt_status_bool = false;
    // } else{
    //     prompt_status_bool = true;
    // }

    return exec_result;
}

char *next_token(char **str_ptr, const char *delim)
{
    if (*str_ptr == NULL) {
        return NULL;
    }

    size_t tok_start = strspn(*str_ptr, delim);
    size_t tok_end = strcspn(*str_ptr + tok_start, delim);

    /* Zero length token. We must be finished. */
    if (tok_end  == 0) {
        *str_ptr = NULL;
        return NULL;
    }

    /* Take note of the start of the current token. We'll return it later. */
    char *current_ptr = *str_ptr + tok_start;

    /* Shift pointer forward (to the end of the current token) */
    *str_ptr += tok_start + tok_end;

    if (**str_ptr == '\0') {
        /* If the end of the current token is also the end of the string, we
         * must be at the last token. */
        *str_ptr = NULL;
    } else {
        /* Replace the matching delimiter with a NUL character to terminate the
         * token string. */
        **str_ptr = '\0';

        /* Shift forward one character over the newly-placed NUL so that
         * next_pointer now points at the first character of the next token. */
        (*str_ptr)++;
    }

    return current_ptr;
}

// void check_builtins()



int main(void)
{
    init_ui();
    hist_init(100);
    struct elist *token_list = elist_create(10);
    
    char *command;
    bool keep_shell_running = true;
    while (true) {

        

        command = read_command();
        if (command == NULL) {
            // goto cleanup;
            free(command);
            break;
        }

        LOG("Input command: %s\n", command);

        char *command_dup = strdup(command);
        LOG("commnad dup: %s\n", command_dup);
        hist_add(command_dup);
        
		int token_count = 0;
        int null_count = 0;
        char *next_tok = command;
        char *curr_tok;
        int null_positions[20];
        while ((curr_tok = next_token(&next_tok, " \t\n\r?")) != NULL) {
            elist_add(token_list, curr_tok);
            // *(tokens + token_count) = curr_tok;
            if (strcmp(curr_tok, "|") == 0){
                // *(tokens + token_count) = '\0';
                // token_list_elements[token_count] = '\0';
                null_positions[null_count] = token_count;
                null_count++;
            }
            
            // token_count = token_count + 1;
            printf("Token %02d: '%s'\n", token_count = token_count + 1, (char *)elist_get(token_list, token_count));
        }

        char **token_list_elements = (char **) elist_storage_start(token_list);

        // printf("token_count: %d\n", token_count);
        // printf("Null count: %d\n", null_count);

        // for (int w = 0; w < null_count; w++){
        //     printf("Null at: %d\n", null_positions[w]);
        // }


        token_list_elements[token_count] = (char *) 0;

        if (null_count > 0){
            for (int n = 0; n < null_count; n++){
                int n_pos = null_positions[n];
                token_list_elements[n_pos] = '\0';
            }
        }
        

        // for(int l = 0; l < token_count; l++){
        //     printf("token_list elem: %s\n", token_list_elements[l]);
        // }
        
		if (token_list_elements[0] == NULL){
			continue;
		}

        //Check builtins
        
        printf("first token: %s\n", token_list_elements[0]);
        if(strncmp(token_list_elements[0], "exit", 4) == 0){
            printf("exit clause triggered\n");
            free(command);
            hist_destroy();
            elist_clear(token_list);
			keep_shell_running = false;
            exit(0);
        } else if (strncmp(token_list_elements[0], "cd", 2) ==0){
            if(token_count == 1){
                char *env = getenv("HOME");
                chdir(env);
                continue;
            } else {
                char *next_path = token_list_elements[1];
                printf("next_path: %s\n", next_path);
                chdir(next_path);
                continue;
            }
        }
		// } else if (strncmp(*(cmd[0].tokens), "cd", 2) == 0){
		// 	//handle cd here
            
		// 	chdir(*(cmd[0].tokens));
		// 	continue;
		// } else if (strcmp(*(cmd[0].tokens), "history") == 0){
        //     hist_print();
        //     continue;
        // } 

        //tokenize_command(...)
        //check_builtins(...)
        //execute_stuff(...)

        //get struct command_line and execute pipeline from leetify.c
        struct command_line cmd[token_count - null_count];
        memset(cmd, 0, sizeof(cmd));
        
        int j = 0;
        //first command
        // cmd[j].tokens = &tokens[0];
        
        cmd[j].tokens = token_list_elements;
        cmd[j].stdout_pipe = false;
        cmd[j].stdout_file = NULL;
        j++;
        // printf("j:%d --> %s\n", j, *(cmd[j - 1].tokens));
        for (int k = 0; k < null_count; k++){
            int nulls = null_positions[k];
            // cmd[j].tokens = &tokens[nulls + 1];
            cmd[j].tokens = &token_list_elements[nulls + 1];
            cmd[j].stdout_pipe = false;
            cmd[j].stdout_file = NULL;

            cmd[j - 1].stdout_pipe = true;
            j++;
            // printf("j:%d --> %s\n", j, *(cmd[j - 1].tokens));
        }

        // char *first_token = *(cmd[0].tokens);
        // printf("first_token: %s\n", first_token);
        // printf("first_token + 1: %s\n", *(cmd[0].tokens) + 1);
        // printf("j: %d\n", j);
        // printf("strlen: %ld\n", strlen(*(cmd[0].tokens)));
        // if(strcmp(first_token, "exit") == 0){
		// 	printf("strcmp with exit works\n");
        // }
        // if (j > 1){
        //     printf("cmd[1].tokens + 2: %s\n", *(cmd[1].tokens));
        // }



        pid_t child = fork();
        if (child == 0){
            int exec = execute_pipeline(cmd);
            if (exec != 0){
                prompt_status_bool = false;
            } else {
                prompt_status_bool = true;
            }
            printf("exec_result: %d\n", exec);
            exit(exec);
            
        } else {
         int status;
            wait(&status);
        }

        elist_clear(token_list);

        // when tokenizing the command line prompt
        // for each token:
            // if token == "|"
                //token == '\0' or null
                //this stops execvp
            //to get next set of tokens, start execvp at index after
            // so cmd[1].tokens = &tokens[i + 1]

        /* We are done with command; free it */
// cleanup:
        free(command);
        
    }
    //elist_clear(token_list);

    return 0;
}


/*
use shell.c made in class
read_command calls ui.c

implement scripting support




*/

