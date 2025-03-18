// This program waits for a Ctrl+C (SIGINT) signal and requires two consecutive Ctrl+C presses within 2 seconds to exit.
// If more than 2 seconds pass between the presses, the program resumes printing "working..." and resets the timer.

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

volatile sig_atomic_t first_signal_received = 0;
time_t first_signal_time;

void handle_sigint() {
    signal(SIGINT, handle_sigint); 

    time_t current_time = time(NULL);

    if (first_signal_received && difftime(current_time, first_signal_time) <= 2) {
        printf("\nAu revoir !\n");
        exit(0);
    }

    first_signal_received = 1;
    first_signal_time = current_time;
    printf("Appuyez encore sur Ctrl + C dans les 2 secondes pour quitter.\n");
}


int main() {
    signal(SIGINT, handle_sigint);
    
    while (1) {
        // si 2 secondes sont passe, reset first_signal_received
        if (first_signal_received && difftime(time(NULL), first_signal_time) > 2) {
            first_signal_received = 0;
            printf("Reprise du travail...\n");
        }
        if (!first_signal_received) {
            printf("working...\n");
        }

        sleep(1);
    }

    return 0;
}