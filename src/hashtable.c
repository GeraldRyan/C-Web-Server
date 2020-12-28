/*

Example:

// Create a hash table of 128 elements and use the default hash function

struct hashtable *ht = hashtable_create(128, NULL);

int data1 = 12;
int data2 = 30;

// Store pointers to data in the hash table
// (Data can be pointers to any type of data)

hashtable_put(ht, "some data", &data1);
hashtable_put(ht, "other data", &data2);

// Retrieve data

int *result = hashtable_get(ht, "other data");
printf("%d\n", *r1); // prints 30

// Store a struct:

struct foo *p = malloc(sizeof *p);

p->bar = 12;
p->baz = "Hello";

hashtable_put(ht, "mystruct", p);

struct foo *q = hashtable_get("mystruct");

printf("%d %s\n", q->bar, q->baz); // 12 Hello

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "llist.h"
#include "hashtable.h"

#define DEFAULT_SIZE 128
#define DEFAULT_GROW_FACTOR 2

// Hash table entry
struct htent {
    void *key;
    int key_size;
    int hashed_key;
    void *data;
};

// Used to cleanup the linked lists
struct foreach_callback_payload {
	void *arg;
	void (*f)(void *, void *); 
};

/**
 * Change the entry count, maintain load metrics
 */
void add_entry_count(struct hashtable *ht, int d)
{
    ht->num_entries += d;
    ht->load = (float)ht->num_entries / ht->size;
}

/**
 * Default modulo hashing function
 */
int default_hashf(void *data, int data_size, int bucket_count)
{
    const int R = 31; // Small prime
    int h = 0;
    unsigned char *p = data; 

    for (int i = 0; i < data_size; i++) {
        h = (R * h + p[i]) % bucket_count; // hashes the word (if word) letter per letter. 
    }

    return h;
}

/**
 * Create a new hashtable
 */
struct hashtable *hashtable_create(int size, int (*hashf)(void *, int, int))
{
    if (size < 1) {
        size = DEFAULT_SIZE;
    }

    printf("Hashf pointer to function parm (expect to be NULL) %d \n", hashf);

    if (hashf == NULL) {
        printf("default_hashf pointer to function value (address?) %d\n", default_hashf); // added this for learning
        printf("addres of this 'hashtable_create' function (if inputting it wtihotu invoking it) %d\n", hashtable_create); // added this for learning

        hashf = default_hashf;
    }
    struct hashtable *ht = malloc(sizeof *ht);  

    if (ht == NULL) return NULL; // not sure this would ever be, but best practice?

    // initialization or structure construction (compare to class constructor method)
    ht->size = size;
    ht->num_entries = 0;
    ht->load = 0;
    ht->bucket = malloc(size * sizeof(struct llist *));
    ht->hashf = hashf;  // the struct contains its own hashfunction function attached? There will be only one hashtable. It's method is attached, that is a pointer to a memory address of executable code that can be invoked when the signature matches.. the memory being available to the main code

    // doesn't actually invoke hash function, just "attaches" it

    for (int i = 0; i < size; i++) {
        ht->bucket[i] = llist_create();  // llist_create returns a pointer to LL. It takes no args. All it does is Calloc a linkedlist as defined in llist.h. The llist contains a llnode and a count. The llNodes contain a pointer to data and a pointer to the next, so it basically contains a count of nodes nad the head node, which is strung to all the others. So this function creates an empty LinkedList for each index of the hashtable. BASICALLY THIS CALLOCS A LINKEDLIST WHICH IS A HEAD NODE AND A COUNT, AND THAT'S WHAT CREATES THE 'SIZE' PROPERTY. 
    }

    return ht;
}

/**
 * Free an htent
 */
void htent_free(void *htent, void *arg)
{
	(void)arg; // not sure what this does.

	free(htent); // free whatever htent is 
}

/**
 * Destroy a hashtable
 *
 * NOTE: does *not* free the data pointer
 */
void hashtable_destroy(struct hashtable *ht)
{
    for (int i = 0; i < ht->size; i++) {
        struct llist *llist = ht->bucket[i];

		llist_foreach(llist, htent_free, NULL); // the anon function passed in is htent_free, which is not invoked here. 
        // It takes two void pointers but they are passed down inside the anon function f itself. All the compiler needs is the memory address of the anon funct. Think always of what the compiler needs. It just needs memory address of executable code. 
        // This is kind of just like JavaScript. Now you can see how these funcitons are being implemented. 
        // First frees the memory for each node in a linked list
        llist_destroy(llist); // and then destroys the linkedlist itself
        // and then goes to the next index of the hashtable. 
        // so basically this is totally hashtable destructive, completely destroying (freeing) everything in it. Zero memory leaks. 
        // Time complexity and space complexity, including memory leak plugging. 
    }

    free(ht);
}

/**
 * Put to hash table with a string key
 */
