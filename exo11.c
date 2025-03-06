#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

// char get_file_type(struct stat *file_status) {
//     if (S_ISREG(file_status->st_mode)) return '-';   
//     if (S_ISDIR(file_status->st_mode)) return 'd';    
//     if (S_ISLNK(file_status->st_mode)) return 'l';    
//     if (S_ISSOCK(file_status->st_mode)) return 's';   
//     if (S_ISFIFO(file_status->st_mode)) return 'p';   
//     if (S_ISBLK(file_status->st_mode)) return 'b';   
//     if (S_ISCHR(file_status->st_mode)) return 'c'; 
//     return '?';  
// }
void print_file_attributes(struct stat *file_status, char *file_name) {
    printf(" %lu %u %u %lld %s\n", file_status->st_nlink, file_status->st_uid, file_status->st_gid,
           (long long int)file_status->st_size, file_name);
}

int main() {
    DIR *dir = opendir(".");
    struct dirent *input;

    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    while ((input = readdir(dir)) != NULL) {
        if (strcmp(input->d_name, ".") == 0 || strcmp(input->d_name, "..") == 0) {
            continue;
        }

        struct stat file_status;
        if (stat(input->d_name, &file_status) == -1) {
            perror("Error reading file information");
            closedir(dir);
            exit(EXIT_FAILURE);
        }

        print_file_attributes(&file_status, input->d_name);
    }

    closedir(dir);

    return 0;
}