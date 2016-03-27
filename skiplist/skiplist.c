#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main(int argc, char *argv[]) {
    int i, n, seed;

    if (argc < 2 || argc > 3) {
        printf("Usage: %s <n> [seed]\n", argv[0]);
        exit(1);
    }

    n = atoi(argv[1]);

    if (argc == 3) seed = atoi(argv[2]);
    else seed = time(NULL);
    srand(seed);

    for (i = 0; i < n; i++) {
        printf("%3d : %3d\n", i, rand());
    }

    return 0;
}
