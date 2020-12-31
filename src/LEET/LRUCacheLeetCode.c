#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct QNode
{
  struct QNode *prev, *next;
  unsigned bucket;
  int key;
  int data;
} QNode;

typedef struct Queue
{
  unsigned count;
  unsigned capacity;
  QNode *front, *rear;
} Queue;

typedef struct Hash
{
  int capacity;
  QNode **array; // array of pointers, or pointer to a range of pointers.
} Hash;

typedef struct LRUCache
{
  unsigned int capacity;
  Queue *queue;
  Hash *hash;

} LRUCache;

struct QNode *newQNode(unsigned bucket)
{
  struct QNode *temp = malloc(sizeof(struct QNode));
  temp->bucket = bucket;
  temp->prev = temp->next = NULL;
  return temp;
}

struct Queue *createQueue(int capacity)
{
  struct Queue *queue = malloc(sizeof(struct Queue));
  queue->count = 0;
  queue->capacity = capacity;
  queue->front = queue->rear = NULL;
  return queue;
}

struct Hash *createHash(int capacity)
{
  struct Hash *hash = malloc(sizeof(struct Hash));
  hash->capacity = capacity;
  hash->array = malloc(sizeof(struct QNode) * hash->capacity);
  for (int i = 0; i < hash->capacity; i++)
  {
    hash->array[i] = NULL;
  }
  return hash;
}

unsigned int hashfunc(unsigned capacity, int key);
void Enqueue(struct LRUCache *cache, struct Queue *queue, struct Hash *hash, int value, int key);
int searchCache(struct LRUCache *cache, int key);

void deQueue(Queue *queue)
{ // we could have return QNode but how would affect memory mgmt?
  if (queue->rear == NULL)
  {
    // nothing to do
    return;
  }
  if (queue->front == queue->rear)
  { // LL of one, as long as front and rear are properly pointed
    queue->front = NULL;
    queue->rear = NULL;
    queue->count--;
    return;
  }
  // QNode* temp = queue->rear;
  // LL two or more
  queue->rear = queue->rear->prev; // could be front if size = 2. That's fine
  if (queue->rear)
  {
    queue->rear->next = NULL; // orphan the remaining, no need for memory freeing until it runs
  }
  queue->count--;
  return;
  // free(temp); // This creates an AddressSanitizer error heap-use-after-free.
}

void dehash(Queue *queue, Hash *hash, unsigned bucket, int key)
{                                                       // sets hash to null. Doesn't free
  unsigned test_bucket = hashfunc(hash->capacity, key); // should be same as passed in. for checking
  if (test_bucket != bucket)
  {
    printf("Warning, hash values coming out differently in dehash function\n");
  }
  if (hash->array[bucket])
  { // we have to be certain this is right bucket adjusted for collision
    if (hash->array[bucket]->key == key)
    { // double check for debug
      // QNode* tmp = hash->array[bucket];
      hash->array[bucket] = NULL;
      // free(tmp);
    }
    else
      printf("entry found in hash but keys don't match. No action taken.\n");
    return;
  }
}

unsigned getGoodBucket(Hash *hash, int key)
{
  unsigned try_bucket = hashfunc(hash->capacity, key);
  if (hash->array[try_bucket] == NULL)
    return try_bucket;
  unsigned original_try = try_bucket;
  while (hash->array[try_bucket] != NULL)
  { // spot is taken
    try_bucket = (try_bucket + 1) % hash->capacity;
    if (try_bucket == original_try)
    { // no space found (erroneously I think)
      printf("No space found in hash for new node, looped through everything\n");
    }
  }
  return try_bucket; // hope this works no collisions.
}

