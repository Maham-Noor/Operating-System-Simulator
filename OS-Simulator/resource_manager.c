#include "resource_manager.h"
#include <stdio.h>

#define RESET   "\033[0m"
#define RED     "\033[0;31m"
#define BOLDRED "\033[1;31m"
#define WHITE   "\033[1;37m"
#define BOLD    "\033[1m"
#define YELLOW  "\033[1;33m"

SystemResources sys_res;

void init_resources() {
    sys_res.used_ram = 0;
    sys_res.used_hdd = 0;
    sys_res.used_cores = 0;
}

int allocate_resources(int ram, int hdd, int cores) {
    // Check if we have enough resources before allocating
    if (sys_res.used_ram + ram > MAX_RAM ||
        sys_res.used_hdd + hdd > MAX_HDD ||
        sys_res.used_cores + cores > MAX_CORES) {
        return 0; // Not enough resources
    }
    sys_res.used_ram += ram;
    sys_res.used_hdd += hdd;
    sys_res.used_cores += cores;

    return 1; // Success
}

void free_resources(int ram, int hdd, int cores) {
    sys_res.used_ram -= ram;
    if (sys_res.used_ram < 0) sys_res.used_ram = 0;

    sys_res.used_hdd -= hdd;
    if (sys_res.used_hdd < 0) sys_res.used_hdd = 0;

    sys_res.used_cores -= cores;
    if (sys_res.used_cores < 0) sys_res.used_cores = 0;

    //printf("[DEBUG] Freeing resources for PID-- : RAM=--, HDD=--, CORES=1\n");
}

void print_resource_status() {
    // Calculate remaining resources
    int remaining_ram = MAX_RAM - sys_res.used_ram;
    int remaining_hdd = MAX_HDD - sys_res.used_hdd;
    int remaining_cores = MAX_CORES - sys_res.used_cores;

    printf("\n%s%s SYSTEM RESOURCE STATUS %s\n", BOLD, RED, RESET);
    printf("%s=========================================%s\n", BOLDRED, RESET);
    printf("%s💾 RAM:%s     %s%d MB remaining%s\n", RED, RESET, WHITE, remaining_ram, RESET);
    printf("%s📀 HDD:%s     %s%d MB remaining%s\n", RED, RESET, WHITE, remaining_hdd, RESET);
    printf("%s⚙️ Cores:%s   %s%d available%s\n", RED, RESET, WHITE, remaining_cores, RESET);
    printf("%s=========================================%s\n\n", BOLDRED, RESET);
}
