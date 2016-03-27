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
void skiplist_insert_data(SkipList *skiplist, int data);
int skiplist_get_random_n_level(SkipList *skiplist);


int main(int argc, char *argv[]) {
    int i, n, max_levels, seed, data;
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

    printf("[+] dump data: \n");
    printf("    (i    , data)\n");
    for (i = 0; i < n; i++) {
        data = rand() % 10;
        printf("    (%-4d , %4d)\n", i, data);
        skiplist_insert_data(&skiplist, data);

    }
    return 0;
}

void skiplist_init(SkipList *skiplist, int max_levels) {
    int i;

    skiplist->max_levels = max_levels;

    skiplist->levels = malloc(max_levels*sizeof(*(skiplist->levels)));
    if (!skiplist->levels) {
        printf("[-] malloc error for skiplist->levels.\n");
        exit(1);
    }
    for (i = 0; i < max_levels; i++)
        skiplist->levels[i] = NULL;
}

void node_init(Node *node, int data, int n_levels) {
    int i;

    node->data = data;
    node->n_levels = n_levels;

    node->levels = malloc(n_levels*sizeof(*(node->levels)));
    if (!node->levels) {
        printf("[-] malloc error for node->levels\n");
        exit(1);
    }
    for (i = 0; i < n_levels; i++)
        node->levels[i] = NULL;
}

void skiplist_insert_data(SkipList *skiplist, int data) {
    Node *data_node;
    Node *tmp_node;
    int level;

    data_node = malloc(sizeof(*data_node));
    node_init(data_node, data, skiplist_get_random_n_level(skiplist));

    tmp_node = skiplist->levels[data_node->n_levels - 1];
    for (level = data_node->n_levels - 1; level >= 0; level--) {
        if (skiplist->levels[level] == NULL ||
            skiplist->levels[level]->data > data_node->data) {
            data_node->levels[level] = skiplist->levels[level];
            skiplist->levels[level] = data_node;
            tmp_node = skiplist->levels[level - 1];
        }
        else {
            while (tmp_node->levels[level] != NULL &&
                   tmp_node->levels[level]->data < data_node->data) {
                tmp_node = tmp_node->levels[level];
            }
            data_node->levels[level] = tmp_node->levels[level];
            tmp_node->levels[level] = data_node;
        }
    }
}

int skiplist_get_random_n_level(SkipList *skiplist) {
    /* TODO: discard those out of the non-skew range */
    return 1 + rand() % (skiplist->max_levels);
}
