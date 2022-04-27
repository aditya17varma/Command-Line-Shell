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

struct command_line {
    char **tokens;
    bool stdout_pipe;
    char *stdout_file;
};

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

int main(void)
{
    init_ui();

    char *command;
    while (true) {

        
        command = read_command();
        if (command == NULL) {
            // goto cleanup;
            free(command);
            break;
        }

        LOG("Input command: %s\n", command);

        char *tokens[10];
		int token_count = 0;
    		char *next_tok = command;
    		char *curr_tok;
    		while ((curr_tok = next_token(&next_tok, " \t\n\r?")) != NULL) {
			tokens[token_count] = curr_tok;
        		printf("Token %02d: '%s'\n", token_count++, curr_tok);
    		}
		tokens[token_count] = (char *) 0;

		if (tokens[0] == NULL){
			continue;
		}

        //tokenize_command(...)
        //check_builtins(...)
        //execute_stuff(...)

        //get struct command_line and execute pipeline from leetify.c

        struct command_line cmd = {0};
        cmd.tokens = tokens;
        cmd.stdout_file = NULL;
        cmd.stdout_pipe = false;

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

    return 0;
}

/*
use shell.c made in class
read_command calls ui.c

implement scripting support




*/

