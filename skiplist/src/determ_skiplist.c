#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define SKIPLIST_MAX_HEIGHT 16
#define SKIPLIST_MAX_KEY 100

#define INF INT_MAX
#define NEG_INF INT_MIN

typedef struct cell_t {
    int key;
    struct cell_t *bottom;
    struct cell_t *right;
} Cell;

typedef struct skiplist_t {
    Cell *header;
    Cell *end;
    int height;
} SkipList;

Cell *skiplist_search(SkipList *skiplist, int key);
void skiplist_insert(SkipList *skiplist, int key);
void skiplist_init(SkipList *skiplist);
void cell_init(Cell *cell, int key);
void sanitize_insertion(SkipList *skiplist, Cell *first_on_level[],
                        Cell *pointing_key_node[]);
void skiplist_vertical_print(SkipList *skiplist);

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
    node = skiplist_search(&skiplist, 15);
    if (node)
        printf("[+] key %d found.\n", node->key);
    else
        printf("[-] Key 15 not found.\n");
    skiplist_vertical_print(&skiplist);
    return 0;
}


Cell *skiplist_search(SkipList *skiplist, int key) {
    Cell *cell;

    cell = skiplist->header;
    while (cell->bottom != NULL) {
        cell = cell->bottom;
        while (cell->right->key < key)
            cell = cell->right;
    }
    if (cell->right->key == key)
        return cell;

    return NULL;
}


void skiplist_vertical_print(SkipList *skiplist) {
    Cell *cell, *base_cell;
    int base_list[5000];
    int i, n;

    i = 0;

    for (cell = skiplist->header; cell->bottom != NULL; cell = cell->bottom);

    for (i = 0; cell->right != NULL; cell = cell->right, i++)
        base_list[i] = cell->key;
    n = i;

    printf("-----------------------------\n");
    base_cell = skiplist->header->bottom;
    while (base_cell != NULL) {
        cell = base_cell;
        i = 1;
        while (cell->right->right != NULL) {
            cell = cell->right;
            for (; i < n && base_list[i] != cell->key; i++)
                printf("---");
            i++;
            printf("%2d-", cell->key);
        }
        printf("\n");
        base_cell = base_cell->bottom;

        printf("\n");
    }
    printf("-----------------------------\n");
    fflush(stdout);

}

void skiplist_insert(SkipList *skiplist, int key) {
    Cell *cell, *new_cell;
    Cell *first_on_level[SKIPLIST_MAX_HEIGHT];
    Cell *pointing_key_node[SKIPLIST_MAX_HEIGHT];
    int i;

    cell = skiplist->header;
    first_on_level[skiplist->height] = cell;
    pointing_key_node[skiplist->height] = cell;
    i = skiplist->height - 1;
    while (cell->bottom != NULL) {
        cell = cell->bottom;
        first_on_level[i] = cell;
        while (cell->right->key < key) {
            cell = cell->right;
        }
        pointing_key_node[i] = cell;
        i--;
    }

    printf("KEY %d for INS %d\n", cell->key, key);
    if (cell->right->key == key)
        return;

    new_cell = malloc(sizeof(*new_cell));
    new_cell->key = key;
    new_cell->bottom = NULL;
    new_cell->right = cell->right;
    cell->right = new_cell;

    sanitize_insertion(skiplist, first_on_level, pointing_key_node);
}

void sanitize_insertion(SkipList *skiplist, Cell *first_on_level[],
                        Cell *pointing_key_node[]) {
    int i;
    Cell *cell, *new_cell, *up_right;
    int insertion_height;

    insertion_height = 1;
    for (i = 0; i < skiplist->height; i++) {
        cell = first_on_level[i]->right;
        printf("cell[%d] = %d \n", i, cell->key);
        if (cell->right != NULL && cell->right->right != NULL) {
            if (cell->right->right->right == NULL) {
                printf("NUULL\n\n");
                return;
            }
            for (up_right = first_on_level[i+1];
                 up_right->key < cell->key; up_right = up_right->right);
            up_right = pointing_key_node[i+1]->right;
            printf("up_right %x\n", up_right);
            if (up_right == NULL || cell->right->right->key < up_right->key) {
                new_cell = malloc(sizeof(*new_cell));
                new_cell->key = cell->right->key;
                new_cell->bottom = cell->right;
                new_cell->right = pointing_key_node[i+1]->right;
                pointing_key_node[i+1]->right = new_cell;
                insertion_height += 1;
            }
        }
    }
    skiplist_vertical_print(skiplist);

    if (insertion_height > skiplist->height) {
        skiplist->height = insertion_height;
        new_cell = malloc(sizeof(*new_cell));
        new_cell->key = skiplist->header->key;
        new_cell->right = skiplist->end;
        new_cell->bottom = skiplist->header;
        skiplist->header = new_cell;
        printf("Aumentou!\n");
    }

    {printf("FINAL_SANIT\n"); fflush(stdout);}

}

void skiplist_init(SkipList *skiplist) {
    skiplist->header = malloc(sizeof(*(skiplist->header)));
    skiplist->end = malloc(sizeof(*(skiplist->end)));
    if (!skiplist->header || !skiplist->end) {
        printf("[-] malloc error for skiplist->(header|end).\n");
        exit(1);
    }
    cell_init(skiplist->header, NEG_INF);
    cell_init(skiplist->end, INF);

    skiplist->header->right = skiplist->end;
    skiplist->height = 0;
}

void cell_init(Cell *cell, int key) {
    cell->key = key;
    cell->bottom = NULL;
    cell->right = NULL;
}
