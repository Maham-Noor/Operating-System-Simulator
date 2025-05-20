#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TASKS 100
#define FILENAME "ctasks.txt"

typedef struct {
    char date[11];     // Format: YYYY-MM-DD
    char description[100];
} Task;

Task taskList[MAX_TASKS];
int taskCount = 0;

void load_tasks() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) return;

    while (fscanf(file, "%10s %[^\n]s", taskList[taskCount].date, taskList[taskCount].description) != EOF) {
        taskCount++;
    }

    fclose(file);
}

void save_tasks() {
    FILE *file = fopen(FILENAME, "w");
    if (!file) {
        printf("❌ Unable to save tasks.\n");
        return;
    }

    for (int i = 0; i < taskCount; i++) {
        fprintf(file, "%s %s\n", taskList[i].date, taskList[i].description);
    }

    fclose(file);
}

void add_task() {
    if (taskCount >= MAX_TASKS) {
        printf("⚠️ Task limit reached!\n");
        return;
    }

    printf("📅 Enter date (YYYY-MM-DD): ");
    scanf("%s", taskList[taskCount].date);
    getchar(); // flush newline
    printf("📝 Enter task description: ");
    fgets(taskList[taskCount].description, sizeof(taskList[taskCount].description), stdin);
    taskList[taskCount].description[strcspn(taskList[taskCount].description, "\n")] = '\0'; // remove newline
    taskCount++;

    save_tasks();
    printf("✅ Task added!\n");
}

void view_tasks_by_date() {
    char query[11];
    printf("📅 Enter date to view tasks (YYYY-MM-DD): ");
    scanf("%s", query);
    int found = 0;

    printf("\n📌 Tasks on %s:\n", query);
    for (int i = 0; i < taskCount; i++) {
        if (strcmp(taskList[i].date, query) == 0) {
            printf("👉 %s\n", taskList[i].description);
            found = 1;
        }
    }

    if (!found) {
        printf("❌ No tasks found on this date.\n");
    }
}

void view_all_tasks() {
    printf("\n📅 All Tasks:\n");
    for (int i = 0; i < taskCount; i++) {
        printf("📆 %s: %s\n", taskList[i].date, taskList[i].description);
    }
}

void show_menu() {
    printf("\n======================\n");
    printf("📆 Task Calendar Menu\n");
    printf("======================\n");
    printf("1. Add Task\n");
    printf("2. View Tasks by Date\n");
    printf("3. View All Tasks\n");
    printf("0. Exit\n");
    printf("Choose an option: ");
}

int main() {
    int choice;
    load_tasks();

    do {
        show_menu();
        scanf("%d", &choice);
        switch (choice) {
            case 1: add_task(); break;
            case 2: view_tasks_by_date(); break;
            case 3: view_all_tasks(); break;
            case 0: printf("👋 Exiting Task Calendar.\n"); break;
            default: printf("❗ Invalid choice!\n");
        }
    } while (choice != 0);

    return 0;
}
