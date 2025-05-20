#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include "resource_manager.h"
#include "scheduler.h"
#include "system_log.h"

#define MAX_TASKS 100
#define RESET   "\x1b[0m"
#define BOLD    "\x1b[1m"
#define CYAN    "\x1b[36m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define MAGENTA "\x1b[35m"

typedef enum { USER_MODE, KERNEL_MODE } Mode;
Mode current_mode = USER_MODE;

Task tasks[MAX_TASKS];
int task_count = 0;
char log_message[256];

void* scheduler_thread_func(void* arg) {
    run_scheduler();
    return NULL;
}

// Launch a new task in a separate terminal
void launch_task(const char* path, int ram, int hdd, int cores) {
    if (!allocate_resources(ram, hdd, cores)) {
        printf("Not enough resources to launch this task.\n");
        return;
    }

    // Check if the task is already running
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].state == RUNNING && strcmp(path, tasks[i].name) == 0) {
            printf("This task is already running!\n");
            return; // Don't launch again if already running
        }
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Execute the task in a new xterm
        execlp("xterm", "xterm", "-e", path, NULL);
        perror("Exec failed");
        exit(1);
    } else if (pid > 0) {
        printf("Task %d: launched in new terminal (PID: %d)\n", task_count, pid);
        sprintf(log_message, "🚀 Task %d: launched in new terminal (PID: %d)\n", task_count, pid);
        log_event(log_message);

        Task new_task;
        new_task.pid = pid;
        strncpy(new_task.name, path, sizeof(new_task.name) - 1);
        new_task.name[sizeof(new_task.name) - 1] = '\0';
        new_task.ram = ram;
        new_task.hdd = hdd;
        new_task.cores = cores;
        new_task.priority = 5; // default priority
        new_task.state = READY;

        tasks[task_count++] = new_task;
        sprintf(log_message, "💾 Allocated %dMB RAM, %dMB HDD, %d core(s) for task PID: %d.\n\n", ram, hdd, cores, pid);
        log_event(log_message);

        add_task(new_task);

    } else {
        perror("Fork failed");
        free_resources(ram, hdd, cores);
    }
    //print_all_queues(); // Show tasks in queues
    //print_resource_status(); // Show remaining RAM, HDD, cores
}

// Check for any finished tasks and free their resources
void check_task_completion() {
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].state == RUNNING) {
            int status;
            pid_t result = waitpid(tasks[i].pid, &status, WNOHANG);
            if (result == tasks[i].pid) {
                // Task completed or terminal closed
                printf("Task with PID %d completed.\n", tasks[i].pid);
                printf("Changing task state to TERMINATED.\n");
                free_resources(tasks[i].ram, tasks[i].hdd, tasks[i].cores);
                tasks[i].state = TERMINATED;
                remove_task(tasks[i].pid);
                
                print_resource_status();

                // Dequeue task from the scheduler
                update_task_state(tasks[i].pid, TERMINATED);

                // Optional: Remove task from the task list if needed
                for (int j = i; j < task_count - 1; j++) {
                    tasks[j] = tasks[j + 1];
                }
                task_count--; // Adjust task count after removal
                i--; // Adjust index since we shifted tasks
                print_resource_status();
                sprintf(log_message, "✅ Task %d with PID %d completed.\n",task_count, tasks[i].pid);
                log_event(log_message);
            }
        }
    }
}

void graceful_shutdown() {
    printf("\n🔻 Initiating graceful shutdown...\n");
    log_event("🔻 Initiating graceful shutdown...\n");

   TaskNode* current = get_rr_queue_front();
while (current != NULL) {
    pid_t pid = current->task.pid;

    if (current->task.state != TERMINATED) {
        kill(pid, SIGTERM);
        sprintf(log_message, "☠️ Task with PID %d Killed->Terminated/.\n", pid);
        log_event(log_message);
        update_task_state(pid, TERMINATED);
    }

    current = current->next;
}
    // Optionally, wait for all tasks to finish
    sleep(1);

    /*for (int i = 0; i < task_count; i++) {
        if (tasks[i].state == RUNNING || tasks[i].state == READY) {
            printf("Terminating task PID %d (%s)...\n", tasks[i].pid, tasks[i].name);
            kill(tasks[i].pid, SIGKILL);  // or use SIGTERM for softer shutdown
            sprintf(log_message, "Task with PID %d Killed->Terminated/.\n", tasks[i].pid);
            log_event(log_message);
            waitpid(tasks[i].pid, NULL, 0);  // Wait for it to terminate
            free_resources(tasks[i].ram, tasks[i].hdd, tasks[i].cores);
            remove_task(tasks[i].pid);
            update_task_state(tasks[i].pid, TERMINATED);
        }
    }*/

    task_count = 0;
    printf("✅ All tasks terminated.\n");
    print_resource_status();
}



// Boot sequence
void boot_os() {
    printf("\n\x1b[1;34m***************************************\x1b[0m\n");
    printf("\x1b[1;32m🚀 Booting RAHAM's OS...\x1b[0m\n");
    sleep(1);
    printf("\x1b[1;33m🧠 Initializing Kernel...\x1b[0m\n");
    sleep(1);
    printf("\x1b[1;36m🔌 Loading Drivers...\x1b[0m\n");
    sleep(1);
    printf("\x1b[1;32m🖥️ Welcome to RAHAM's OS v11.7!\x1b[0m\n");
    printf("\x1b[1;34m***************************************\x1b[0m\n\n");
}

