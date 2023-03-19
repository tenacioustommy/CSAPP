#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include<limits.h>
#include <sys/types.h>
static int M = 0;
static int L = 0;
static int S =0;
int s = 0;
int E = 0;
int b =0;
char* route;
int miss_count = 0;
int hit_count = 0;
int eviction_count = 0;
unsigned long long int lru_counter = 1;

typedef unsigned long long int mem_addr_t;
typedef struct cache_line {
    char valid;
    mem_addr_t tag;
    unsigned long long int lru;
} cache_line_t;

typedef cache_line_t* cache_set_t;
typedef cache_set_t* cache_t;
cache_t cache;
mem_addr_t set_index_mask;

void accessData(mem_addr_t addr) {
    int i;
    unsigned long long int eviction_lru = ULONG_MAX;
    unsigned int eviction_line = 0;
    mem_addr_t set_index = (addr >> b) & set_index_mask;
    mem_addr_t tag = addr >> (s + b);

    cache_set_t cache_set = cache[set_index];

    for (i = 0; i < E; ++i) {
        if (cache_set[i].valid) {
            if (cache_set[i].tag == tag) {
                cache_set[i].lru = lru_counter++;
                hit_count++;
                return;
            }
        }
    }

    miss_count++;

    for (int i = 0; i < E; ++i) {
        if (eviction_lru > cache_set[i].lru) {
            eviction_line = i;
            eviction_lru = cache_set[i].lru;
        }
    }

    if (cache_set[eviction_line].valid) {
        eviction_count++;
    }

    cache_set[eviction_line].valid = 1;
    cache_set[eviction_line].tag = tag;
    cache_set[eviction_line].lru = lru_counter++;
}
void replayTrace(char* trace_fn) {
    FILE* trace_fp = fopen(trace_fn, "r");
    char trace_cmd;
    mem_addr_t address;
    int size;

    while (fscanf(trace_fp, " %c %llx,%d", &trace_cmd, &address, &size) == 3) {
        switch(trace_cmd) {
            case 'L': accessData(address); break;
            case 'S': accessData(address); break;
            case 'M': accessData(address); accessData(address); break;
            default: break;
        }
    }

    fclose(trace_fp);
}
void initCache() {
    int i, j;
    cache = (cache_set_t*)malloc(sizeof(cache_set_t) * S);
    for (i = 0; i < S; i++) {
        cache[i] = (cache_line_t*)malloc(sizeof(cache_line_t) * E);
        for (j = 0; j < E; j++) {
            cache[i][j].valid = 0;
            cache[i][j].tag = 0;
            cache[i][j].lru = 0;
        }
    }

    /* Computes set index mask */
    set_index_mask = (mem_addr_t)(pow(2, s) - 1);
}

/*
 * freeCache - free allocated memory
 */
void freeCache() {
    int i;
    for (i = 0; i < S; i++) {
        free(cache[i]);
    }
    free(cache);
}
int main(int argc, char* argv[])
{
    char c;

    while ((c = getopt(argc,argv,"s:E:b:t")) != -1) {
        switch(c) {
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b =atoi(optarg);
            break;
        case 't':
            replayTrace(optarg);}
    }

    printSummary(0, 0, 0);
    return 0;
}