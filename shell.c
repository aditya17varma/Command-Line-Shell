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
    char *stdin_file;
    bool stdout_append;
};

static int exec_result = 0;

static bool keep_shell_running;


int execute_pipeline(struct command_line *cmds)
{
    struct command_line *current = cmds;
    // exec_result = 0;
    
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
            close(fileno(stdin));
            exit(EXIT_FAILURE);
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
        
        exec_result = execvp(current->tokens[0], current->tokens);
        close(fileno(stdin));
        exit(EXIT_FAILURE);

    }

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
int change_cd(char *path){
    return chdir(path);
}

int exit_shell(void){
    keep_shell_running = false;
    return 0;
}

void sigint_handler(int signo){
    LOGP("ctrl + C doesn't kill me");
}


char **tokenize_command(char *comm){
    struct elist *token_list = elist_create(10);

    int token_count = 0;
    int null_count = 0;
    char *next_tok = comm;
    char *curr_tok;
    int null_positions[20];
    while ((curr_tok = next_token(&next_tok, " \t\n\r?")) != NULL) {
        
        // *(tokens + token_count) = curr_tok;
        if (strcmp(curr_tok, "|") == 0){
            // *(tokens + token_count) = '\0';
            // token_list_elements[token_count] = '\0';
            null_positions[null_count] = token_count;
            null_count++;
            // token_count = token_count + 1;
        }
        else if (strncmp(curr_tok, "#", 1) == 0){
            //change here
            break;
        } 
        elist_add(token_list, curr_tok);
        token_count = token_count + 1;
        
        // printf("Token %02d: '%s'\n", token_count = token_count + 1, (char *)elist_get(token_list, token_count));
    }

    char **tokens_elements = (char **) elist_storage_start(token_list);

    // printf("token_count: %d\n", token_count);
    // printf("Null count: %d\n", null_count);

    // for (int w = 0; w < null_count; w++){
    //     printf("Null at: %d\n", null_positions[w]);
    // }


    tokens_elements[token_count] = (char *) 0;

    if (null_count > 0){
        for (int n = 0; n < null_count; n++){
            int n_pos = null_positions[n];
            tokens_elements[n_pos] = '\0';
        }
    }

    return tokens_elements;
}

