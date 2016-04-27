/*  Author  : Thales Paiva
 *  Contact : <first name + last name at googlemail com>
 *
 *  Compile : gcc -Wall -Werror -ansi -o skp skiplist.c
 *
 *  Usage   : Usage: ./skp <n> <height> [seed]
 *
 *  Example : ./skp 10 7 0
 *  Output  : Values: 3, 7, 3, 6, 9, 2, 0, 3, 0, 2
 *            | 0|-| 0|-| 2|-| 2|-| 3|-| 3|-| 3|-| 6|-| 7|-| 9|-Null
 *            | 0|-| 0|-| 2|-| 2|------| 3|-| 3|-| 6|-| 7|-| 9|-Null
 *            | 0|------| 2|-| 2|------| 3|-| 3|-| 6|-| 7|-| 9|-Null
 *            | 0|-----------| 2|------| 3|-| 3|------| 7|-| 9|-Null
 *            | 0|-----------| 2|------| 3|-----------| 7|------Null
 *            ---------------| 2|---------------------| 7|------Null
 *            --------------------------------------------------Null
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define SKIPLIST_MAX_HEIGHT 16
#define SKIPLIST_MAX_KEY 1000

#define INF INT_MAX
#define NEG_INF INT_MIN

typedef struct node_t {
    int key;
    int height;
    struct node_t **levels;
} Node;

typedef struct skiplist_t {
    double p;
    int height;
    Node *header;
    Node *end;
} SkipList;

void node_init(Node *node, int key, int height);
void skiplist_init(SkipList *skiplist, double p);
void skiplist_insert(SkipList *skiplist, int key);
void skiplist_horizontal_print(SkipList *skiplist);
void skiplist_vertical_print(SkipList *skiplist);
int skiplist_get_random_node_height(SkipList *skiplist);


int main(int argc, char *argv[]) {
    int i, n, seed, key;
    double p;

    SkipList skiplist;

    if (argc < 3 || argc > 4) {
        printf("Usage: %s <n> <p> [seed]\n", argv[0]);
        exit(1);
    }

    n = atoi(argv[1]);
    p = atof(argv[2]);

    if (argc == 4) seed = atoi(argv[3]);
    else seed = time(NULL);
    srand(seed);

    skiplist_init(&skiplist, p);

    printf("[+] dump key: \n");
    printf("    (i    , key)\n");
    for (i = 0; i < n; i++) {
        key = rand() % SKIPLIST_MAX_KEY;
        printf("    (%-4d , %4d)\n", i, key);
        skiplist_insert(&skiplist, key);
    }
    skiplist_vertical_print(&skiplist);

    return 0;
}

void skiplist_init(SkipList *skiplist, double p) {
    skiplist->p = p;

    skiplist->header = malloc(sizeof(*(skiplist->header)));
    skiplist->end = malloc(sizeof(*(skiplist->end)));
    if (!skiplist->header || !skiplist->end) {
        printf("[-] malloc error for skiplist->header.\n");
        exit(1);
    }
    node_init(skiplist->header, NEG_INF, SKIPLIST_MAX_HEIGHT);
    node_init(skiplist->end, INF, 0);

    skiplist->height = 1;
    skiplist->header->levels[0] = skiplist->end;
}

void node_init(Node *node, int key, int height) {
    int i;

    node->key = key;
    node->height = height;

    node->levels = malloc(height*sizeof(*(node->levels)));
    if (!node->levels) {
        printf("[-] malloc error for node->levels\n");
        exit(1);
    }
    for (i = 0; i < height; i++)
        node->levels[i] = NULL;
}

void skiplist_insert(SkipList *skiplist, int key) {
    int i;
    Node *key_node;
    Node *tmp_node;
    Node *pointing_key_node[SKIPLIST_MAX_HEIGHT];

    key_node = malloc(sizeof(*key_node));
    node_init(key_node, key, skiplist_get_random_node_height(skiplist));

    tmp_node = skiplist->header;
    for (i = skiplist->height - 1; i >= 0; i--) {
        while (tmp_node->levels[i] != NULL && tmp_node->levels[i]->key < key)
            tmp_node = tmp_node->levels[i];
        pointing_key_node[i] = tmp_node;
    }

    tmp_node = tmp_node->levels[0];
    if (tmp_node->key == key) /* Já havia um elemento com chave key. */
        return;

    if (key_node->height > skiplist->height) {
        for (i = skiplist->height; i < key_node->height; i++)
            pointing_key_node[i] = skiplist->header;
        skiplist->height = key_node->height;
    }

    for (i = 0; i < key_node->height; i++) {
        key_node->levels[i] = pointing_key_node[i]->levels[i];
        pointing_key_node[i]->levels[i] = key_node;
    }
}

int skiplist_get_random_node_height(SkipList *skiplist) {
    int height;

    height = 1;
    while (rand()/(1.0 +  RAND_MAX) < skiplist->p)
        height++;

    if (height > SKIPLIST_MAX_HEIGHT)
        height = SKIPLIST_MAX_HEIGHT;
    return height;
}

void skiplist_horizontal_print(SkipList *skiplist) {
    Node *tmp_node;
    Node **nodes_addresses;
    int i, j, n;

    for (tmp_node = skiplist->header->levels[0], n = 0; tmp_node != NULL;
         tmp_node = tmp_node->levels[0], n++);

    nodes_addresses = malloc(n*sizeof(*nodes_addresses));

    for (tmp_node = skiplist->header->levels[0], i = 0; tmp_node != NULL;
         tmp_node = tmp_node->levels[0], i++)
        nodes_addresses[i] = tmp_node;

    printf("[+] dump skiplist: \n");
    for (i = 0; i < skiplist->height; i++) {
        tmp_node = skiplist->header->levels[i];
        for (j = 0; j < n; j++) {
            if (tmp_node != NULL && tmp_node == nodes_addresses[j]) {
                printf("|%2d|-", tmp_node->key);
                tmp_node = tmp_node->levels[i];
            }
            else {
                printf("-----");
            }
        }
        printf("Null\n");
    }
}

void skiplist_vertical_print(SkipList *skiplist) {
    Node *tmp_node;
    int i;
    int max_number_of_digits;
    int n;
    char format1[20];
    char format2[20];

    max_number_of_digits = 0;
    for (n = SKIPLIST_MAX_KEY; n > 0; n /= 10)
        max_number_of_digits++;
    sprintf(format1, " %%%dd ", max_number_of_digits);
    sprintf(format2, "%%%ds| ", max_number_of_digits);
    printf("[+] dump skiplist: \n");
    for (tmp_node = skiplist->header->levels[0]; tmp_node != skiplist->end;
            tmp_node = tmp_node->levels[0]) {
        printf("  ");
        for (i = 0; i < skiplist->height; i++) {
            if (i < tmp_node->height)
                printf(format1, tmp_node->key);
            else
                printf(format2, " ");
        }
        printf("\n");
    }
}
