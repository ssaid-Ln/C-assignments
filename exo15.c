// ldd: -lcrypto
#include <openssl/md5.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


#define NUM_CHILDREN 10
#define ZERO_COUNT 6

int zeros(char *s, int n) {
    for (int i = 0; i < n; i++) {
        if (s[i] != '0') return 0;
    }
    return 1;
}

// Fonction de minage brute-force
void bruteforce(int first, int step, int zero) {
    char buffer[32];
    unsigned char digest[MD5_DIGEST_LENGTH];
    char hash_string[MD5_DIGEST_LENGTH * 2 + 1];
    int nonce = first;
    pid_t pid = getpid();
    FILE *file;

    while (1) {
        snprintf(buffer, sizeof(buffer), "%d", nonce);
        MD5((unsigned char*)buffer, strlen(buffer), digest);
        
        for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
            sprintf(&hash_string[i * 2], "%02x", digest[i]);
        }
        
        if (zeros(hash_string, zero)) {
            char filename[32];
            snprintf(filename, sizeof(filename), "found.%d", pid);
            file = fopen(filename, "w");
            if (file) {
                fprintf(file, "%d\n", nonce);
                fclose(file);
            }
            exit(0);
        }
        nonce += step;
    }
}

int main() {
    pid_t pids[NUM_CHILDREN];
    int first = 0;
    
    // Lancer les enfants
    for (int i = 0; i < NUM_CHILDREN; i++) {
        if ((pids[i] = fork()) == 0) {
            bruteforce(first + i, NUM_CHILDREN, ZERO_COUNT);
            exit(0);
        }
    }
    
    // Attendre le premier enfant qui trouve un nonce
    int status;
    pid_t winner = wait(&status);
    
    // Tuer tous les autres enfants
    for (int i = 0; i < NUM_CHILDREN; i++) {
        if (pids[i] != winner) {
            kill(pids[i], SIGKILL);
        }
    }
    
    // Lire la valeur trouvée
    char filename[32];
    snprintf(filename, sizeof(filename), "found.%d", winner);
    FILE *file = fopen(filename, "r");
    if (file) {
        int result;
        fscanf(file, "%d", &result);
        fclose(file);
        printf("Nonce trouvé : %d\n", result);
        remove(filename);
    }
    return 0;
}