int main(void)
{
    init_ui();
    hist_init(100);
    struct elist *token_list = elist_create(10);

    signal(SIGINT, sigint_handler);
    
    char *command;
    keep_shell_running = true;
    while (keep_shell_running) {

        command = read_command();
        if (command == NULL) {
            // goto cleanup;
            free(command);
            break;
        }
        if (strlen(command) == 0){
            continue;
        }

        LOG("Input command: %s\n", command);

        char *command_dup = strdup(command);
        // LOG("commnad dup: %s\n", command_dup);
        hist_add(command_dup);
        
		int token_count = 0;
        int null_count = 0;
        char *next_tok = command;
        char *curr_tok;
        int null_positions[20];
        while ((curr_tok = next_token(&next_tok, " \t\n\r?")) != NULL) {
            
            // *(tokens + token_count) = curr_tok;
            if (strcmp(curr_tok, "|") == 0){
                // *(tokens + token_count) = '\0';
                // token_list_elements[token_count] = '\0';
                null_positions[null_count] = token_count;
                null_count++;
                // token_count = token_count + 1;
            }
            else if (strncmp(curr_tok, "#", 1) == 0){
                //change here
                break;
            } 
            elist_add(token_list, curr_tok);
            token_count = token_count + 1;
            
            // printf("Token %02d: '%s'\n", token_count = token_count + 1, (char *)elist_get(token_list, token_count));
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
        // // for(int l = 0; l < token_count; l++){
        // //     printf("token_list elem: %s\n", token_list_elements[l]);
        // // }
        
		if (token_list_elements[0] == NULL){
            // LOGP("token list[0] check");
			continue;
		}

        //Check builtins
        char *builtins[] = {"cd", "exit", "history", "!"};
        char *first_token = token_list_elements[0];
        
        int builtin_indx = -1;
        for (int b = 0; b < 4; b++){
            if (strncmp(builtins[b], first_token, strlen(builtins[b])) == 0){
                LOG("builtin: %s %s\n", builtins[b], first_token);
                builtin_indx = b;
                break;
            }
        }

        if (builtin_indx == 1){
            //exit
            // printf("exit clause triggered\n");
            free(command);
            hist_destroy();
            elist_clear(token_list);
            exit_shell();
            break;
            // exit(0);
        } else if (builtin_indx == 0) {
            //cd
            if(token_count == 1){
                char *env = getenv("HOME");
                // if(chdir(env) != 0){
                //     perror("cd to home failed\n");
                // }
                change_cd(env);
                // continue;
            } else {
                char *next_path = token_list_elements[1];
                LOG("next_path: %s\n", next_path);
                // if(chdir(next_path) != 0){
				//     prompt_status_bool = false;
				//     perror("cd failed");
			    // }
                change_cd(next_path);
			    // continue;
            }
        } else if (builtin_indx == 2){
            //history
            hist_print();
        } 
        // else if (builtin_indx == 3){
        //     // ! stuff
        //     char *after_bang = &first_token[1];
        //     LOG("bang! :%s\n", after_bang);
            
        //     char *last_com;
        //     char *search_cnum = hist_search_cnum((int)after_bang);
        //     char *search_prefix = hist_search_prefix(after_bang);
        //     if (strcmp(after_bang, "!") == 0){
        //         LOGP("double bang!!");
        //         int last_cnum = hist_last_cnum();
        //         last_com = hist_search_cnum(last_cnum);
        //         char *last_com_dup = strdup(last_com);
        //         hist_add(last_com_dup);
        //     } else if(search_cnum != NULL){
        //         LOGP("Num bang!");
        //         last_com = search_cnum;
        //         char *last_com_dup = strdup(last_com);
        //         hist_add(last_com_dup);
        //     } else if (search_prefix != NULL){
        //         LOGP("Prefix bang!");
        //         last_com = search_prefix;
        //         char *last_com_dup = strdup(last_com);
        //         hist_add(last_com_dup);
        //     }
        //     //retokenize and exec
        //     struct elist *token_list_bang = elist_create(10);

        //     int token_count_bang = 0;
        //     int null_count_bang = 0;
        //     char *next_tok_bang = last_com;
        //     char *curr_tok_bang;
        //     int null_positions_bang[20];
        //     while ((curr_tok_bang = next_token(&next_tok_bang, " \t\n\r?")) != NULL) {
                
        //         if (strcmp(curr_tok_bang, "|") == 0){
        //             null_positions_bang[null_count_bang] = token_count_bang;
        //             null_count_bang++;
        //         }
        //         else if (strncmp(curr_tok_bang, "#", 1) == 0){
        //             //change here
        //             break;
        //         } 
        //         elist_add(token_list_bang, curr_tok_bang);
        //         token_count_bang = token_count_bang + 1;
                
        //         // printf("Token %02d: '%s'\n", token_count = token_count + 1, (char *)elist_get(token_list, token_count));
        //     }

        //     char **token_list_elements_bang = (char **) elist_storage_start(token_list_bang);
        //     // char **token_list_elements_bang = tokenize_command(command);

        //     token_list_elements_bang[token_count_bang] = (char *) 0;

        //     if (null_count_bang > 0){
        //         for (int n = 0; n < null_count_bang; n++){
        //             int n_pos = null_positions_bang[n];
        //             token_list_elements_bang[n_pos] = '\0';
        //         }
        //     }

        //     if (token_list_elements_bang[0] == NULL){
		// 	    continue;
		//     }

        //     struct command_line cmd_bang[token_count_bang - null_count_bang];
        //     memset(cmd_bang, 0, sizeof(cmd_bang));

        //     int b = 0;

        //     cmd_bang[b].tokens = token_list_elements_bang;
        //     cmd_bang[b].stdout_pipe = false;
        //     cmd_bang[b].stdout_file = NULL;
        //     b++;
        //     // printf("j:%d --> %s\n", j, *(cmd[b - 1].tokens));
        //     for (int k = 0; k < null_count_bang; k++){
        //         int nulls = null_positions_bang[k];
        //         // cmd[j].tokens = &tokens[nulls + 1];
        //         cmd_bang[b].tokens = &token_list_elements_bang[nulls + 1];
        //         cmd_bang[b].stdout_pipe = false;
        //         cmd_bang[b].stdout_file = NULL;

        //         cmd_bang[b - 1].stdout_pipe = true;
        //         b++;
        //         // printf("j:%d --> %s\n", j, *(cmd[j - 1].tokens));
        //     }

        //     int exec_bang = 0;
        //     exec_bang = execute_pipeline(cmd_bang);
            
        //     pid_t child_bang = fork();
        //     if (child_bang == 0){
                
        //         LOG("exec bang: %d\n", exec_bang);

        //         //TODO FIX PROMPT EMOJI CHANGE
        //         if (exec_bang != 0){
        //             prompt_status_bool = false;
        //             LOG("promp status bool: %d\n", prompt_status_bool);
        //         } else {
        //             prompt_status_bool = true;
        //             LOG("promp status bool: %d\n", prompt_status_bool);
        //         }
        //         LOG("exec_bang_result: %d\n", exec_bang);
        //         // exit(exec_bang);
                
        //     } else {
        //         int status;
        //         wait(&status);
        //     }
        //     continue;
        //     // LOGP("Gets to ! builtin\n");
        //     //continue;
        // }

        else {
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

            pid_t child = fork();
            if (child == 0){
                int exec = 0;
                exec = execute_pipeline(cmd);
                LOG("exec: %d\n", exec);

                //TODO FIX PROMPT EMOJI CHANGE
                if (exec != 0){
                    prompt_status_bool = false;
                    LOG("promp status bool: %d\n", prompt_status_bool);
                } else {
                    prompt_status_bool = true;
                    LOG("promp status bool: %d\n", prompt_status_bool);
                }
                LOG("exec_result: %d\n", exec);
                // exit(exec);
                
            } else {
                int status;
                wait(&status);
            }

        }
        elist_clear(token_list);
        free(command);
        

        //tokenize_command(...)
        //check_builtins(...)
        //execute_stuff(...)

        

        // when tokenizing the command line prompt
        // for each token:
            // if token == "|"
                //token == '\0' or null
                //this stops execvp
            //to get next set of tokens, start execvp at index after
            // so cmd[1].tokens = &tokens[i + 1]

        /* We are done with command; free it */
// cleanup:
        
        
    }
    

    return 0;
}


/*
use shell.c made in class
read_command calls ui.c

implement scripting support




*/

