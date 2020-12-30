typedef struct QNode {
    struct QNode *prev, *next;
    unsigned bucket;
    int data;
} QNode;

typedef struct Queue{
    unsigned count;
    unsigned numberOfFrames;
    QNode *front, *rear;    
} Queue;

typedef struct Hash{
    int capacity;
    QNode **array; // array of pointers, or pointer to a range of pointers. 
} Hash;

typedef struct LRUCache{
    unsigned int capacity;
    Queue* queue;
    Hash* hash;
    
} LRUCache;

QNode* newQNode(unsigned bucket){
    QNode* temp = malloc(sizeof(struct QNode));
    temp->bucket = bucket;
    temp->prev = temp->next = NULL;
    return temp;
}

Queue* createQueue(int numberOfFrames){
    Queue *queue = malloc(sizeof(struct Queue));
    queue->count = 0;
    queue->numberOfFrames = numberOfFrames;
    queue->front = queue->rear = NULL;
    return queue;
}

Hash* createHash(int capacity){
    Hash* hash = malloc(sizeof(struct Hash));
    hash->capacity = capacity;
    hash->array = malloc(sizeof(struct QNode)*hash->capacity);
    for (int i=0; i<hash->capacity; i++){
        hash->array[i] = NULL;
    }
    return hash;
}

int AreAllFramesFull(Queue* queue){
    return queue->count == queue->numberOfFrames; // size of queue determines size of cache
}

int isQueueEmpty(Queue* queue){
    return queue->rear == NULL;
}

void deQueue(Queue* queue){ // we could have return QNode but how would affect memory mgmt?  
    if (isQueueEmpty(queue)){
        return;
    }
    if (queue->front == queue->rear){
        queue->rear = NULL;
    }
    QNode* temp = queue->rear;
    queue->rear = queue->rear->prev;
    if (queue->rear){
        queue->rear->next = NULL;
    }
    free(temp); // we declared temp just to be able to free it.
    queue->count--;
}

void Enqueue(Queue* queue, Hash* hash, int value, unsigned bucket){ // actually also en-hashes
    if (AreAllFramesFull(queue)){
        hash->array[queue->rear->bucket] = NULL;
        deQueue(queue); // goodbye
    }
    QNode* temp = newQNode(bucket);
    temp->data = value; // obviously have to put the value in there
    temp->next = queue->front;
    if (isQueueEmpty(queue)){
        queue->rear = queue->front = temp;
    }
    else{
        queue->front->prev = temp;
        queue->front = temp;
    }
    hash->array[bucket] = temp;
    queue->count++;
}

unsigned int hashfunc(unsigned capacity, int key){
    unsigned int bucket = (key*353) % capacity; // hope this works collision free
    return bucket; // bucket in hashtable is now mapped to key given, or int provided. 
}

int searchCache(struct LRUCache* cache, int key){
    // look in hash for item "key"
    unsigned bucket = hashfunc(cache->capacity, key);
    if (cache->hash->array[bucket] !=NULL){
        return bucket; 
    }
    else {
        return -1;
    }
}

void freeQueue(Queue* queue);
void freeHash(Hash* hash);
void freeQNode(QNode* qnode);



LRUCache* lRUCacheCreate(int capacity) {
    struct LRUCache* cache = malloc(sizeof(LRUCache));
    cache->capacity = capacity;
    cache->queue = createQueue(capacity);
    cache->hash = createHash(capacity);
    return cache;
} // THIS IS WHERE THE ERROR HAPPENS

int lRUCacheGet(struct LRUCache* obj, int key) {
  unsigned location = searchCache(obj, key);
    if (location >=0){ // found
        struct QNode* tempNode = obj->hash->array[location]; // found in hash
        int tempData = tempNode->data; // to be return after queue reordering
        if (obj->queue->front == tempNode){ // if head, we're already good
            return tempData; // successful cache get
        } 
        else if (obj->queue->rear == tempNode){ // rear
            obj->queue->rear = tempNode->prev;
            tempNode->prev->next = NULL;
            tempNode->prev = NULL;
            tempNode->next = obj->queue->front;
            obj->queue->front = tempNode;
        }
        else{ // base case
            // Qnode* t = tempNode->prev; // not needed bc we're not changing tempNode itself
            tempNode->next->prev = tempNode->prev;
            tempNode->prev->next = tempNode->next;
            tempNode->prev = NULL;
            tempNode->next = obj->queue->front;
            obj->queue->front = tempNode;
        }
        return tempData;
    }
    else{ 
        return location; // is -1 already
    }
}

void lRUCachePut(struct LRUCache* obj, int key, int value) {
    unsigned bucket = hashfunc(obj->capacity, key);
    if (obj->queue->count == obj->capacity){ // at limit, must dequeue // TRY WITH JUST ENQUEUE ONCE WORKING
        deQueue(obj->queue); // does the -- operation
    }
    Enqueue(obj->queue, obj->hash, value, bucket); // also adds to hash-- and handles situations and increments/decrmenets
}

void lRUCacheFree(struct LRUCache* obj) {
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

void freeQueue(struct Queue* queue){
    struct QNode* n = queue->front, *next;
    while (n != NULL){
        next = n->next;
        printf("address of n %p", n);
        free(n);
        n = next;
    }
    free(queue);
}
void freeHash(Hash* hash){ // if you free the nodes associated with the hash in the linked list, you also free them in the hash right? Because they're pointed at. They'll point at null or rather out of range things (seg fault territory?)
    free(hash);
}
void freeQNode(QNode* qnode);