#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "cache.h"

/**
 * Allocate a cache entry
 */
struct cache_entry *alloc_entry(char *path, char *content_type, void *content, int content_length)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    // I think he just means allocate memory and initialize. We should have all the info to do that.
    struct cache_entry *ce = malloc(sizeof(struct cache_entry));
    ce->path = path;
    ce->content_type = content_type;
    ce->content = content;
    ce->content_length = content_length;
    return ce;
}

/**
 * Deallocate a cache entry
 */
void free_entry(struct cache_entry *entry)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    // free(entry->path); /// THESE THREE SUB FREEs CRASH THE PROGRAM. BUT WHY???
    // free(entry->content_type);
    // free(entry->content);
    free(entry);
}

/**
 * Insert a cache entry at the head of the linked list
 */
void dllist_insert_head(struct cache *cache, struct cache_entry *ce)
{
    // Insert at the head of the list
    if (cache->head == NULL)
    {
        cache->head = cache->tail = ce;
        ce->prev = ce->next = NULL;
    }
    else
    {
        cache->head->prev = ce;
        ce->next = cache->head;
        ce->prev = NULL;
        cache->head = ce;
    }
}

/**
 * Move a cache entry to the head of the list
 */
void dllist_move_to_head(struct cache *cache, struct cache_entry *ce)
{
    if (ce != cache->head)
    {
        if (ce == cache->tail)
        {
            // We're the tail
            cache->tail = ce->prev;
            cache->tail->next = NULL;
        }
        else if (ce->prev == NULL && ce->next == NULL) // newborn
        { 
            ce->next = cache->head;
            cache->head = ce;
            return;
        }
        else
        {
            // We're neither the head nor the tail
            printf("aaaaaaa\n");

            ce->prev->next = ce->next; // seg fault here
            // printf("HEREasdfsffffffadsf\n");

            ce->next->prev = ce->prev;
            // printf("HEREasdfsffffffadsf\n");
        }

        ce->next = cache->head;
        cache->head->prev = ce;
        ce->prev = NULL;
        cache->head = ce;
    }
}

/**
 * Removes the tail from the list and returns it
 * 
 * NOTE: does not deallocate the tail
 */
struct cache_entry *dllist_remove_tail(struct cache *cache)
{
    struct cache_entry *oldtail = cache->tail;

    cache->tail = oldtail->prev;
    cache->tail->next = NULL;

    cache->cur_size--;

    return oldtail;
}

/**
 * Create a new cache
 * 
 * max_size: maximum number of entries in the cache
 * hashsize: hashtable size (0 for default)
 */
struct cache *cache_create(int max_size, int hashsize)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    // A cache
    // struct cache {
    //     struct hashtable *index;
    //     struct cache_entry *head, *tail; // Doubly-linked list
    //     int max_size; // Maxiumum number of entries
    //     int cur_size; // Current number of entries
    // };
    struct cache *cache = malloc(sizeof(struct cache));
    struct hashtable *ht = hashtable_create(hashsize, NULL); // if set size to 0, defaults to 128
    cache->index = ht;
    cache->head = NULL;
    cache->tail = NULL;
    cache->max_size = max_size;
    cache->cur_size = 0;
    return cache;
}

void cache_free(struct cache *cache)
{
    struct cache_entry *cur_entry = cache->head;

    hashtable_destroy(cache->index); //both hashtable and linkedlists are destroyed in a nested for loop O(n*m)

    while (cur_entry != NULL)
    {
        struct cache_entry *next_entry = cur_entry->next;

        free_entry(cur_entry);

        cur_entry = next_entry;
    }

    free(cache);
}

/**
 * Store an entry in the cache
 *
 * This will also remove the least-recently-used items as necessary.
 * 
 * NOTE: doesn't check for duplicate cache entries
 */
void cache_put(struct cache *cache, char *path, char *content_type, void *content, int content_length)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////

    //     1. Implement `cache_put()` in `cache.c`.
    //    Algorithm:
    //    * Allocate a new cache entry with the passed parameters.
    struct cache_entry *ce = alloc_entry(path, content_type, content, content_length);
    //    * Insert the entry at the head of the doubly-linked list.
    // struct cache_entry *prior_head = cache->head; //PROBABLY NOT NECESSARY
    // ce->next = cache->head;
    // cache->head = ce;
    dllist_insert_head(cache, ce); // automatically makes head the tail if head == NULL;
    //    * Store the entry in the hashtable as well, indexed by the entry's `path`.
    hashtable_put(cache->index, path, ce); // void pointer because we don't know type of data.
    //    * Increment the current size of the cache.
    cache->cur_size++;
    //    * If the cache size is greater than the max size:
    if (cache->cur_size > cache->max_size)
    {
        //      * Remove the cache entry at the tail of the linked list.
        struct cache_entry *old_tail = dllist_remove_tail(cache);
        // cache->tail = cache->tail->prev;
        //      * Remove that same entry from the hashtable, using the entry's `path` and the `hashtable_delete` function.
        hashtable_delete(cache->index, old_tail->path);
        //      * Free the cache entry.
        free_entry(old_tail);
        //      * Ensure the size counter for the number of entries in the cache is correct.
        cache->cur_size = cache->max_size;
    }
}

/**
 * Retrieve an entry from the cache
 */
struct cache_entry *cache_get(struct cache *cache, char *path)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    // void* data;
    //    Algorithm:
    //    * Attempt to find the cache entry pointer by `path` in the hash table.
    struct cache_entry *ce = malloc(sizeof(struct cache_entry));
    ce = hashtable_get(cache->index, path); // this is only the data portion
    // ce->content = hashtable_get(cache->index, path); // this is only the data portion

    // //    * If not found, return `NULL`.
    // ce->content_type = "image/jpg";
    // // ce->content = data; // this breaks you

    // ce->content_length = 10000;

    // ce->path = path;

    // if (!ce->content)
    // {
    //     printf("NULL\n");
    //     return NULL;
    // }
    //    * Move the cache entry to the head of the doubly-linked list.

    dllist_move_to_head(cache, ce); // breaks here
    printf("HEREasdfadsf\n");

    // ce->next->prev = ce->prev;
    // ce->prev->next = ce->next;
    // ce->next = cache->head;
    // cache->head = ce;
    //    * Return the cache entry pointer.
    printf("HERE\n\n");

    return ce;
}
