#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define NUM_CHILDREN 10

// Fonction pour gérer SIGUSR1
void handle_sigusr1(int sig) {
    exit(rand() % 2); // Quitte avec code 0 ou 1
}

int main() {
    pid_t pids[NUM_CHILDREN];
    int behaviors[NUM_CHILDREN];
    srand(time(NULL));

    for (int i = 0; i < NUM_CHILDREN; i++) {
        if ((pids[i] = fork()) == 0) {  // Processus enfant
            int behavior = rand() % 4;  // Choisir un comportement (0 à 3)
            behaviors[i] = behavior;

            switch (behavior) {
                case 0:
                    printf("[PID %d] Dort indéfiniment.\n", getpid());
                    while (1) pause();
                    break;
                case 1:
                    printf("[PID %d] Ignore SIGUSR1.\n", getpid());
                    signal(SIGUSR1, SIG_IGN);
                    while (1) pause();
                    break;
                case 2:
                    printf("[PID %d] Termine avec code 0 après SIGUSR1.\n", getpid());
                    signal(SIGUSR1, handle_sigusr1);
                    while (1) pause();
                    break;
                case 3:
                    printf("[PID %d] Termine avec code 1 après SIGUSR1.\n", getpid());
                    signal(SIGUSR1, handle_sigusr1);
                    while (1) pause();
                    break;
            }
        }
    }

    sleep(2); // Attendre un peu avant d'envoyer les signaux
    for (int i = 0; i < NUM_CHILDREN; i++) {
        kill(pids[i], SIGUSR1);
    }

    for (int i = 0; i < NUM_CHILDREN; i++) {
        int status;
        pid_t child_pid = wait(&status);
        if (WIFEXITED(status)) {
            printf("[PARENT] PID %d s'est terminé avec le code %d.\n", child_pid, WEXITSTATUS(status));
        } else {
            printf("[PARENT] PID %d n'a pas terminé normalement.\n", child_pid);
        }
    }

    return 0;
}
