#include <stdlib.h>
#include "llist.h"

struct llist_node {
	void *data; // because it's a pointer, needs to be freed, because pointers live on stack. normal decs live on stack. 
	struct llist_node *next; // this is freed in other function. It's a level higher than data so the 2nd runner function. 
};

/**
 * Allocate a new linked list
 */
struct llist *llist_create(void)
{
	return calloc(1, sizeof(struct llist));  // creates 1 item, sets to zero bc that's what calloc is. 
	// the calloc call doesn't 'know' we're creating a linked list. It just knows to reserve space in memory that is the exact size of it. This llist_create function doth return a llist struct but the calloc function doesn't know. Just the size has to be correct. Think in terms of memory. See what's there, with the mind's eye. 
}

/**
 * Destroy a linked list
 *
 * If destfn is not NULL, it is called with the node data before the
 * node is deallocated.
 *
 * NOTE: does *not* deallocate the data in each node!!
 */
void llist_destroy(struct llist *llist)
{
	struct llist_node *n = llist->head, *next; // n is memory address- not address of variable holding memory address. it is already dereferenced as 'free()' takes a pointer. It knows. Could have said llist->head I think. Different pointer but same memory address, I'm thinking. 

	while (n != NULL) {
		next = n->next;
		free(n); // this function obviously frees all linkedlist nodes and doesn't require any args except ll iself. 

		n = next;
	}

	free(llist);  // free each and then finish by freeing list itself. 
	
	// hint for future- count the levels of data (linked items are same level as each other- number of levels underneath plus what is linked itself means number of times have to use a for or while loop). See patterns. Even better when self-discovered, not just read or memorized. 
}

/**
 * Insert at the head of a linked list
 */
void *llist_insert(struct llist *llist, void *data)
{
	struct llist_node *n = calloc(1, sizeof *n);

	if (n == NULL) {
		return NULL;
	}

	n->data = data;
	n->next = llist->head;
	llist->head = n;

	llist->count++;

	return data;
}

/**
 * Append to the end of a list
 */
void *llist_append(struct llist *llist, void *data)  // data pointer is struct htent. 
{
	struct llist_node *tail = llist->head;

	// If list is empty, just insert
	if (tail == NULL) { // already calloced empty spot. 
		return llist_insert(llist, data);
	}

	struct llist_node *n = calloc(1, sizeof *n);

	if (n == NULL) {
		return NULL;
	}

	while (tail->next != NULL) {
		tail = tail->next;
	}

	n->data = data; // ok so the LLnode carries an htent. So the hashtable carries linked lists, and the linked lists carry nodes and hte nodes carry htents. One htent per node I suppose. Nested structs all the way down. It's in the node's data property, and htent also has a data property, so node->data->data, which is lord konws what- our main user type of data. What was this project again?  
	tail->next = n;  // this 'tail' is just a running marker. It's not being stored anywhere as tail, so we don't have to redefine it. Just pass a next object, n. 

	llist->count++;

	return data;
}

/**
 * Return the first element in a list
 */
void *llist_head(struct llist *llist)
{
	if (llist->head == NULL) {
		return NULL;
	}

	return llist->head->data;
}

/**
 * Return the last element in a list
 */
void *llist_tail(struct llist *llist)  
{
	struct llist_node *n = llist->head;

	if (n == NULL) {
		return NULL;
	}

	while (n->next != NULL) {
		n = n->next;
	}

	return n->data;
}

/**
 * Find an element in the list
 *
 * cmpfn should return 0 if the comparison to this node's data is equal.
 */
void *llist_find(struct llist *llist, void *data, int (*cmpfn)(void *, void *)) // void pointer as 2nd arg. Can be anything. It's htent pointer as seen in the wild. What is htent doing? Why that and llnode?  
{
	struct llist_node *n = llist->head;

	if (n == NULL) {
		return NULL; // nothing was found in this linked list. 
	}

	while (n != NULL) {
		if (cmpfn(data, n->data) == 0) { // comparing each one, passing data object and the given data of the LL. 0 is hit.  
			break;
		}

		n = n->next;
	}

	if (n == NULL) {
		return NULL;
	}

	return n->data;
}

/**
 * Delete an element in the list
 *
 * cmpfn should return 0 if the comparison to this node's data is equal.
 *
 * NOTE: does *not* free the data--it merely returns a pointer to it
 */
void *llist_delete(struct llist *llist, void *data, int (*cmpfn)(void *, void *))
{
	struct llist_node *n = llist->head, *prev = NULL;

	while (n != NULL) {
		if (cmpfn(data, n->data) == 0) {

			void *data = n->data;

			if (prev == NULL) {
				// Free the head
				llist->head = n->next;
				free(n);

			} else {
				// Free the non-head
				prev->next = n->next;
				free(n);
			}

			llist->count--;

			return data;
		}

		prev = n;
		n = n->next;
	}

	return NULL;
}

/**
 * Return the number of elements in the list
 */
int llist_count(struct llist *llist)
{
	return llist->count;
}

/**
 * For each item in the list run a function
 */
void llist_foreach(struct llist *llist, void (*f)(void *, void *), void *arg) // void(.f)(arg1, arg2) is what makes this a true "For Each" function. Is this how for each functions are implemented in higher level languages that are built by C? I would expect pointers including pointers to functions to be used heavily.  This functions job is just to pass in arguments to an anon function for each item in a linked list (and yes it has to be a linked list)
{
	struct llist_node *p = llist->head, *next; // basically runs anon function on each item in a LL, passing data and another arg into anon function. Two ways to do for loops to max. If you know max value, do the usual. Otherwise do while not NULL, in LL case. See this pattern. Momorize it. 

	while (p != NULL) {
		next = p->next;
		f(p->data, arg);  // Calls the function provided with the data of p and the args.   
		p = next;
	}
	// when hashtable_destroy calls this function, it basically frees all the memory (garbage collects) for each item in a given linkedlist. 
}

/**
 * Allocates and returns a new NULL-terminated array of pointers to data
 * elements in the list.
 *
 * NOTE: This is a read-only array! Consider it an array view onto the linked
 * list.
 */
void **llist_array_get(struct llist *llist)
{
	if (llist->head == NULL) {
		return NULL;
	}

	void **a = malloc(sizeof *a * llist->count + 1);

	struct llist_node *n;
	int i;

	for (i = 0, n = llist->head; n != NULL; i++, n = n->next) {
		a[i] = n->data;
	}

	a[i] = NULL;

	return a;
}

/**
 * Free an array allocated with llist_array_get().
 * 
 * NOTE: this does not modify the linked list or its data in any way.
 */
void llist_array_free(void **a)
{
	free(a);
}
