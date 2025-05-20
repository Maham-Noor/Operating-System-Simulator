#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

#define WIDTH 40
#define HEIGHT 15
#define MAX_LENGTH 100

// Color Definitions
#define BOLD    "\x1b[1m"
#define RESET   "\x1b[0m"
#define GREEN   "\x1b[32m"
#define RED     "\x1b[31m"
#define CYAN    "\x1b[36m"
#define YELLOW  "\x1b[33m"

int gameOver, score;
int x, y, fruitX, fruitY, tailX[MAX_LENGTH], tailY[MAX_LENGTH];
int nTail;  // Number of tail segments
int dir;    // 0 = LEFT, 1 = RIGHT, 2 = UP, 3 = DOWN

// kbhit() function to check if a key is pressed
int kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;  // A key was pressed
    }

    return 0;  // No key was pressed
}

// Setup the initial game state
void Setup() {
    gameOver = 0;
    dir = 1;  // Start moving right
    x = WIDTH / 2;
    y = HEIGHT / 2;
    fruitX = rand() % WIDTH;
    fruitY = rand() % HEIGHT;
    score = 0;
    nTail = 0;
}

// Draw the game state on the console
void Draw() {
    system("clear");  // Clear screen (Use "cls" on Windows)
    for (int i = 0; i < WIDTH + 2; i++) printf(CYAN "#");
    printf(RESET "\n");

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (j == 0) printf(CYAN "#");  // Left border
            if (i == y && j == x) printf(BOLD GREEN "O");  // Snake head
            else if (i == fruitY && j == fruitX) printf(BOLD RED "F");  // Fruit
            else {
                int isTail = 0;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        printf(GREEN "o");
                        isTail = 1;
                    }
                }
                if (!isTail) printf(" ");
            }
            if (j == WIDTH - 1) printf(CYAN "#");  // Right border
        }
        printf(RESET "\n");
    }

    for (int i = 0; i < WIDTH + 2; i++) printf(CYAN "#");
    printf(RESET "\n");
    printf(YELLOW "Score: %d\n", score);
}

// Input function to handle user keyboard input
void Input() {
    if (kbhit()) {
        char ch = getchar(); // Get the key pressed
        switch (ch) {
            case 'a': dir = 0; break;  // Left
            case 'd': dir = 1; break;  // Right
            case 'w': dir = 2; break;  // Up
            case 's': dir = 3; break;  // Down
            case 'x': gameOver = 1; break;  // Quit game
        }
    }
}

// Game logic to update the snake's position and check for collisions
void Logic() {
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;

    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    switch (dir) {
        case 0: x--; break;  // Left
        case 1: x++; break;  // Right
        case 2: y--; break;  // Up
        case 3: y++; break;  // Down
    }

    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) gameOver = 1;  // Wall collision
    for (int i = 0; i < nTail; i++) {
        if (tailX[i] == x && tailY[i] == y) gameOver = 1;  // Self-collision
    }

    if (x == fruitX && y == fruitY) {
        score += 10;
        fruitX = rand() % WIDTH;
        fruitY = rand() % HEIGHT;
        nTail++;
    }
}


// Main function to run the game loop
int main() {
        char playAgain;

    do {
        Setup();
        while (!gameOver) {
            Draw();
            Input();
            Logic();
            usleep(200000);  // Sleep for 100ms (0.1 seconds)
        }

        printf(RED "Game Over! Final Score: %d\n" RESET, score);

        // Clear input buffer
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);

        // Ask if user wants to play again
        printf("Do you want to play again? (y/n): ");
        scanf("%c", &playAgain);

    } while (playAgain == 'y' || playAgain == 'Y');

    return 0;
}
