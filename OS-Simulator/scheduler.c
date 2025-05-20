#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>  
#include <signal.h>
#include "resource_manager.h" 
#include "scheduler.h"
#include "system_log.h"

#define MAX_QUEUES 1
#define TIME_SLICE 10 // Time slice duration (in terms of how many tasks before switching)
#define MAGENTA "\033[1;35m"
#define BOLD    "\x1b[1m"
#define GREEN   "\x1b[32m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

// Global Round-Robin queue
static TaskQueue rr_queue = {NULL, NULL, 0};

// Initialize all queues
void init_scheduler() {
    rr_queue.front = NULL;
    rr_queue.rear = NULL;
    rr_queue.size = 0; 
    printf("Round-Robin Scheduler initialized.\n");
}

// Create and allocate a task node
static TaskNode* create_task_node(Task task) {
    TaskNode* node = (TaskNode*)malloc(sizeof(TaskNode));
    if (!node) {
        perror("Failed to allocate task node");
        exit(EXIT_FAILURE);
    }
    node->task = task;
    node->next = NULL;
    return node;
}

// Add task to Round-Robin queue
void add_task(Task task) {
    task.state = READY;  // New tasks are always READY initially
    TaskNode* node = create_task_node(task);

    if (rr_queue.rear == NULL) {
        rr_queue.front = node;
        rr_queue.rear = node;
    } else {
        rr_queue.rear->next = node;
        rr_queue.rear = node;
    }
    rr_queue.size++;

    //printf("Task '%s' (PID: %d) added to queue.\n", task.name, task.pid);
}

// Fetch next task (simple FIFO for now)
Task* get_next_task() {
    
        if (rr_queue.front != NULL) {
            TaskNode* node = rr_queue.front;
            rr_queue.front = node->next;

            if (rr_queue.front == NULL) {
                rr_queue.rear = NULL;
            }
            rr_queue.size--;

            Task* next_task = (Task*)malloc(sizeof(Task));
            *next_task = node->task;
            free(node);

            next_task->state = RUNNING;
            //printf("Task '%s' (PID: %d) dequeued from queue.\n", next_task->name, next_task->pid);
            return next_task;
        }
        return NULL;
    }

// Update the state of a task by PID
void update_task_state(pid_t pid, TaskState new_state) {

        TaskNode* current = rr_queue.front;
        while (current != NULL) {
            if ((int)current->task.pid == (int)pid) {
                current->task.state = new_state;
                printf("Task PID %d state updated to %s.\n", pid, get_task_state_name(new_state));
                return;
            }
            current = current->next;
        }
    printf("Task PID %d not found in any queue.\n", pid);
}

// Debug print of the queue
void print_all_queues() {
        printf("\n%s=================================================%s", CYAN, RESET);
        printf("\n%s%s         🚀 Round Robin Queue%s", BOLD, GREEN, RESET);
        printf("\n%s=================================================%s\n", CYAN, RESET);
        TaskNode* current = rr_queue.front;

        while (current) {
            printf("%s  Task: %s (PID: %d, State: %d)%s\n", MAGENTA, 
                   current->task.name, current->task.pid, current->task.state, RESET);
            current = current->next;
        }

        if (rr_queue.front == NULL) {
            printf("  [Empty]\n");
        }
        printf("\n%s=================================================%s",CYAN ,RESET);
}

void remove_task(pid_t pid) {
    
        TaskNode* current = rr_queue.front;
        TaskNode* prev = NULL;

        while (current != NULL) {
            if (current->task.pid == pid) {
                // Found the task to remove
                if (prev == NULL) {
                    // Task is at the front
                    rr_queue.front = current->next;
                    if (rr_queue.rear == current) {
                        rr_queue.rear = NULL;
                    }
                } else {
                    prev->next = current->next;
                    if (rr_queue.rear == current) {
                        rr_queue.rear = prev;
                    }
                }

                rr_queue.size--; //Decrease size when removing
                printf("Task with PID %d removed from queue.\n", pid);
                char log_message[256];
                sprintf(log_message, "Terminated task with PID %d.", pid);
                log_event(log_message);
                printf("_____________________________________\n");
                free(current);
                return;
            }

            prev = current;
            current = current->next;
        }

    //printf("Task with PID %d not found in any queue.\n", pid);
}

// Scheduler function to run tasks based on scheduling algorithm
void run_scheduler() {
    sleep(5);
    printf("\nRunning scheduler...\n");

    while (1) {
        Task* task_ptr = get_next_task();

        if (task_ptr == NULL) {
            //printf("No tasks in the queue. Scheduler is idle.\n");
            //sleep(1);
            continue;
        }

        task_ptr->state = RUNNING;
        //printf("Running task: %s (PID: %d)\n", task_ptr->name, task_ptr->pid);

        int status;
        pid_t result = waitpid(task_ptr->pid, &status, WNOHANG);
        if (result == task_ptr->pid) {
            // Task has finished
            free_resources(task_ptr->ram, task_ptr->hdd, task_ptr->cores);
            task_ptr->state = TERMINATED;
            printf("\nTask with PID %d completed.\n", task_ptr->pid);
            char log_message[256];
            sprintf(log_message, "Task with PID %d completed.\n\n", task_ptr->pid);
            log_event(log_message);
            printf("________________________________\n");
            print_resource_status();
        } else if (result == 0) {
            // Still running, put back in queue
            //printf("Task %d still running. Re-queuing it.\n", task_ptr->pid);
            task_ptr->state = READY;
            add_task(*task_ptr);
        } else {
            perror("waitpid failed");
        }
        free(task_ptr);
        //sleep(3);
    }
}

const char* get_task_state_name(TaskState state) {
    switch (state) {
        case READY: return "READY";
        case RUNNING: return "RUNNING";
        case BLOCKED: return "BLOCKED";
        case TERMINATED: return "TERMINATED";
        default: return "UNKNOWN";
    }
}

TaskNode* get_rr_queue_front() {
    return rr_queue.front;
}