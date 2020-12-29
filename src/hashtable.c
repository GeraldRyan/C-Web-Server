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
struct htent
{
    void *key;
    int key_size;
    int hashed_key;
    void *data;
};

// Used to cleanup the linked lists
struct foreach_callback_payload
{
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

    for (int i = 0; i < data_size; i++)
    {
        h = (R * h + p[i]) % bucket_count; // hashes the word (if word) letter per letter.
    }

    return h; // does every famous hash table use the mod function? I can see it being the case. I can also see it possibly not being the case.
}

/**
 * Create a new hashtable
 */
struct hashtable *hashtable_create(int size, int (*hashf)(void *, int, int))
{
    if (size < 1)
    {
        size = DEFAULT_SIZE;
    }

    if (hashf == NULL)
    {
        hashf = default_hashf;
    }
    struct hashtable *ht = malloc(sizeof *ht);

    if (ht == NULL)
        return NULL; // not sure this would ever be, but best practice?

    // initialization or structure construction (compare to class constructor method)
    ht->size = size;
    ht->num_entries = 0;
    ht->load = 0;
    ht->bucket = malloc(size * sizeof(struct llist *));
    ht->hashf = hashf; // the struct contains its own hashfunction function attached? There will be only one hashtable. It's method is attached, that is a pointer to a memory address of executable code that can be invoked when the signature matches.. the memory being available to the main code

    // doesn't actually invoke hash function, just "attaches" it

    for (int i = 0; i < size; i++)
    {
        ht->bucket[i] = llist_create(); // llist_create returns a pointer to LL. It takes no args. All it does is Calloc a linkedlist as defined in llist.h. The llist contains a llnode and a count. The llNodes contain a pointer to data and a pointer to the next, so it basically contains a count of nodes nad the head node, which is strung to all the others. So this function creates an empty LinkedList for each index of the hashtable. BASICALLY THIS CALLOCS A LINKEDLIST WHICH IS A HEAD NODE AND A COUNT, AND THAT'S WHAT CREATES THE 'SIZE' PROPERTY.
    }

    return ht;
}

/**
 * Free an htent
 */
void htent_free(void *htent, void *arg)
{
    (void)arg; 

    free(htent); 
}

/**
 * Destroy a hashtable
 *
 * NOTE: does *not* free the data pointer
 */
//In fact destroys both hashtable and associated LinkedList.
void hashtable_destroy(struct hashtable *ht)
{
    for (int i = 0; i < ht->size; i++)
    {
        struct llist *llist = ht->bucket[i];

        llist_foreach(llist, htent_free, NULL); // See the forest for the trees
        llist_destroy(llist);                   // and then destroys the linkedlist itself
        // and then goes to the next index of the hashtable.
        // so basically this is totally hashtable destructive, completely destroying (freeing) everything in it. Zero memory leaks.
        // Time complexity and space complexity, including memory leak plugging.
    }
    free(ht); 
}

/**
 * Put to hash table with a string key
 */
void *hashtable_put(struct hashtable *ht, char *key, void *data)
{
    return hashtable_put_bin(ht, key, strlen(key), data);
}

/**
 * Put to hash table with a binary key
 */
void *hashtable_put_bin(struct hashtable *ht, void *key, int key_size, void *data)
{
    int index = ht->hashf(key, key_size, ht->size); // hashes index

    struct llist *llist = ht->bucket[index]; // A linkedlist size memory space is already there, via calloc. It could be null but it's there. We can check,a dn then append. So the ht->bucket was a double pointer or a pointer to a pointer, an array of pointers to structs. there are pointers to functions and pointers to structs and pointers to whatever. I knew this day would come. So to access the bucket property- that is easy. You are accessing a pointer (to an array of pointers) by accessing bucket. In this case we are going one level down, we are indexing, getting a specific struct pointer, I think. Struct 578 or something. My only curiosity is the bracket syntax. I'm sure I'm right and C just has that. That would mean index has a range. Of course you can get the index out of range error, that common error, if you go out of range. It knows the size of each element, I'm sure, because we calloced or malloced it. So we get the pointer to an array of pointers and then choose our chosen pointer out of them to get our chosen struct. Our chosen pointer is determined by our index, which is our hashed key, by calling the hash function attached to the struct as a pointer to a function, a pointer to executable (invocable) code. This is genius but it is the result of much focus, many design patterns. Don't be a worshipper. People do it from focus, out of need and desire, for money incentive and curiosity. Don't worship brilliance. Become brilliant.

    struct htent *ent = malloc(sizeof *ent); // this struct defined above (lots of structs, lots of structs. structs structs structs. Know thy struct). This struct defined above has a key, keysize, hashedkey and data itself. Why does author want to store all this data in an entry? It is common practice? This is a design pattern to be scoured, confirmed by further observation. I can imagine uses or importance.
    ent->key = malloc(key_size);             // void pointer-- so we don't know the size. THe size of the pointer is known but not what is pointed to. malloc doesn't take a pointer as arg, unlike free. It just takes a size_t. It knows it's only run on pointers because only they are on heap. I could be wrong with these 'absolutes' but I'm at least predominately right. So malloc doens't take a pointer but it returns a void pointer. So it returns a pointer so ent->key is a void pointer that's given a range of memory. This makes sense. The key_size size_t sets the range. This makes so much sense.
    memcpy(ent->key, key, key_size);         // returns void pointer (but we're not catching it. Why? Because we already have it? It's not really necessary with this I think- they just offer it for those who want it. Basically copy key into ent->key for key_size length. Any time we want to save size of thing, it could be bc of memcpy or similar. Take note. And these things have to be exact. Be worth a good salary, or even business creation ability, via partnership or single, but probably partnership. Be worth. Be salty.
    // so going back, first we had to malloc the key space with key_size and then we had to memcpy. Is this a pattern? Both times we used key_size. This has to be a pattern. Look out for malloc followed by memcpy with same final arg.
    ent->key_size = key_size; // Now we just attach this int to the entry struct for future reference.
    ent->hashed_key = index;  // int that was already hashed above. attached function invoked
    ent->data = data;         // void pointer. Where is this malloc'd? We have to malloc key so why not data?

    if (llist_append(llist, ent) == NULL)
    { // appending an htent onto linked list.
        free(ent->key);
        free(ent);
        return NULL;
    }

    add_entry_count(ht, +1); // I can predict what this does. But does plus belong in the arg? I guess it is like negative one, they could just say one?

    return data; // this whole function returns a void pointer - actually the same it was passed in. Well that's if it ran through error free. It's just a convenience offering I suppose.
}

