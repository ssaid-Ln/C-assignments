#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define NUM_CHILDREN 10

volatile sig_atomic_t signal_count = 0;  // Compte les signaux reçus
int num;  // Nombre aléatoire propre à chaque processus

// Gestionnaire de signal SIGTERM
void handle_sigterm(int sig) {
    signal_count++;
    if (signal_count >= num) {
        printf("[PID %d] Terminé après %d SIGTERM.\n", getpid(), signal_count);
        exit(0);
    }
}

int main() {
    pid_t pids[NUM_CHILDREN];
    srand(time(NULL));  // Initialise la graine aléatoire

    for (int i = 0; i < NUM_CHILDREN; i++) {
        if ((pids[i] = fork()) == 0) {  // Processus enfant
            signal(SIGTERM, handle_sigterm);  // Attache le gestionnaire de signal
            num = (rand() % 10) + 1;  // Tire un nombre entre 1 et 10
            printf("[PID %d] Nombre aléatoire: %d\n", getpid(), num);
            while (1) pause();  // Dort indéfiniment
        }
    }

    // Processus parent : envoie des signaux SIGTERM
    for (int i = 0; i < NUM_CHILDREN; i++) {
        int sent_signals = 0;
        while (kill(pids[i], SIGTERM) == 0) {
            sent_signals++;
            sleep(1);  // Attendre pour éviter d'envoyer trop rapidement
        }
        printf("[PARENT] PID %d a été terminé après %d signaux.\n", pids[i], sent_signals);
    }

    // Attendre la fin de tous les enfants
    for (int i = 0; i < NUM_CHILDREN; i++) {
        wait(NULL);
    }

    return 0;
}
