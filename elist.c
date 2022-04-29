#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "logger.h"
#include "elist.h"
#include "history.h"

#define DEFAULT_INIT_SZ 10
#define RESIZE_MULTIPLIER 2

struct elist {
    size_t capacity;         /*!< Storage space allocated for list items */
    size_t size;             /*!< The actual number of items in the list */
    void * *element_storage;  /*!< Pointer to the beginning of the array  */
};

bool idx_is_valid(struct elist *list, size_t idx);

struct elist *elist_create(size_t list_sz)
{
    struct elist *list = malloc(sizeof(struct elist));
    if (list == NULL) {
        return NULL;
    }

    if (list_sz == 0){
        list_sz = DEFAULT_INIT_SZ;
    }

    list->size = 0;
    if (list_sz >= DEFAULT_INIT_SZ){
        list->capacity = list_sz;
    } else {
        list->capacity = DEFAULT_INIT_SZ;
    }
    list->element_storage = malloc(sizeof(void *) * list->capacity);
    if (list->element_storage == NULL){
        free(list);
        return NULL;
    }

    LOG("Created new elist. Size = %zu, capacity = %zu, start address = %p\n",
        list->size,
        list->capacity,
        list->element_storage);


    return list;

}

void **elist_storage_start(struct elist *list){
    return list->element_storage;
}

void elist_destroy(struct elist *list)
{
    free(list->element_storage);
    free(list);

}

int elist_set_capacity(struct elist *list, size_t capacity)
{
    if (capacity == 0){
        // option 1: set size to 1, then clear array
        // option 2: set the element storage to NULL, set the new capacity, return, and ensure it get re-allocated
        
        //Option 1
        // list->capacity == 1
        // elist_clear(list) // will probably update list->size

        //Option 2
        //list->element_storage = NULL
        //list->capacity = 0;
        //list->size = 0
        // add a condition that checks for capacity == 0, and realloc to default capacity
        
        capacity = 1;
        list->size = 0;

    }
    
    if (capacity == list->capacity) {
        return 0;
    }


    LOG("Setting new capacity: %zu old capacity = %zu\n", capacity, list->capacity);
    void *new_elements = realloc(list->element_storage, sizeof(void *) * capacity);
    if (new_elements == NULL){ 
        return -1;
    }

    //setting element storage here and not above just in case realloc fails.
    list->element_storage = new_elements;
    list->capacity = capacity;

    if (list->capacity < list->size){
        list->size = list->capacity;
    }

    return 0;
}

size_t elist_capacity(struct elist *list)
{
    return list->capacity;
}

ssize_t elist_add(struct elist *list, void *item)
{
    if(list->size >= list->capacity){
        //resize
        if (elist_set_capacity(list, list->capacity * RESIZE_MULTIPLIER) == -1){
            return -1;
        }
        void *temp = realloc(list->element_storage, list->capacity * sizeof(void *) * RESIZE_MULTIPLIER);
        if (temp != NULL){
            list->element_storage = temp;
        }
    }

    size_t index = list->size++;
    list->element_storage[index] = item;
    return index;


    // return -1 on failure, index otherwise
    //return -1;
}

int elist_set(struct elist *list, size_t idx, void *item)
{
    if (idx > list->capacity){
        printf("Index out of range!!\n");
        return -1;
    }

    if (idx >= list->size){
        printf("Index not set!!\n");
        return -1;
    }

    list->element_storage[idx] = item;

    return 0;
}

void *elist_get(struct elist *list, size_t idx)
{
    if (idx > list->capacity){
        //printf("Index out of bounds\n");
        return NULL;
    }

    if (idx >= list->size){
        //printf("Index out of bounds\n");
        return NULL;
    }

    //LOG("Trying to get at index: %zu \n", idx);
    
    return list->element_storage[idx];
}

size_t elist_size(struct elist *list)
{
    return list->size;
}

int elist_remove(struct elist *list, size_t idx)
{
    if (idx > list->size){
        return -1;
    }

    if (list->size == 0){
        printf("The list is empty!\n");
        return -1;
    }

    list->size--;
	//memmove(&list_ptr->start[idx],
	memmove(list->element_storage + idx, list->element_storage + idx + 1, (list->size - idx) * sizeof(void *));
    //free(list->element_storage[list->size]);
    
    return 0;
}

void elist_clear(struct elist *list)
{
    list->size = 0;
    // resize to 0
}

void elist_clear_mem(struct elist *list)
{
    memset(list->element_storage, 0, (list->size) * sizeof(void *));
    list->size = 0;
    list->capacity = DEFAULT_INIT_SZ;
    //memeset
    //remove all pointers, make them null
}

ssize_t elist_index_of(struct elist *list, void *item, size_t item_sz)
{
    for (int i = 0; i < list->size; i++){
        if (memcmp(item, list->element_storage[i], item_sz) == 0){
            return i;
        }
    }

    return -1;
}

void elist_sort(struct elist *list, int (*comparator)(const void *, const void *))
{
    qsort(list->element_storage, list->size, sizeof(void *), comparator);

}

bool idx_is_valid(struct elist *list, size_t idx)
{
    return false;
}