void Enqueue(struct LRUCache *cache, struct Queue *queue, struct Hash *hash, int value, int key)
{ // also adds to hash
  // check if exists already in cache
  int bucket = searchCache(cache, key); // bucket can be -1 not found or bucket where exists
  printf("Enqueuing key %d\nbucket is: %d\n", key, bucket);
  // dequeue if full
  if (queue->count == queue->capacity && bucket < 0)
  {                                            // if full and new one needs enter
    unsigned hashbucket = queue->rear->bucket; // order of operation important
    deQueue(queue);                            // if empty, no action, if front=rear set both null, owise remove rear
    dehash(queue, hash, hashbucket, key);
  }

  if (bucket == -1)
  { // if brand new node. bucket state already verified matching keys
    // create new node
    QNode *temp = newQNode(0);
    temp->data = value; // obviously have to put the value in there
    temp->key = key;    // needed for handling hash collisions
    int goodBucket = getGoodBucket(hash, key);
    printf("GoodBucket to hash index on %d\n", goodBucket);
    temp->bucket = goodBucket; // Get it a bucket
    temp->prev = NULL;         // because it will go at head

    // attach node
    if (queue->rear == NULL)
    { // should mean queue is empty but double check
      if (queue->front != NULL)
      {
        printf("Rear null but head not, error\n");
      }
      queue->rear = temp;
      queue->front = temp;
    }
    else if (queue->front == queue->rear)
    {
      queue->front = temp;
      queue->front->next = queue->rear;
      // should already be pointing to
    }
    hash->array[goodBucket] = temp; // pass to hash
    queue->count++;
    return;
  }
  else if (bucket >= 0)
  { // explicitly double checked, existing in cache
    if (hash->array[bucket] == queue->front)
    { // Already at head, don't need to do anything.
      return;
    }
    else if (queue->front == queue->rear)
    { // shouldn't be triggered based on above condition.
      printf("This should never run. Should be handled by the above condition. Implying error\n");
      printf("Q count should be 1. Queue count is %d\n", queue->count);
      return;
    }
    else
    {                                   // if in hash, should(!!) also be in the queue. If not, VERY BAD. Not super easy to check
      QNode *tmp = hash->array[bucket]; // pointer to the node in question
      tmp->prev->next = tmp->next;      // next can be NULL or not null, doesn't matter
      tmp->next->prev = tmp->prev;      // now the neighbors are reassociated
      tmp->next = queue->front;
      tmp->prev = NULL;   // Now the node in question is right pointed
      queue->front = tmp; // queue front could point to queue->rear, or to an intermediary
      return;             // queue count doesn't change. we're done? Will it work?
    }
  }
}

unsigned int hashfunc(unsigned capacity, int key)
{
  unsigned int provisional_bucket = (key * 353) % capacity;
  return provisional_bucket; // may need adjustment (linear probing)
}

int searchCache(struct LRUCache *cache, int key)
{
  // look in hash for item of "key"; double check key=key in case of collision
  unsigned test_bucket = hashfunc(cache->capacity, key);
  if (cache->hash->array[test_bucket] != NULL)
  {
    if (cache->hash->array[test_bucket]->key == key)
    {
      return test_bucket; // simplest case, returns location
    }
    else
    { // linear probe time
      unsigned original_test_bucket = test_bucket;
      while (cache->hash->array[test_bucket] != NULL)
      {
        test_bucket = ((test_bucket + 1) % cache->capacity);
        if (test_bucket == original_test_bucket)
        { // back to start, avoid infinite looping
          printf("Probed hash, object not found\n");
          return -1; // returns -1
        }
        if (cache->hash->array[test_bucket]->key == key)
        {                     // found our guy via linear probing
          return test_bucket; // returns adjusted bucket
        }
      }
    }
  }
  else
  { // just have else for good measure. Simply not found in array
    return -1;
  }
  return -1;
}

void freeQueue(Queue *queue);
void freeHash(Hash *hash);
void freeQNode(QNode *qnode);