/**
 * Comparison function for hashtable entries
 */
int htcmp(void *a, void *b)
{
    struct htent *entA = a, *entB = b;

    int size_diff = entB->key_size - entA->key_size; // is this byte size or number value size? I think the latter.

    if (size_diff)
    { // if the latter, it will always catch a difference. Iin which case, memcpy will rarely run.
        return size_diff;
    }

    return memcmp(entA->key, entB->key, entA->key_size); // returns pointer to entA->key. // why would we want to overright keys? I thought we were just comparing.
}

/**
 * Get from the hash table with a string key
 */
void *hashtable_get(struct hashtable *ht, char *key)
{
    return hashtable_get_bin(ht, key, strlen(key)); // this definition is so straightforward it hardly needs any analysis-- but peeking at the implementation requires some.
}

/**
 * Get from the hash table with a binary data key
 */
void *hashtable_get_bin(struct hashtable *ht, void *key, int key_size)
{
    int index = ht->hashf(key, key_size, ht->size); // get the index

    struct llist *llist = ht->bucket[index]; // create and initialize the linked list from the bucket at index position.

    struct htent cmpent;
    cmpent.key = key;
    cmpent.key_size = key_size;

    struct htent *n = llist_find(llist, &cmpent, htcmp); // so htent contains key and keylenght. How are they used compared to linkedlist nodes? Both store data. Htent stores keys as well. Ah, htent IS the data. It goes in the data. Why then does it have a data property? Is it data of data? Payload type concept?

    if (n == NULL)
    {
        return NULL;
    }

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

    if (ent == NULL)
    {
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
void foreach_callback(void *vent, void *vpayload) // WHAT DOES THIS DO!!!???
{
    printf("Line 281 in hashtable.c Memory address of Vpayload %d\n", vpayload);

    struct htent *ent = vent;                            // we declare a new struct and make it the first arg. Why the indirection?
    struct foreach_callback_payload *payload = vpayload; // why the indirection?
    // Struct Definition above. Used to cleanup the linked lists
    // struct foreach_callback_payload {
    // 	void *arg;
    // 	void (*f)(void *, void *);
    // };
    printf("Line 290 in hashtable.c Memory address of payload (no v prefix) %d\n", payload);
    // if these two addresses are equal, then it tells me something (confirms) and it means this functino's job is to attach an anon function to the foreach_callback payload. So where is this called? IT IS NOT CALLED ANYWHERE! this investigation may have to be orphaned

    payload->f(ent->data, payload->arg); // you access the f pointer to function. You invoke it with parentheses, passing two void pointers/ (v)ent->data and (v)payload->arg. So it's calling an attached pointed to function passing its own arg.
    // the parenthesis invoke (execute) the executable code in memory. Remember that. How does the system know?
}

/**
 * For-each element in the hashtable
 *
 * Note: elements are returned in effectively random order.
 */
void hashtable_foreach(struct hashtable *ht, void (*f)(void *, void *), void *arg) // stay big picture this time. It takes a hashtable. It must iterate at least top level. Maybe it goes deeper but maybe it doesn't. It goes through each index or bucket of the table just from intuition. For each one it runs an anon function. This is just like javascript. You pass it an anonymous function. Now it is starting to click. I don't konw if JS engines were built from C but the concepts and principles are recurring, still foundational. Didn't know there were callback functions in C. Now you know.
// so a Pointer to Function big picture is a callback function, because it is a function passed as argument to other by Reference. Remember by value and by reference? Those old two birds. That's how simple it is.
// So in the definition above, we're given a hashtable and a callback function (pointer to function) to call with two params.
{
    struct foreach_callback_payload payload; // here we're declaring a payload struct as in the function above, but we're never calling that function above. Instead we'er populating it with the arg parameters.

    // so receiving a callback function a function can do one of two things with it. It can invoke it or it can attach it (or technically it can also ignore it, conditionally or unconditionally). But in this case it attaches it, to the payload object it creates (one time) and then for each bucket in the ht it calls LLforeach and passes the payload. So basically for each bucket it calls llforeach- so basically a foreach inside a foreach passing one and the same payload to each.
    payload.f = f;
    payload.arg = arg;

    for (int i = 0; i < ht->size; i++)
    {
        struct llist *llist = ht->bucket[i]; // has memory adds of current LL bucket.

        llist_foreach(llist, foreach_callback, &payload); // &payload is the args required of the llforeach
    }
}
