#include <stdio.h>
#include <stdlib.h>
#include <time.h>


typedef struct node_t {
    int data;
    int n_levels;
    struct node_t **levels;
} Node;

typedef struct skiplist_t {
    int max_levels;
    Node **levels;
} SkipList;


void skiplist_init(SkipList *skiplist, int max_levels);


int main(int argc, char *argv[]) {
    int i, n, max_levels, seed;
    SkipList skiplist;

    if (argc < 3 || argc > 4) {
        printf("Usage: %s <n> <max_levels> [seed]\n", argv[0]);
        exit(1);
    }

    n = atoi(argv[1]);
    max_levels = atoi(argv[2]);

    if (argc == 4) seed = atoi(argv[3]);
    else seed = time(NULL);
    srand(seed);

    skiplist_init(&skiplist, max_levels);

    for (i = 0; i < n; i++) {
        printf("%3d : %3d\n", i, rand());
    }

    return 0;
}


void skiplist_init(SkipList *skiplist, int max_levels) {
    int i;

    skiplist->max_levels = max_levels;

    skiplist->levels = malloc(max_levels*sizeof(skiplist->levels));
    if (!skiplist->levels) {
        printf("[-] malloc error for skiplist->levels.\n");
        exit(1);
    }
    for (i = 0; i < max_levels; i++)
        skiplist->levels[i] = NULL;
}