struct LRUCache *lRUCacheCreate(int capacity)
{
  struct LRUCache *cache = malloc(sizeof(struct LRUCache));
  cache->capacity = capacity;
  cache->queue = createQueue(capacity);
  cache->hash = createHash(capacity);
  return cache;
} // THIS IS WHERE THE ERROR HAPPEN(ED) -- fixed- (sort of), but not understood

int lRUCacheGet(struct LRUCache *obj, int key)
{
  int location = searchCache(obj, key);
  if (location >= 0)
  {                                                      // found, else searchCache returns -1
    struct QNode *tempNode = obj->hash->array[location]; // found in hash  SEG FAULT HERE!!
    int tempData = tempNode->data;                       // Preserve Data before enqeueing
    Enqueue(obj, obj->queue, obj->hash, tempData, key);  // TODO redundantly recalculates hash. Should be separate funtion "reorder" or "move_to_head"?
    return tempData;
  }
  else
  {
    return location; // is -1 already
  }
}

void lRUCachePut(struct LRUCache *obj, int key, int value)
{
  // unsigned bucket = hashfunc(obj->capacity, key); // gets original bucket
  Enqueue(obj, obj->queue, obj->hash, value, key); // also dequeues/dehashes, adds to hash, moves to front
}

void lRUCacheFree(struct LRUCache *obj)
{
  freeQueue(obj->queue);
  freeHash(obj->hash);
  free(obj);
  return;
}

/**
 * Your LRUCache struct will be instantiated and called as such:
 * LRUCache* obj = lRUCacheCreate(capacity);
 * int param_1 = lRUCacheGet(obj, key);
 
 * lRUCachePut(obj, key, value);
 
 * lRUCacheFree(obj);
*/

void freeQueue(struct Queue *queue)
{
  struct QNode *curr = queue->front;
  while (curr)
  {
    QNode *tmp = curr;
    curr = curr->next;
    free(tmp);
  }
  free(queue);
}
void freeHash(Hash *hash)
{ // if you free the nodes associated with the hash in the linked list, you also free them in the hash right? Because they're pointed at. They'll point at null or rather out of range things (seg fault territory?)
  free(hash);
}
void freeQNode(QNode *qnode);

void printCurrentState(LRUCache *cache, char *op)
{
  printf("CURRENT STATE after %s::::::::::\n\n", op);
  printf("cache has capacity of %d\n", cache->capacity);
  QNode *n = cache->queue->front;
  int c = 0;
  while (n != NULL)
  {
    printf("QueueNode %d data = %d\n", c, n->data);
    c++;
    n = n->next;
  }
  for (int i = 0; i < cache->capacity; i++)
  {
    if (cache->hash->array[i] != NULL) printf("Hashtable[%d] = %d\n", i, cache->hash->array[i]->data);
  }
}

int main(void)
{
  struct LRUCache *lRUCache = lRUCacheCreate(2);
  lRUCachePut(lRUCache, 1, 1); // cache is {1=1}
  printCurrentState(lRUCache, "put 1, 1");
  lRUCachePut(lRUCache, 2, 2); // cache is {1=1, 2=2}
  printCurrentState(lRUCache, "put 2, 2");
  lRUCacheGet(lRUCache, 1); // return 1
  printCurrentState(lRUCache, "get 1");
  lRUCachePut(lRUCache, 3, 3); // LRU key was 2, evicts key 2, cache is {1=1, 3=3}
  printCurrentState(lRUCache, "put 3 3");
  lRUCacheGet(lRUCache, 2); // returns -1 (not found)
  printCurrentState(lRUCache, "get 2");
  lRUCachePut(lRUCache, 4, 4); // LRU key was 1, evicts key 1, cache is {4=4, 3=3}
  printCurrentState(lRUCache, "put 4 4");
  lRUCacheGet(lRUCache, 1); // return -1 (not found)
  printCurrentState(lRUCache, "get 1");
  lRUCacheGet(lRUCache, 3); // return 3
  printCurrentState(lRUCache, "get 3");
  lRUCacheGet(lRUCache, 4); // return 4

  return 1;
}