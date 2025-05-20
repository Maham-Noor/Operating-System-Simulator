#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <sys/types.h> // for pid_t

// Task states
typedef enum {
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} TaskState;

// Unified Task structure
typedef struct {
    pid_t pid;
    char name[100];       // Executable name/path
    int priority;         // Lower = higher priority (used in PRIORITY queue)
    int ram;
    int hdd;
    int cores;
    TaskState state;
} Task;

// Linked list node for tasks
typedef struct TaskNode {
    Task task;
    struct TaskNode* next;
} TaskNode;

// Queue structure
typedef struct {
    TaskNode* front;
    TaskNode* rear;
    int size;
} TaskQueue;

// Scheduler API
void init_scheduler();
void run_scheduler();
void add_task(Task task);
Task* get_next_task(); // Returns pointer to next task to schedule
void update_task_state(pid_t pid, TaskState new_state);
void print_all_queues();
void remove_task(pid_t pid);
const char* get_task_state_name(TaskState state);
TaskNode* get_rr_queue_front();


#endif
