#ifndef H_HASHMAP
#define H_HASHMAP
#define HASHMAP_NUM_BUCKETS 100
#define HASHMAP_BUCKET_SIZE 20

typedef struct {
    int* key;
    int* value;
} KVPair;

typedef struct {
    KVPair* items[HASHMAP_BUCKET_SIZE];
    int* numItems;
} HashBucket;

typedef struct {
    HashBucket* entries[HASHMAP_NUM_BUCKETS];
} HashMap;

KVPair* newKVPair();
HashBucket* newHashBucket();
HashMap* newHashMap();
void initKVPair(KVPair* kv);
void initHashBucket(HashBucket* b);
void initHashMap(HashMap* m);
void deleteKVPair(KVPair* kv);
void deleteHashBucket(HashBucket* b);
void deleteHashMap(HashMap* m);
int hashKey(int key);

int hashBucket_add(HashBucket* b, int k, int v);
int hashBucket_add_enableDuplicates(HashBucket* b, int k, int v);
int hashBucket_remove(HashBucket* b, int k);
int hashBucket_removeSpecific(HashBucket* b, int k, int v);

int hashMap_add(HashMap* m, int k, int v);
int hashMap_add_enableDuplicates(HashMap* m, int k, int v);
int hashMap_get(HashMap* m,int k);
int hashMap_remove(HashMap* m, int k);
int hashMap_removeSpecific(HashMap* m, int k, int v);

void printMap(HashMap* m);
#endif
