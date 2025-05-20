#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#define MAX_RAM 2048  // MB
#define MAX_HDD 262144  // MB
#define MAX_CORES 8

typedef struct {
    int used_ram;
    int used_hdd;
    int used_cores;
} SystemResources;

extern SystemResources sys_res;

void init_resources();
int allocate_resources(int ram, int hdd, int cores);
void free_resources(int ram, int hdd, int cores);
void print_resource_status();

#endif

