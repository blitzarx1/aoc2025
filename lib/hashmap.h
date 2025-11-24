#ifndef HASHMAP_H
#define HASHMAP_H

#define ARRAY_IMPLEMENTATION
#include "array.h"

#ifdef LOG_NAME
#undef LOG_NAME
#endif
#define LOG_NAME "hashmap"
#include "log.h"

#include <stdint.h>
#include <stddef.h>

#define MAP_DEFAULT_CAP 64
#define MAP_LOAD_THRESH 0.5
#define MAP_CAP_MULT 2

typedef struct {
    char *key;
    void *val;
} HashMapBucketEntry;

typedef struct {
    Array *entries;
} HashMapBucket;

typedef struct {
    void *buckets;
    size_t cap;
    /*
     * Current number of occupied buckets int the map
     *
     * Note: Buckets are scattered among capped memory
     */
    size_t len; 
} HashMap;

HashMap *mapnew(size_t cap);
int     mapset(HashMap *map, char *key, void *val);
void    *mapget(HashMap *map, char *key);
void    mapfree(HashMap *map);

// helper macros for typed access
#define MAP_GET(type, map, key)     ((type *)(mapget((map), (key))))
#define MAP_GET_VAL(type, map, key) (*(type *)(mapget((map), (key))))

#ifdef HASHMAP_IMPLEMENTATION

// hash function (FNV-1a)
static uint32_t hash_str_fnv1a(const char *s) {
    uint32_t hash = 2166136261u;     // FNV offset basis
    while (*s) {
        hash ^= (unsigned char)*s++;
        hash *= 16777619u;           // FNV prime
    }
    return hash;
}

static int mapbuckalloc(HashMap *map, size_t cap) {
    size_t bucket_size = sizeof(HashMapBucket);
    LOG_DEBUG("initializing buckets with zero values for cap: %zu and bucket size: %zu", bucket_size, cap);

    // init with zeros
    void *buckets = calloc(cap, bucket_size);
    if (!buckets) {
        LOG_ERROR("calloc failed");
        return -1;
    }

    map->buckets = buckets;
    map->cap     = cap;
    return 0;
}

HashMap *mapnew(size_t cap) {
    LOG_DEBUG("called with cap: %zu", cap);

    HashMap *map = malloc(sizeof(HashMap));
    *map = (HashMap){ NULL, 0, 0 };

    size_t mapcap = cap > 0 ? cap : MAP_DEFAULT_CAP;
    if (mapbuckalloc(map, mapcap) < 0) {
        return NULL;
    }

    return map;
}

void *mapget(HashMap *map, char *key) {
    LOG_DEBUG("getting value for key: %s", key);

    if (!map || !key) {
        LOG_ERROR("invalid args (map=%p key=%p)",
                  (void*)map, (void*)key);
        return NULL;
    }

    if (!map->buckets) {
        LOG_ERROR("map is not initialized");
        return NULL;
    }

    uint32_t hash = hash_str_fnv1a(key);
    LOG_DEBUG("got hash: %u", hash);

    if (map->cap == 0) {
        LOG_ERROR("map capacity is zero");
        return NULL;
    }

    int idx = hash % map->cap;
    LOG_DEBUG("got bucket index: %d", idx);

    HashMapBucket *buckets = (HashMapBucket *)map->buckets;
    HashMapBucket *b = &buckets[idx];

    if (!b->entries) {
        LOG_DEBUG("bucket at idx %d is empty", idx);
        return NULL;
    }

    LOG_DEBUG("bucket at idx %d has %zu entries", idx, arrlen(b->entries));
    for (size_t i = 0; i < arrlen(b->entries); i++) {
        HashMapBucketEntry *entry = (HashMapBucketEntry *)arrget(b->entries, i);
        if (strcmp(entry->key, key) == 0) {
            LOG_DEBUG("found matching key at entry %zu", i);
            return entry->val;
        }
    }

    LOG_DEBUG("key not found in bucket at idx %d", idx);

    return NULL;
}

