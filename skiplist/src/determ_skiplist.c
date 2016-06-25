#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define SKIPLIST_MAX_HEIGHT 20
#define SKIPLIST_MAX_KEY 100
#define SKIPLIST_MAX_ENTRIES 2000

#define INF INT_MAX
#define NEG_INF INT_MIN

typedef struct cell_t {
    int key;
    struct cell_t *down;
    struct cell_t *right;
} Cell;

typedef struct skiplist_t {
    Cell *header;
    Cell *end;
    Cell *bottom;
} SkipList;

void cell_init(Cell *cell, int key, Cell *down, Cell *right);
Cell *skiplist_search(SkipList *skiplist, int key);
void skiplist_insert(SkipList *skiplist, int key);
void skiplist_remove(SkipList *skiplist, int key);
void skiplist_init(SkipList *skiplist);
void skiplist_vertical_print(SkipList *skiplist);
void skiplist_grow(SkipList *skiplist);


int main(int argc, char *argv[]) {
    int i, n, seed, key;

    SkipList skiplist;
    Cell *node;

    if (argc < 2 || argc > 3) {
        printf("Usage: %s <n> [seed]\n", argv[0]);
        exit(1);
    }

    n = atoi(argv[1]);

    if (argc == 3) seed = atoi(argv[2]);
    else seed = time(NULL);
    srand(seed);

    skiplist_init(&skiplist);

    printf("[+] dump key: \n");
    printf("    (i    , key)\n");
    for (i = 0; i < n; i++) {
        key = rand() % SKIPLIST_MAX_KEY;
        printf("    (%-4d , %4d)\n", i, key);
        skiplist_insert(&skiplist, key);
        skiplist_vertical_print(&skiplist);

    }
    skiplist_vertical_print(&skiplist);
    /*node = skiplist_search(&skiplist, 15);*/
    if (NULL)
        printf("[+] key %d found.\n", node->key);
    else
        printf("[-] Key 15 not found.\n");
    return 0;
}


Cell *skiplist_search(SkipList *skiplist, int key) {
    Cell *cell;

    cell = skiplist->header;
    while (cell->down != NULL) {
        cell = cell->down;
        while (cell->right->key < key)
            cell = cell->right;
    }
    if (cell->right->key == key)
        return cell;

    return NULL;
}


void skiplist_insert(SkipList *skiplist, int key) {
    int down_last_key;
    Cell *cell, *new;

    skiplist->bottom->key = key;

    cell = skiplist->header;
    while (cell != skiplist->bottom) {

        while (key > cell->key)
            cell = cell->right;

        if ((cell->down == skiplist->bottom) && (cell->key == key))
            return;

        if (cell->down == skiplist->bottom || cell->key == cell->down->right->right->right->key) {
            new = malloc(sizeof(*new));
            cell_init(new, cell->key, cell->down->right->right, cell->right);
            cell->right = new;
            cell->key = cell->down->right->key;
        }
        cell = cell->down;

    }

    if (skiplist->header->right != skiplist->end) {
        printf("skl: %d\n", skiplist->header->key);
        printf("     %d\n", skiplist->header->right->key);
        printf("     %d\n", skiplist->end->key);
        skiplist_grow(skiplist);
    }

}


void skiplist_grow(SkipList *skiplist) {
    Cell *new_cell;

   new_cell = malloc(sizeof(*new_cell));
   cell_init(new_cell, INF - 1, skiplist->header, skiplist->end);
   skiplist->header = new_cell;
   printf("CRESCI!\n");
}


void skiplist_init(SkipList *skiplist) {
    skiplist->header = malloc(sizeof(*(skiplist->header)));
    skiplist->end = malloc(sizeof(*(skiplist->end)));
    skiplist->bottom = malloc(sizeof(*(skiplist->bottom)));

    if (!skiplist->header || !skiplist->end || !skiplist->bottom) {
        printf("[-] malloc error for skiplist->(header|end|bottom).\n");
        exit(1);
    }

    cell_init(skiplist->bottom, INF, skiplist->bottom, skiplist->bottom);
    cell_init(skiplist->end, INF, NULL, skiplist->end);
    cell_init(skiplist->header, INF - 1, skiplist->bottom, skiplist->end);

}


void cell_init(Cell *cell, int key, Cell *down, Cell *right) {
    cell->key = key;
    cell->down = down;
    cell->right = right;
}


void skiplist_vertical_print(SkipList *skiplist) {
    Cell *cell, *base_cell;
    int base_list[SKIPLIST_MAX_ENTRIES];
    int i, n;

    i = 0;

    for (cell = skiplist->header; cell->down != skiplist->bottom; cell = cell->down);

    for (i = 0; cell->right != skiplist->end; cell = cell->right, i++) {
        base_list[i] = cell->key;
        printf("%d_", cell->key);
    }
    n = i;
    printf("\n");
    printf("[+] Dumping SkipList \n");

    base_cell = skiplist->header->down;
    while (base_cell != skiplist->bottom) {
        cell = base_cell;
        i = 0;
        while (cell->right != skiplist->end) {
            for (; base_list[i] != cell->key && i < n; i++)
                printf("---");
            i++;
            printf("%2d-", cell->key);
            cell = cell->right;
        }
        for (; i < n; i++) {
            printf("---");

        }
        printf("\n");
        base_cell = base_cell->down;
        printf("\n");
    }
    fflush(stdout);

}
