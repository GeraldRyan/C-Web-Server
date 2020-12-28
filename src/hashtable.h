#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

struct hashtable { // sizeof knows the size of this whole entity. 
    int size; // Read-only
    int num_entries; // Read-only
    float load; // Read-only
    struct llist **bucket; // DOUBLE POINTER!! Is this to make it an array of linked lists? that's what I believe. I don't fully understand how exactly it does that, but I'm sure that's what it's doing. Yes it's a pointer to a memory address of a linkedlist. If the next pointer is not null, it will find the next Linked list. It is starting to make sense I think. This is an array of linked lists of indefinite size. A pointer is a location in memory that signifies the beginning of something (oftentimes), and the ending is determined either by a 'length' var or a null character. Pointers are small in size. A pointer to a pointer is an array of pointers. Every time I see a double pointer, I have to think of an array of pointers. Since pointers can point themselves to an array-- or to structs or to anything of variable size, a pointer to a pointer can be an array of arrays, or an array of structs. In this case it's the latter- namely an array of linked lists. We can have a third pointer. It ends (by convention-- if we were to loop) when there is a null termination, or something. This is how memory works in C and C underlies much code. So bottom line, the 'bucket' property of the hashtable struct, which is a struct itself, is in fact a pointer to an array of struct pointers. An array of struct pointers, either of which must I think have been malloced somewhere.  
    int (*hashf)(void *data, int data_size, int bucket_count); // pointer to function. This makes structs similar to classes, giving them connected functions (methods). So it will return an int each time. It's like an empty chamber.
};

extern struct hashtable *hashtable_create(int size, int (*hashf)(void *, int, int));
extern void hashtable_destroy(struct hashtable *ht);
extern void *hashtable_put(struct hashtable *ht, char *key, void *data);
extern void *hashtable_put_bin(struct hashtable *ht, void *key, int key_size, void *data);
extern void *hashtable_get(struct hashtable *ht, char *key);
extern void *hashtable_get_bin(struct hashtable *ht, void *key, int key_size);
extern void *hashtable_delete(struct hashtable *ht, char *key);
extern void *hashtable_delete_bin(struct hashtable *ht, void *key, int key_size);
extern void hashtable_foreach(struct hashtable *ht, void (*f)(void *, void *), void *arg);

#endif