static int insert_val(HashMap *map, char *key, void *val) {
    uint32_t hash = hash_str_fnv1a(key);
    LOG_DEBUG("got hash: %u", hash);

    int idx = hash % map->cap;
    LOG_DEBUG("got bucket index: %d", idx);

    HashMapBucket *buckets = (HashMapBucket *)map->buckets;
    HashMapBucket *b = &buckets[idx];

    if (!b->entries) {
        LOG_DEBUG("bucket at idx %d is empty; initializing new bucket", idx);

        HashMapBucketEntry entry = (HashMapBucketEntry){ key, val };
        Array *entries = arrnew(sizeof(HashMapBucketEntry), 0);
        if (arrappend(entries, &entry) < 0) {
            LOG_ERROR("append to bucket entries");
            return -1;
        }

        b->entries = entries;   // <-- just assign, no memcpy
        map->len++;             // new occupied bucket

        LOG_DEBUG("bucket at idx %d is successfully filled", idx);
        return 0;
    }

    size_t entries_len = arrlen(b->entries);
    LOG_DEBUG("bucket at idx %d is occupied; len=%zu", idx, entries_len);

    // update if key exists
    for (size_t i = 0; i < entries_len; i++) {
        HashMapBucketEntry *existing_entry =
            (HashMapBucketEntry *)arrget(b->entries, i);
        if (strcmp(existing_entry->key, key) == 0) {
            LOG_DEBUG("found existing key at entry %zu; updating value", i);
            existing_entry->val = val;
            return 0;
        }
    }

    // append new entry
    LOG_DEBUG("collision; appending new entry to bucket %d", idx);
    HashMapBucketEntry entry = (HashMapBucketEntry){ key, val };
    if (arrappend(b->entries, &entry) < 0) {
        LOG_ERROR("append to bucket entries");
        return -1;
    }

    return 0;
}

static int mapresize(HashMap *map, size_t new_cap) {
    LOG_DEBUG("resizing map from cap=%zu to new_cap=%zu", map->cap, new_cap);

    HashMapBucket *old_buckets = (HashMapBucket *)map->buckets;
    size_t old_cap = map->cap;

    HashMapBucket *new_buckets = calloc(new_cap, sizeof(HashMapBucket));
    if (!new_buckets) {
        LOG_ERROR("calloc failed in mapresize");
        return -1;
    }

    // switch map to new buckets
    map->buckets = new_buckets;
    map->cap = new_cap;
    map->len = 0;

    // reinsert all entries into new buckets
    for (size_t i = 0; i < old_cap; i++) {
        HashMapBucket *old_b = &old_buckets[i];
        if (!old_b->entries) continue;

        size_t entries_len = arrlen(old_b->entries);
        for (size_t j = 0; j < entries_len; j++) {
            HashMapBucketEntry *entry =
                (HashMapBucketEntry *)arrget(old_b->entries, j);

            LOG_DEBUG("rehash inserting key: %s ...", entry->key);

            if (insert_val(map, entry->key, entry->val) < 0) {
                LOG_ERROR("rehash insert failed");
                // basic failure path; you can improve this later
                return -1;
            }

            LOG_DEBUG("rehash insert for key: %s was successful", entry->key);
        }

        // free old entries array for that bucket
        arrfree(old_b->entries);
    }

    free(old_buckets);

    return 0;
}

int mapset(HashMap *map, char *key, void *val) {
    LOG_DEBUG("setting value for key: %s", key);

    float mapload = (float)map->len / (float)map->cap;
    if (mapload >= MAP_LOAD_THRESH) {
        LOG_DEBUG("map load factor %.2f exceeds threshold %.2f; resizing...",
                  mapload, MAP_LOAD_THRESH);

        size_t new_cap = map->cap * MAP_CAP_MULT;
        LOG_DEBUG("new capacity: %zu", new_cap);

        if (mapresize(map, new_cap) < 0) {
            LOG_ERROR("mapresize failed");
            return -1;
        }
    }

    return insert_val(map, key, val);
}

void    mapfree(HashMap *map) {
    if (!map) return;
    LOG_DEBUG("freeing map len=%zu cap=%zu", map->len, map->cap);
    
    for (size_t i = 0; i < map->cap; i++) {
        HashMapBucket *buckets = (HashMapBucket *)map->buckets;
        HashMapBucket *b = &buckets[i];
        if (b->entries) {
            arrfree(b->entries);
            b->entries = NULL;
        }
    }
    free(map->buckets);
    free(map);
}

#endif // HASHMAP_IMPLEMENTATION
#endif // HASHMAP_H
