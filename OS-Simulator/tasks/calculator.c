#include <stdio.h>
#include <unistd.h>

// ANSI escape codes for better contrast on white background
#define RED     "\x1b[31m"
#define BLACK   "\x1b[30;1m"   // Bold black
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define RESET   "\x1b[0m"

int main() {
    int a, b;
    char op;
    char choice;

    printf(BLUE "╔══════════════════════════════╗\n");
    printf("║         CALCULATOR           ║\n");
    printf("╚══════════════════════════════╝\n" RESET);

    do {
        printf("\n" MAGENTA "➤ Enter expression (e.g. 3 + 5): " RESET);
        if (scanf("%d %c %d", &a, &op, &b) != 3) {
            printf(RED "✘ Invalid input format.\n" RESET);
            while(getchar() != '\n'); // clear input buffer
            continue;
        }

        printf(BLACK "\n───────── Result ──────────\n" RESET);
        switch(op) {
            case '+':
                printf("   %d + %d = %d\n", a, b, a + b);
                break;
            case '-':
                printf("   %d - %d = %d\n", a, b, a - b);
                break;
            case '*':
                printf("   %d * %d = %d\n", a, b, a * b);
                break;
            case '/':
                if (b == 0) {
                    printf(RED "  Error: Division by zero.\n" RESET);
                } else {
                    printf("   %d / %d = %d\n", a, b, a / b);
                }
                break;
            default:
                printf(RED "   Unsupported operation: %c\n" RESET, op);
        }
        printf(BLACK "───────────────────────────\n" RESET);

        printf(MAGENTA "\nDo you want another calculation? (y/n): " RESET);
        while (getchar() != '\n'); // clear input buffer
        choice = getchar();

    } while (choice == 'y' || choice == 'Y');

    printf(BLUE "\nGoodbye! Thank you for using the calculator.\n" RESET);
    sleep(2);
    return 0;
}
