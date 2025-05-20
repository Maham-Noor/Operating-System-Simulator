#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>

// ANSI color codes
#define CYAN    "\x1b[36m"
#define YELLOW  "\x1b[33m"
#define GREEN   "\x1b[32m"
#define BOLD    "\x1b[1m"
#define RESET   "\x1b[0m"

void clear_screen() {
    printf("\033[2J\033[1;1H");
}

// Make stdin non-blocking
void set_nonblocking_input(int enable) {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);
    if (enable) {
        ttystate.c_lflag &= ~ICANON;
        ttystate.c_lflag &= ~ECHO;
        ttystate.c_cc[VMIN] = 0;
        ttystate.c_cc[VTIME] = 0;
    } else {
        ttystate.c_lflag |= ICANON;
        ttystate.c_lflag |= ECHO;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

int main() {
    char ch;
    set_nonblocking_input(1);

    while (1) {
        clear_screen();
        time_t now = time(NULL);
        struct tm *local = localtime(&now);

        printf(BOLD CYAN "╔═════════════════════════════════════╗\n");
        printf("║            MyOS CLOCK               ║\n");
        printf("╚═════════════════════════════════════╝\n\n" RESET);

        printf(BOLD YELLOW "   Time: " RESET BOLD "%02d:%02d:%02d\n", 
               local->tm_hour, local->tm_min, local->tm_sec);
        printf(BOLD GREEN "   Date: " RESET BOLD "%04d-%02d-%02d\n", 
               local->tm_year + 1900, local->tm_mon + 1, local->tm_mday);

        printf(RESET "\n" CYAN "Press 'x' to exit.\n" RESET);

        usleep(500000);  // Sleep for 0.5 sec

        // Non-blocking check for 'q'
        if (read(STDIN_FILENO, &ch, 1) > 0) {
            if (ch == 'x' || ch == 'X') {
                break;
            }
        }
    }

    set_nonblocking_input(0);  // Restore terminal settings
    printf("\n" GREEN "Clock exited.\n" RESET);
    return 0;
}
