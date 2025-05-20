#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define BOLD        "\x1b[1m"
#define MAGENTA     "\x1b[35m"
#define BLUE        "\x1b[34m"
#define RESET       "\x1b[0m"

int main() {
    // Clear screen and show header
    printf("\033[2J\033[1;1H");
    printf(BOLD MAGENTA "╔═════════════════════════════════════╗\n");
    printf("║            MyOS MUSIC               ║\n");
    printf("╚═════════════════════════════════════╝\n" RESET);

    printf(BOLD BLUE "\nNow playing: miccheck.wav 🎵\n" RESET);

    // Start music in sync (blocking) and close terminal window after music finishes
    system("powershell.exe -Command \"(New-Object Media.SoundPlayer 'C:\\\\Windows\\\\Media\\\\miccheck.wav').PlaySync(); exit;\""); 
    //new path == 'Desktop\\\\maham_noor\\\\tasks\\\\miccheck.wav'

    // After music completes, exit the program
    printf("\nMusic completed. \nExiting...\n");
    return 0;
}
