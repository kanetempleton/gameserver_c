#include "HashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

KVPair* newKVPair() {
    return (KVPair*)malloc(sizeof(KVPair));
}
HashBucket* newHashBucket() {
    return (HashBucket*)malloc(sizeof(HashBucket));
}
HashMap* newHashMap() {
    return (HashMap*)malloc(sizeof(HashMap));
}

void initKVPair(KVPair* kv) {
    kv->key = malloc(sizeof(int));
    kv->value = malloc(sizeof(int));
    *(kv->key)=-1;
    *(kv->value)=-1;
}
void initHashBucket(HashBucket* b) {
    for (int i=0; i<HASHMAP_BUCKET_SIZE; i++) {
        b->items[i]=newKVPair();
        initKVPair(b->items[i]);
    }
    b->numItems = malloc(sizeof(int));
    *(b->numItems) = 0;
}
void initHashMap(HashMap* m) {
    for (int i=0; i<HASHMAP_NUM_BUCKETS; i++) {
        m->entries[i]=newHashBucket();
        initHashBucket(m->entries[i]);
    }
}

void deleteHashMap(HashMap* m) {
}

int hashKey(int key) {
    return key%HASHMAP_NUM_BUCKETS;
}

int hashBucket_add(HashBucket* b, int k, int v) {
    for (int i=0; i<HASHMAP_BUCKET_SIZE; i++) {
        KVPair* kv = b->items[i];
        if (*(kv->key)==k) {
            *(b->items[i]->value) = v; //replace existing value with new one
            return i;
        }
        else if (*(kv->key)<0) { //add new kv pair
            *(b->items[i]->key) = k;
            *(b->items[i]->value) = v;
            *(b->numItems)=*(b->numItems)+1;
            //printf("added (%d,%d) and now has %d items\n",*(b->items[i]->key),*(b->items[i]->value),*(b->numItems));
            return i;
        }
    }
    return -1; //failure, bucket is full
}

int hashBucket_add_enableDuplicates(HashBucket* b, int k, int v) {
    for (int i=0; i<HASHMAP_BUCKET_SIZE; i++) {
        KVPair* kv = b->items[i];
        if (*(kv->key)==k && *(kv->value)==v) {
            //printf("duplicate  value detected\n");
            continue; //dont allow duplicate values
        }
        else if (*(kv->key)<0 && !(*(kv->key)==k && *(kv->value)==v)) { //add new kv pair
            *(b->items[i]->key) = k;
            *(b->items[i]->value) = v;
            *(b->numItems)=*(b->numItems)+1;
            //printf("added (%d,%d) and now has %d items\n",*(b->items[i]->key),*(b->items[i]->value),*(b->numItems));
            return i;
        }
    }
    return -1; //failure, bucket is full
}

int hashBucket_remove(HashBucket* b, int k) {
    int j=-1;
    int rv=-1;
    for (int i=0; i<*(b->numItems); i++) {
        if (*(b->items[i]->key)==k) {
            j=i;
            rv=*(b->items[i]->value);
            break;
        }
    }
    if (j==-1)
        return -1; //not found
    for (int i=j; i<*(b->numItems)-1; i++) {
        *(b->items[i]->key) = *(b->items[i+1]->key);
        *(b->items[i]->value) = *(b->items[i+1]->value);
    }
    *(b->items[*(b->numItems)-1]->key)=-1;
    *(b->items[*(b->numItems)-1]->value)=-1;
    *(b->numItems)=*(b->numItems)-1;
    return rv; //successful remove
}

int hashBucket_removeSpecific(HashBucket* b, int k, int v) {
    int j=-1;
    int rv=-1;
    for (int i=0; i<*(b->numItems); i++) {
        if (*(b->items[i]->key)==k && *(b->items[i]->value)==v) {
            j=i;
            rv=*(b->items[i]->value);
            break;
        }
    }
    if (j==-1)
        return -1; //not found
    for (int i=j; i<*(b->numItems)-1; i++) {
        *(b->items[i]->key) = *(b->items[i+1]->key);
        *(b->items[i]->value) = *(b->items[i+1]->value);
    }
    *(b->items[*(b->numItems)-1]->key)=-1;
    *(b->items[*(b->numItems)-1]->value)=-1;
    *(b->numItems)=*(b->numItems)-1;
    return rv; //successful remove
}

