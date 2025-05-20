#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>

// ANSI color codes
#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define CYAN    "\033[1;36m"
#define BOLD    "\033[1m"
#define MAGENTA "\033[1;35m"

void create_file() {
    char filename[100];
    printf(CYAN "Enter file name to create: " RESET);
    scanf("%s", filename);

    int fd = open(filename, O_CREAT | O_WRONLY, 0644);
    if (fd == -1) {
        perror(RED "File creation failed" RESET);
    } else {
        printf(GREEN "File created: %s\n" RESET, filename);
        close(fd);
    }
    getchar(); getchar(); // Wait for Enter
}

void delete_file() {
    char filename[100];
    printf(CYAN "Enter file name to delete: " RESET);
    scanf("%s", filename);

    if (remove(filename) == 0) {
        printf(GREEN "File deleted successfully.\n" RESET);
    } else {
        perror(RED "Failed to delete file" RESET);
    }
    getchar(); getchar(); // Wait for Enter
}

void copy_file() {
    char src[100], dest[100];
    printf(CYAN "Enter source file: " RESET);
    scanf("%s", src);
    printf(CYAN "Enter destination file: " RESET);
    scanf("%s", dest);

    FILE *f1 = fopen(src, "r");
    FILE *f2 = fopen(dest, "w");

    if (!f1 || !f2) {
        perror(RED "Copy failed" RESET);
        return;
    }

    char ch;
    while ((ch = fgetc(f1)) != EOF) {
        fputc(ch, f2);
    }

    fclose(f1);
    fclose(f2);
    printf(GREEN "File copied.\n" RESET);
    getchar(); getchar(); // Wait for Enter
}

void move_file() {
    char src[100], dest[100];
    printf(CYAN "Enter source file: " RESET);
    scanf("%s", src);
    printf(CYAN "Enter destination path: " RESET);
    scanf("%s", dest);

    if (rename(src, dest) == 0) {
        printf(GREEN "File moved successfully.\n" RESET);
    } else {
        perror(RED "Move failed" RESET);
    }
    getchar(); getchar(); // Wait for Enter
}

void file_info() {
    char filename[100];
    printf(CYAN "Enter file name: " RESET);
    scanf("%s", filename);

    struct stat st;
    if (stat(filename, &st) == 0) {
        printf(YELLOW "Size: %ld bytes\n" RESET, st.st_size);
        printf(YELLOW "Permissions: %o\n" RESET, st.st_mode & 0777);

        // Format last modified time
        char timebuf[100];
        struct tm *timeinfo = localtime(&st.st_mtime);
        strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", timeinfo);
        printf(YELLOW "Last modified: %s\n" RESET, timebuf);
    } else {
        perror(RED "File info error" RESET);
    }
    getchar(); getchar(); // Pause for Enter
}


void list_files() {
    struct dirent *de;
    DIR *dr = opendir(".");

    if (dr == NULL) {
        perror(RED "Could not open current directory" RESET);
        return;
    }

    printf(MAGENTA "\nFiles in current directory:\n" RESET);
    while ((de = readdir(dr)) != NULL) {
        if (de->d_type == DT_REG)
            printf(GREEN "- %s\n" RESET, de->d_name);
    }

    closedir(dr);
    getchar(); getchar(); // Wait for Enter
}

int main() {
    int choice;
    do {
        system("clear"); // Clears the terminal screen

        printf(BOLD "\n========== FILE MANAGER ==========\n" RESET);
        printf( "1. Create File\n" );
        printf( "2. Delete File\n" );
        printf( "3. Copy File\n" );
        printf( "4. Move File\n" );
        printf( "5. File Info\n" );
        printf( "6. List Files\n" );
        printf(RED "0. Exit\n" RESET);
        printf(CYAN "Enter choice: " RESET);
        scanf("%d", &choice);

        switch (choice) {
            case 1: create_file(); break;
            case 2: delete_file(); break;
            case 3: copy_file(); break;
            case 4: move_file(); break;
            case 5: file_info(); break;
            case 6: list_files(); break;
            case 0: printf(RED "Exiting File Manager...\n" RESET); break;
            default: printf(RED "Invalid choice.\n" RESET); getchar(); getchar();
        }
    } while (choice != 0);

    return 0;
}