void *hashtable_put(struct hashtable *ht, char *key, void *data) // good that we're just given key. We can hash elsewhere.
{
    return hashtable_put_bin(ht, key, strlen(key), data);
}

/**
 * Put to hash table with a binary key
 */
void *hashtable_put_bin(struct hashtable *ht, void *key, int key_size, void *data)
{
    int index = ht->hashf(key, key_size, ht->size); // This is where 'attached' p2f is invoked (with correct arg set)
    // the key is not the index. The key transforms (hashes) into the index. Big difference. Why can't you just use key as index straight?  Run the hash via the function provided by hash table

    struct llist *llist = ht->bucket[index]; // A linkedlist size memory space is already there, via calloc. It could be null but it's there. We can check,a dn then append 

    struct htent *ent = malloc(sizeof *ent); // this struct defined above (lots of structs, lots of structs. structs structs structs. Know thy struct). This struct defined above has a key, keysize, hashedkey and data itself. Why does author want to store all this data in an entry? It is common practice? This is a design pattern to be scoured, confirmed by further observation. I can imagine uses or importance. 
    ent->key = malloc(key_size);  // void pointer-- so we don't know the size. THe size of the pointer is known but not what is pointed to. malloc doesn't take a pointer as arg, unlike free. It just takes a size_t. It knows it's only run on pointers because only they are on heap. I could be wrong with these 'absolutes' but I'm at least predominately right. So malloc doens't take a pointer but it returns a void pointer. So it returns a pointer so ent->key is a void pointer that's given a range of memory. This makes sense. The key_size size_t sets the range. This makes so much sense. 
    memcpy(ent->key, key, key_size); // returns void pointer (but we're not catching it. Why? Because we already have it? It's not really necessary with this I think- they just offer it for those who want it. Basically copy key into ent->key for key_size length. Any time we want to save size of thing, it could be bc of memcpy or similar. Take note. And these things have to be exact. Be worth a good salary, or even business creation ability, via partnership or single, but probably partnership. Be worth. Be salty. 
    ent->key_size = key_size; // int
    ent->hashed_key = index; // int 
    ent->data = data; // void pointer

    if (llist_append(llist, ent) == NULL) {
        free(ent->key);
        free(ent);
        return NULL;
    }

    add_entry_count(ht, +1);

    return data;
}

/**
 * Comparison function for hashtable entries
 */
int htcmp(void *a, void *b)
{
    struct htent *entA = a, *entB = b;

    int size_diff = entB->key_size - entA->key_size;

    if (size_diff) {
        return size_diff;
    }

    return memcmp(entA->key, entB->key, entA->key_size);
}

/**
 * Get from the hash table with a string key
 */
void *hashtable_get(struct hashtable *ht, char *key)
{
    return hashtable_get_bin(ht, key, strlen(key));
}

/**
 * Get from the hash table with a binary data key
 */
void *hashtable_get_bin(struct hashtable *ht, void *key, int key_size)
{
    int index = ht->hashf(key, key_size, ht->size);

    struct llist *llist = ht->bucket[index];

    struct htent cmpent;
    cmpent.key = key;
    cmpent.key_size = key_size;

    struct htent *n = llist_find(llist, &cmpent, htcmp);

    if (n == NULL) { return NULL; }

    return n->data;
}

/**
 * Delete from the hashtable by string key
 */
void *hashtable_delete(struct hashtable *ht, char *key)
{
    return hashtable_delete_bin(ht, key, strlen(key));
}

/**
 * Delete from the hashtable by binary key
 *
 * NOTE: does *not* free the data--just free's the hash table entry
 */
void *hashtable_delete_bin(struct hashtable *ht, void *key, int key_size)
{
    int index = ht->hashf(key, key_size, ht->size);

    struct llist *llist = ht->bucket[index];

    struct htent cmpent;
    cmpent.key = key;
    cmpent.key_size = key_size;

    struct htent *ent = llist_delete(llist, &cmpent, htcmp);

	if (ent == NULL) {
		return NULL;
	}

	void *data = ent->data;

	free(ent);

    add_entry_count(ht, -1);

	return data;
}

/**
 * Foreach callback function
 */
void foreach_callback(void *vent, void *vpayload)
{
	struct htent *ent = vent;
	struct foreach_callback_payload *payload = vpayload;

	payload->f(ent->data, payload->arg);
}

/**
 * For-each element in the hashtable
 *
 * Note: elements are returned in effectively random order.
 */
void hashtable_foreach(struct hashtable *ht, void (*f)(void *, void *), void *arg)
{
	struct foreach_callback_payload payload;

	payload.f = f;
	payload.arg = arg;

	for (int i = 0; i < ht->size; i++) {
		struct llist *llist = ht->bucket[i];

		llist_foreach(llist, foreach_callback, &payload);
	}
}
