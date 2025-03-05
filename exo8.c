#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#define DEFAULT_PLAYER 2
#define DEFAULT_WIDTH 4
#define DEFAULT_HEIGHT 4


bool getvar(int argc, char *argv[], int *height, int *width, int *players) {

    *height = DEFAULT_HEIGHT;
    *width = DEFAULT_WIDTH;
    *players = DEFAULT_PLAYER;

    if (argc > 1) {
        *height = atoi(argv[1]);
    } else {
        char *height_tmp = getenv("P4HEIGHT");
        if (height_tmp) {
            *height = atoi(height_tmp);
        }
    }

    if (argc > 2) {
        *width = atoi(argv[2]);
    } else {
        char *width_tmp = getenv("P4WIDTH");
        if (width_tmp) {
            *width = atoi(width_tmp);
        }
    }

    if (argc > 3) {
        *players = atoi(argv[3]);
    } else {
        char *players_tmp = getenv("P4PLAYERS");
        if (players_tmp) {
            *players = atoi(players_tmp);
        }
    }

    if (*height < 4 || *height > 16) {
        *height = DEFAULT_HEIGHT;
    }
    if (*width < 4 || *width > 26) {
        *width = DEFAULT_WIDTH;
    }
    if (*players < 2 || *players > 8) {
        *players = DEFAULT_PLAYER;
    }

    return true;
}

int main(int argc, char *argv[]) {
    int height, width, players;

    if (!getvar(argc, argv, &height, &width, &players)) {
        fprintf(stderr, "Invalid parameters\n");
        return EXIT_FAILURE;
    }
    printf("Grid Width: %d\n", width);
    printf("Grid Height: %d\n", height);
    printf("Number of Players: %d\n", players);
    printf("%d",argc);

    return EXIT_SUCCESS;
}
