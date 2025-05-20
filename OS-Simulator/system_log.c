#include "system_log.h"
#include <stdio.h>
#include <time.h>

// Log event function
void log_event(const char* event) {
    FILE* log_file = fopen("system.log", "a");
    if (log_file == NULL) {
        perror("Error opening log file");
        return;
    }

    // Get the current time
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    // Write log message with timestamp
    fprintf(log_file, "[%04d-%02d-%02d %02d:%02d:%02d] %s",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec, event);

    fclose(log_file);
}
