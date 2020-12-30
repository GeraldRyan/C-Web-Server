typedef struct QNode {
    struct QNode *prev, *next;
    unsigned bucket;
    int key;
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

struct QNode* newQNode(unsigned bucket){
    struct QNode* temp = malloc(sizeof(struct QNode));
    temp->bucket = bucket;
    temp->prev = temp->next = NULL;
    return temp;
}

struct Queue* createQueue(int numberOfFrames){
    struct Queue *queue = malloc(sizeof(struct Queue));
    queue->count = 0;
    queue->numberOfFrames = numberOfFrames;
    queue->front = queue->rear = NULL;
    return queue;
}

struct Hash* createHash(int capacity){
    struct Hash* hash = malloc(sizeof(struct Hash));
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
    if (queue->front == queue->rear){ // LL of one
        queue->front = NULL;
    }
    QNode* temp = queue->rear;
    queue->rear = queue->rear->prev;
    if (queue->rear){
        queue->rear->next = NULL;
    }
    queue->count--;
    // free(temp); // This creates an AddressSanitizer error heap-use-after-free.

}

void Enqueue(Queue* queue, Hash* hash, int value, unsigned bucket, int key){ // also adds to hash
    if (AreAllFramesFull(queue)){
        hash->array[queue->rear->bucket] = NULL;
        deQueue(queue); // goodbye
    }
    QNode* temp = newQNode(bucket);
    temp->data = value; // obviously have to put the value in there
    temp->key = key; // needed for handling hash collisions
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
    // look in hash for item of "key"; double check key=key in case of collision
    unsigned bucket = hashfunc(cache->capacity, key);
    if (cache->hash->array[bucket] !=NULL && cache->hash->array[bucket]->key == key){
        return bucket; 
    }
    else {
        return -1;
    }
}

void freeQueue(Queue* queue);
void freeHash(Hash* hash);
void freeQNode(QNode* qnode);



struct LRUCache* lRUCacheCreate(int capacity) {
    struct LRUCache* cache = malloc(sizeof(struct LRUCache));
    cache->capacity = capacity;
    cache->queue = createQueue(capacity);
    cache->hash = createHash(capacity);
    return cache;
} // THIS IS WHERE THE ERROR HAPPEN(ED) -- fixed- (sort of), but not understood

int lRUCacheGet(struct LRUCache* obj, int key) {
  unsigned location = searchCache(obj, key);
    if (location >=0){ // found, else searchCache returns -1
        struct QNode* tempNode = obj->hash->array[location]; // found in hash 
        int tempData = tempNode->data; // to be return after queue reordering
        if (obj->queue->front == tempNode){ // if head
            return tempData; // success
        } 
        else if (obj->queue->rear == tempNode){ // rear
            obj->queue->rear = tempNode->prev;
            tempNode->prev->next = NULL;
            tempNode->prev = NULL;
            tempNode->next = obj->queue->front;
            obj->queue->front = tempNode;
        }
        else{ // node somewhere in middle
            // Qnode* t = tempNode->prev; // not needed bc we're not changing tempNode itself
            tempNode->next->prev = tempNode->prev; // tempnode->next could null
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
    Enqueue(obj->queue, obj->hash, value, bucket, key); // also adds to hash-- and handles situations and increments/decrmenets
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
    struct QNode* curr = queue->front;
    while (curr){
        QNode* tmp = curr;
        curr = curr->next;
        free(tmp);
    }
    free(queue);
}
void freeHash(Hash* hash){ // if you free the nodes associated with the hash in the linked list, you also free them in the hash right? Because they're pointed at. They'll point at null or rather out of range things (seg fault territory?)
    free(hash);
}
void freeQNode(QNode* qnode);