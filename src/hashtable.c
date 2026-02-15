#include "kai.h"

static uint64_t fnv1a_hash(char* s, int len) {
    uint64_t hash = 0xcbf29ce484222325;
    for(size_t i=0; i<len; i++) {
        hash *= 0x100000001b3;
        hash ^= (unsigned char)s[i];
    }
    return hash;
}

static HashPair* get_pair(HashTable* ht, char* key) {
    
}

void hashinsert(HashTable* hashtable, char* key, char* val) {
    uint64_t hash = fnv1a_hash(key, strlen(key));

}

void* hashget(HashTable* hashtable, char* key) {
    
}

void hashdelete(HashTable* hashtable, char* key) {

}

