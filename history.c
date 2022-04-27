#include <stddef.h>

#include "history.h"
#include "elist.h"

static struct elist *list;
struct hist_entry {
    char *cmd;
    int cmd_number;
};

static int counter = 0;

void hist_init(unsigned int limit)
{
    // TODO: set up history data structures, with 'limit' being the maximum
    // number of entries maintained.
    elist_create(limit);
}

void hist_destroy(void)
{

}

void hist_add(const char *cmd)
{

}

void hist_print(void)
{

}

const char *hist_search_prefix(char *prefix)
{
    // TODO: Retrieves the most recent command starting with 'prefix', or NULL
    // if no match found.
    return NULL;
}

const char *hist_search_cnum(int command_number)
{
    // TODO: Retrieves a particular command number. Return NULL if no match
    // found.
    return NULL;
}

unsigned int hist_last_cnum(void)
{
    // TODO: Retrieve the most recent command number.
    return 0;
}