// Menu

void show_menu() {
    printf("\n" CYAN "==========================================" RESET);
    printf("\n" BOLD GREEN "         📂 Available Tasks Menu" RESET);
    printf("\n" CYAN "==========================================" RESET);
    printf("\n" MAGENTA "🔐 Current Mode: %s", current_mode == KERNEL_MODE ? "KERNEL" : "USER");
    printf("\n" YELLOW " 1. Calculator");
    printf("\n 2. Notepad");
    printf("\n 3. Clock");
    printf("\n 4. File Manager");
    printf("\n 5. Music Simulator");
    printf("\n 6. MiniGame");
    printf("\n 7. Calender");
    printf("\n 8. Toggle Kernel Mode");
    printf("\n 0. Shutdown" RESET);
    printf("\n" CYAN "==========================================" RESET);
    printf("\n" BOLD "Enter your choice: " RESET);
}

void kernel_mode_menu() {
    int sub_choice, result;
    do{
        printf("\n" CYAN "==========================================" RESET);
        printf("\n" BOLD GREEN "         📂 Available Tasks Menu" RESET);
        printf("\n" CYAN "==========================================" RESET);
        printf("\n" MAGENTA "🔐 Current Mode: %s", "KERNEL");
        printf("\n" YELLOW " 1. View Queue Status");
        printf("\n 2. View Resource Status");
        printf("\n 3. Kill a Task (Kernel only)");
        printf("\n 0. Toggle User Mode");
        printf("\n" CYAN "==========================================" RESET);
        printf("\n" BOLD "Enter your choice: " RESET);
        kernel_menu();
        result = scanf("%d", &sub_choice);
        while (getchar() != '\n');
        if (result != 1) {
            printf("Invalid input!\n");
            continue;
        }

        switch (sub_choice) {
            case 1:
                if (current_mode == KERNEL_MODE) {
                    print_all_queues();
                } else {
                    printf("Access denied: Kernel mode required.\n");
                }
                break;
            case 2:
                if (current_mode == KERNEL_MODE) {
                    print_resource_status();
                } else {
                    printf("Access denied: Kernel mode required.\n");
                }
                break;
            case 3:
                if (current_mode != KERNEL_MODE) {
                    printf("Permission denied. Must be in KERNEL mode.\n");
                } else {
                    int pid_to_kill;
                    printf("Enter PID to kill: ");
                    scanf("%d", &pid_to_kill);
                    while (getchar() != '\n');
                    int found = 0;
                    for (int i = 0; i < task_count; i++) {
                        if (tasks[i].pid == pid_to_kill) {
                            kill(pid_to_kill, SIGKILL);
                            free_resources(tasks[i].ram, tasks[i].hdd, tasks[i].cores);
                            tasks[i].state = TERMINATED;
                            remove_task(pid_to_kill);
                            update_task_state(pid_to_kill, TERMINATED);
                            printf("Task %d terminated.\n", pid_to_kill);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) printf("Task not found.\n");
                }
                break;
            case 0:
                current_mode = USER_MODE;
                printf("Switched to %s mode.\n", current_mode == KERNEL_MODE ? "KERNEL" : "USER");
                break;
            default:
                printf("Invalid option.\n");
        }
    } while (sub_choice != 0);
    current_mode = USER_MODE;
}

int main() {
    init_resources();
    init_scheduler();
    boot_os();

    pthread_t scheduler_thread;
    pthread_create(&scheduler_thread, NULL, scheduler_thread_func, NULL);

    int choice;
    int result;

    do {
        check_task_completion();
        show_menu();
        //scanf("%d", &choice);

        result = scanf("%d", &choice);
        // Clear input buffer
        while (getchar() != '\n');
        if (result != 1) {
            printf("Invalid input!\n");
            choice = -1; // Ensure it doesn't trigger any valid case
            continue;
        }
        if (choice < 0 || choice > 9) {
            printf("Number out of range! Please enter a digit between 0 and 9.\n");
            continue;
        }

        switch (choice) {
            case 1:
                launch_task("./tasks/calculator", 100, 50, 0);
                break;
            case 2:
                launch_task("./tasks/notepad", 100, 50, 1);
                break;
            case 3:
                launch_task("./tasks/clock", 50, 30, 0);
                break;
            case 4:
                launch_task("./tasks/file_manager", 150, 100, 2);
                break;
            case 5:
                launch_task("./tasks/music", 50, 20, 0);
                break;
            case 6:
                launch_task("./tasks/minigame", 80, 40, 1);
                break;
             case 7:
                launch_task("./tasks/calender", 80, 40, 0);
                break;
            case 8:
                current_mode = KERNEL_MODE;
                kernel_mode_menu();
                break;
            case 0:
                graceful_shutdown();
                printf("🛑 Maham's OS is now shut down.\n");
                break;
            default:
                printf("Invalid option. Try again.\n");
        }

    } while (choice != 0);

    sprintf(log_message,"🛑 System shutdown.\n");
    log_event(log_message);
    //pthread_join(scheduler_thread, NULL);
    printf("********************************\n");
    return 0;
}
