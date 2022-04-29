#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "history.h"
#include "elist.h"


static struct elist *list;
struct hist_entry {
    char *comd;
    int cmd_number;
};

static int counter = 0;

void hist_init(unsigned int limit)
{
    // TODO: set up history data structures, with 'limit' being the maximum
    // number of entries maintained.
    list = elist_create(limit);
}

void hist_destroy(void)
{
    elist_destroy(list);
}

void hist_add(const char *cmd)
{
    struct hist_entry *temp = malloc(sizeof(struct hist_entry));
    temp->comd = cmd;
    temp->cmd_number = counter;
    counter++;
    elist_add(list, temp);
}

void hist_print(void)
{
    if (counter > 100){
        int begin = counter - 100 + 1;
        for (int j = begin; j < counter; j++){
            struct hist_entry *temp = elist_get(list, j);
            int temp_cmd_num = temp->cmd_number;
            char* temp_cmd = temp->comd;
            printf("%d %s\n", temp_cmd_num, temp_cmd);
        }

    } else {
        for (int i = 0; i < counter; i++){
            struct hist_entry *temp = elist_get(list, i);
            int temp_cmd_num = temp->cmd_number;
            char* temp_cmd = temp->comd;
            printf("%d %s\n", temp_cmd_num, temp_cmd);
        }
    }

}

const char *hist_search_prefix(char *prefix)
{
    // TODO: Retrieves the most recent command starting with 'prefix', or NULL
    // if no match found.
    for(int i = counter; i > 0; i--){
        struct hist_entry *temp = elist_get(list, i);
        if (strncmp(prefix, temp->comd, strlen(prefix)) == 0){
            return temp->comd;
        }
    }

    return NULL;
}

const char *hist_search_cnum(int command_number)
{
    // TODO: Retrieves a particular command number. Return NULL if no match
    // found.
    if (command_number <= counter){
        struct hist_entry *temp = elist_get(list, command_number);
        return temp->comd;
    }

    return NULL;
}

unsigned int hist_last_cnum(void)
{
    // TODO: Retrieve the most recent command number.
    return counter;
}
