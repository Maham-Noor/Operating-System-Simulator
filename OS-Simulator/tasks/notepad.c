#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define MAX_LINES 1000
#define MAX_LENGTH 256
#define AUTOSAVE_INTERVAL 5  // seconds

// Colors for text
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

char lines[MAX_LINES][MAX_LENGTH];
int line_count = 0;
int running = 1;
char filename[100];

// Auto-save function
void autosave(int signum) {
    FILE *file = fopen(filename, "w");
    if (file) {
        for (int i = 0; i < line_count; i++) {
            fprintf(file, "%s\n", lines[i]);
        }
        fclose(file);
        printf("\033[2m.\033[0m"); // Dim dot indicating autosave
        fflush(stdout);
    }
    alarm(AUTOSAVE_INTERVAL); // schedule next autosave
}


// Load the file content
void load_file() {
    FILE *file = fopen(filename, "r");
    if (file) {
        while (fgets(lines[line_count], MAX_LENGTH, file) && line_count < MAX_LINES) {
            lines[line_count][strcspn(lines[line_count], "\n")] = '\0'; // strip newline
            line_count++;
        }
        fclose(file);
        printf(GREEN "File %s loaded successfully.\n" RESET, filename);
    } else {
        printf(YELLOW "New file will be created: %s\n" RESET, filename);
    }
}

// Save the content manually
void manual_save() {
    FILE *file = fopen(filename, "w");
    if (file) {
        for (int i = 0; i < line_count; i++) {
            fprintf(file, "%s\n", lines[i]);
        }
        fclose(file);
        printf(GREEN "[Manual Save] Saved to %s\n" RESET, filename);
    } else {
        perror(RED "Manual save failed\n" RESET);
    }
}

// Display the current content of the file
void display_file_content() {
    printf(CYAN "===============================\n" RESET);
    printf(BLUE "Editing file: %s\n" RESET, filename);
    printf(CYAN "===============================\n" RESET);
    if (line_count == 0) {
        printf(YELLOW "The file is empty. Start typing!\n" RESET);
    } else {
        for (int i = 0; i < line_count; i++) {
            printf("%s\n", lines[i]);
        }
    }
}

// Ask the user if they want to open a new file
void ask_to_open_new_file() {
    char choice;
    printf(YELLOW "Do you want to open a new file? (y/n): " RESET);
    choice = getchar();
    while (getchar() != '\n'); // Clear input buffer

    if (choice == 'y' || choice == 'Y') {
        // Reset the current file content
        line_count = 0;
        printf("Enter the new filename: ");
        fgets(filename, sizeof(filename), stdin);
        filename[strcspn(filename, "\n")] = '\0'; // strip newline
        load_file();
    }
}

// Main function
int main() {
    printf(CYAN "=== MyOS Notepad ===\n" RESET);
    printf("Enter filename to open or create: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0'; // strip newline

    load_file();

    signal(SIGALRM, autosave);
    alarm(AUTOSAVE_INTERVAL);  // start autosave

    while (running) {
        // Display current file contents
        display_file_content();

        // Ask user for new content or modification
        printf(YELLOW "\nEnter new content (or ':exit' to exit, ':save' to save): " RESET);
        char input[MAX_LENGTH];
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';  // Remove newline

        if (strcmp(input, ":exit") == 0) {
            running = 0;
        } else if (strcmp(input, ":save") == 0) {
            manual_save();
            // Ask if the user wants to open a new file
            ask_to_open_new_file();
        } else {
            // Add or modify content
            if (line_count < MAX_LINES) {
                // Check if input starts with a specific line number
                if (input[0] == ':') {
                    // Example input format: :edit <line_number>
                    if (strncmp(input, ":edit", 5) == 0) {
                        int line_num = atoi(input + 6);  // Get the line number after ':edit '
                        if (line_num > 0 && line_num <= line_count) {
                            printf("Editing line %d: ", line_num);
                            fgets(lines[line_num - 1], MAX_LENGTH, stdin);
                            lines[line_num - 1][strcspn(lines[line_num - 1], "\n")] = '\0';  // Remove newline
                            printf("Line %d edited.\n", line_num);
                        } else {
                            printf(RED "Invalid line number.\n" RESET);
                        }
                    }
                } else {
                    // If it's not a command, we append the text to the lines
                    strncpy(lines[line_count], input, MAX_LENGTH);
                    line_count++;
                }
            } else {
                printf(RED "Maximum lines reached.\n" RESET);
            }
        }
    }

    manual_save();
    printf(GREEN "\nExiting Notepad. Final save complete.\n" RESET);

    return 0;
}