int hashMap_add(HashMap* m, int k, int v) {
    int h = hashKey(k);

    HashBucket* b = m->entries[h];
    int i=0;
    int h1 = h;
    int addcode = hashBucket_add(m->entries[(h+i)%HASHMAP_NUM_BUCKETS],k,v);
    while (addcode==-1) {
        i++;
        if ((h+i)%HASHMAP_NUM_BUCKETS==h)
            return -1; //hash map is full
        addcode = hashBucket_add(m->entries[(h+i)%HASHMAP_NUM_BUCKETS],k,v);
    }
    return addcode;
    /*while (*(m->entries[(h+i)%HASHMAP_NUM_BUCKETS]->numItems)==HASHMAP_BUCKET_SIZE) {
        i++;
        if ((h+i)%HASHMAP_NUM_BUCKETS==h1)
            return -1; //hash map is full
    }*/
    //return hashBucket_add(m->entries[(h+i)%HASHMAP_NUM_BUCKETS],k,v);
}

int hashMap_add_enableDuplicates(HashMap* m, int k, int v) {
    //printf("adding from  pid %d\n",getpid());
    int h = hashKey(k);

    HashBucket* b = m->entries[h];
    int i=0;
    int h1 = h;
    int addcode = hashBucket_add_enableDuplicates(m->entries[(h+i)%HASHMAP_NUM_BUCKETS],k,v);
    while (addcode==-1) {
        i++;
        if ((h+i)%HASHMAP_NUM_BUCKETS==h)
            return -1; //hash map is full
        addcode = hashBucket_add_enableDuplicates(m->entries[(h+i)%HASHMAP_NUM_BUCKETS],k,v);
    }
    return addcode;
}

int hashMap_get(HashMap* m,int k) {
    int h = hashKey(k);
    int a=0;
    while (a==0 || (a!=0 && ((h+a)%HASHMAP_NUM_BUCKETS!=h)) ) {
        for (int i=0; i<*(m->entries[(h+a)%HASHMAP_NUM_BUCKETS]->numItems); i++) {
            if (*(m->entries[h+a]->items[i]->key)==k) {
                return *(m->entries[(h+a)%HASHMAP_NUM_BUCKETS]->items[i]->value);
            }
        }
        a++;
    }
    return -1; //not found
}

int hashMap_remove(HashMap* m, int k) {
    int h = hashKey(k);
    int a=0;
    while (a==0 || (a!=0 && ((h+a)%HASHMAP_NUM_BUCKETS!=h)) ) {
        int c = hashBucket_remove(m->entries[(h+a)%HASHMAP_NUM_BUCKETS],k);
        if (c>=0) {
            //printf("removed entry for %d; bucket now has %d items\n",k,*(m->entries[(h+a)%HASHMAP_NUM_BUCKETS]->numItems));
            return c; //successful removal
        }
        a++;
    }
    //printf("tried to remove entry for %d, but it was not found\n",k);
    return -1;//not found
}

int hashMap_removeSpecific(HashMap* m, int k, int v) {
    int h = hashKey(k);
    int a=0;
    //printf("attempting specific remove of %d\n",*(m->entries[(h+a)%HASHMAP_NUM_BUCKETS]->numItems));
    while (a==0 || (a!=0 && ((h+a)%HASHMAP_NUM_BUCKETS!=h)) ) {
        int c = hashBucket_removeSpecific(m->entries[(h+a)%HASHMAP_NUM_BUCKETS],k,v);
        if (c>=0) {
            //printf("removed entry for %d; bucket now has %d items\n",k,*(m->entries[(h+a)%HASHMAP_NUM_BUCKETS]->numItems));
            return c; //successful remove
        }
        a++;
    }
    //printf("tried to remove entry for %d, but it was not found\n",k);
    return -1;//not found
}

void printMap(HashMap* m) {
    for (int i=0; i<HASHMAP_NUM_BUCKETS; i++) {
        if (*(m->entries[i]->numItems)>0)
            printf("map bucket %d: ",i);

        for (int j=0; j<HASHMAP_BUCKET_SIZE; j++) {
            if (*(m->entries[i]->items[j]->key) >= 0)
                printf("(%d,%d) ",*(m->entries[i]->items[j]->key),*(m->entries[i]->items[j]->value));
        }

        if (*(m->entries[i]->numItems)>0)
            printf("\n");
    }
}
