// ldd: -lcrypto
#include <openssl/md5.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <poll.h>

#define NUM_CHILDREN 10
#define ZERO_COUNT 6
#define MAX_NONCES 5  

int zeros(char *s, int n) {
    for (int i = 0; i < n; i++) {
        if (s[i] != '0') return 0;
    }
    return 1;
}
char hash[1 + 2*MD5_DIGEST_LENGTH] = {0};
char* md5hash(char *str) {
    unsigned char md5[MD5_DIGEST_LENGTH] = {0};
    MD5((unsigned char*)str, strlen(str), md5);
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(hash + 2 * i, "%02x", md5[i]);
    }
    return hash;
}

void bruteforce(int first, int step, int zero, int write_fd) {
    char buffer[32];
    char *hash_string;
    int nonce = first;

    while (1) {
        snprintf(buffer, sizeof(buffer), "%d", nonce);
        hash_string = md5hash(buffer);

        if (zeros(hash_string, zero)) {
            write(write_fd, &nonce, sizeof(int));
            exit(0);
        }
        nonce += step;
    }
}

int main() {
    pid_t pids[NUM_CHILDREN];
    int pipes[NUM_CHILDREN][2];  
    struct pollfd fds[NUM_CHILDREN];

    for (int i = 0; i < NUM_CHILDREN; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(1);
        }

        if ((pids[i] = fork()) == 0) {
            close(pipes[i][0]);  
            bruteforce(i, NUM_CHILDREN, ZERO_COUNT, pipes[i][1]);
            exit(0);
        }
        close(pipes[i][1]);

        // Configuration de poll()
        fds[i].fd = pipes[i][0];
        fds[i].events = POLLIN;
    }
    int found_count = 0;
    int nonce;

    while (found_count < MAX_NONCES) {
        int ret = poll(fds, NUM_CHILDREN, -1);
        if (ret < 0) {
            perror("poll");
            break;
        }

        for (int i = 0; i < NUM_CHILDREN; i++) {
            if (fds[i].revents & POLLIN) {
                read(fds[i].fd, &nonce, sizeof(int));
                printf("Nonce trouvé : %d\n", nonce);
                found_count++;

                if (found_count >= MAX_NONCES) {
                    break;
                }
            }
        }
    }

    for (int i = 0; i < NUM_CHILDREN; i++) {
        kill(pids[i], SIGKILL);
    }
    for (int i = 0; i < NUM_CHILDREN; i++) {
        close(pipes[i][0]);
    }

    return 0;
}
