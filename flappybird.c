#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 20
#define BIRD_X 5

HANDLE consoleHandle;

int birdY, pipeX, gapY, score, gameOver;
int PIPE_SPEED, GRAVITY, FLAP_FORCE, GAP_SIZE, GAME_SPEED;

char playerName[30];
int highScore = 0;
char highScorePlayer[30] = "None";
char birdIcon = '>';   // DEFAULT BIRD

// Utility

void gotoXY(int x, int y) {
    COORD c = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(consoleHandle, c);
}

void hideCursor() {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 1;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

void clearScreen() {
    system("cls");
}

void setColor(int color) {
    SetConsoleTextAttribute(consoleHandle, color);
}

void printAnimated(const char *text, int delay, int color) {
    setColor(color);
    for (int i = 0; text[i] != '\0'; i++) {
        putchar(text[i]);
        Sleep(delay);
    }
    setColor(7);
}

// High Score Handling 

void createDefaultHighScoreFile() {
    FILE *file = fopen("highscore.txt", "w");
    if (file != NULL) {
        fprintf(file, "0 None >\n");
        fclose(file);
    }
    highScore = 0;
    strcpy(highScorePlayer, "None");
    birdIcon = '>';
}

void loadHighScore() {
    FILE *file = fopen("highscore.txt", "r");

    if (file == NULL) {
        createDefaultHighScoreFile();
        return;
    }

    int read = fscanf(file, "%d %29s %c", &highScore, highScorePlayer, &birdIcon);
    fclose(file);

    if (read < 2) createDefaultHighScoreFile();
    if (read == 2) birdIcon = '>';  // old format support
}

void saveHighScore() {
    FILE *file = fopen("highscore.txt", "w");
    if (!file) return;

    fprintf(file, "%d %s %c\n", highScore, highScorePlayer, birdIcon);
    fclose(file);
}

// Difficulty level

void setDifficulty() {
    int choice;
    clearScreen();
    printAnimated("\nSelect Difficulty\n", 10, 11);

    printf("------------------\n");
    printf("1. Easy\n2. Medium\n3. Hard\n\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            PIPE_SPEED = 1;
            GRAVITY = 1;
            FLAP_FORCE = 3;
            GAP_SIZE = 7;
            GAME_SPEED = 40;
            break;
        case 2:
            PIPE_SPEED = 1;
            GRAVITY = 1;
            FLAP_FORCE = 2;
            GAP_SIZE = 5;
            GAME_SPEED = 25;
            break;
        case 3:
            PIPE_SPEED = 2;
            GRAVITY = 2;
            FLAP_FORCE = 2;
            GAP_SIZE = 4;
            GAME_SPEED = 15;
            break;
        default:
            PIPE_SPEED = 1;
            GRAVITY = 1;
            FLAP_FORCE = 2;
            GAP_SIZE = 5;
            GAME_SPEED = 25;
    }

    while (getchar() != '\n');
}

//  Shop bird icon

void showShop() {
    clearScreen();
    printAnimated("\nBIRD SHOP\n", 20, 13);
    printf("Your High Score: %d\n\n", highScore);

    printf("1. >   (Unlocked)\n");
    if (highScore >= 5)  printf("2. *   (Unlocked)\n");
    else printf("2. *   (Requires 5)\n");

    if (highScore >= 10) printf("3. .   (Unlocked)\n");
    else printf("3. .   (Requires 10)\n");

    if (highScore >= 20) printf("4. ,   (Unlocked)\n");
    else printf("4. ,   (Requires 20)\n");

    if (highScore >= 30) printf("5. @   (Unlocked)\n");
    else printf("5. @   (Requires 30)\n");

    if (highScore >= 50) printf("6. #   (Unlocked)\n");
    else printf("6. #   (Requires 50)\n");

    printf("\nEnter choice (0 to exit): ");

    int choice;
    scanf("%d", &choice);

    switch (choice) {
        case 1: birdIcon = '>'; break;
        case 2: if (highScore >= 5)  birdIcon = '*'; break;
        case 3: if (highScore >= 10) birdIcon = '.'; break;
        case 4: if (highScore >= 20) birdIcon = ','; break;
        case 5: if (highScore >= 30) birdIcon = '@'; break;
        case 6: if (highScore >= 50) birdIcon = '#'; break;
        case 0: return;
        default:
            printf("Invalid choice!");
            Sleep(800);
            return;
    }

    saveHighScore();
    printf("Bird Updated!\n");
    Sleep(800);
}

//  complete Game loop 

void initGame() {
    hideCursor();
    srand((unsigned)time(NULL));

    birdY = HEIGHT / 2;
    pipeX = WIDTH - 1;
    gapY = rand() % (HEIGHT - GAP_SIZE - 4) + 3;

    score = 0;
    gameOver = 0;
}

void drawGame() {
    gotoXY(0, 0);

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {

            if (x == 0 || x == WIDTH - 1) putchar('|');
            else if (x == BIRD_X && y == birdY) putchar(birdIcon);
            else if (x == pipeX && (y < gapY - GAP_SIZE/2 || y > gapY + GAP_SIZE/2))
                putchar('#');
            else putchar(' ');
        }
        putchar('\n');
    }

    for (int i = 0; i < WIDTH; i++) putchar('-');

    printf("\nScore: %d   High Score: %d (%s)  Bird: %c",
           score, highScore, highScorePlayer, birdIcon);
}

void handleInput() {
    if (kbhit()) {
        int c = getch();
        if (c == ' ' || c == 'w' || c == 'W') birdY -= FLAP_FORCE;
        if (c == 27) gameOver = 1;
    }
}
// Loader 
void showLoader() {
    clearScreen();
    hideCursor();
    char *text = "LOADING ASCII FLAPPY BIRD";
    int len = strlen(text);
    int row = HEIGHT / 2;
    int col = (WIDTH - len) / 2;

    for (int i = 0; i <= 100; i += 10) {
        gotoXY(col, row);
        printf("%s [", text);
        int pos = i / 2;
        for (int j = 0; j < 50; j++) {
            if (j <= pos) putchar('#');
            else putchar(' ');
        }
        printf("] %d%%", i);
        Sleep(100);
    }

    Sleep(500);
    clearScreen();  // <-- Clear loader before showing menu
}

void updateGame() {
    birdY += GRAVITY;
    pipeX -= PIPE_SPEED;

    if (pipeX <= 0) {
        pipeX = WIDTH - 1;
        gapY = rand() % (HEIGHT - GAP_SIZE - 4) + 3;
        score++;
    }

    if (birdY <= 0 || birdY >= HEIGHT - 1) gameOver = 1;
    if (pipeX == BIRD_X &&
       (birdY < gapY - GAP_SIZE/2 || birdY > gapY + GAP_SIZE/2))
        gameOver = 1;
}

void startGame() {
    initGame();

    while (!gameOver) {
        drawGame();
        handleInput();
        updateGame();
        Sleep(GAME_SPEED);
    }

    gotoXY(0, HEIGHT + 2);
    printf("\nGAME OVER! Score: %d\n", score);

    if (score > highScore) {
        printf("NEW HIGH SCORE!\n");
        highScore = score;
        strcpy(highScorePlayer, playerName);
        saveHighScore();
    }

    printf("Press any key to return...");
    getch();
}

// Main Menu 

void showInstructions() {
    clearScreen();
    printAnimated("\nINSTRUCTIONS\n", 20, 11);

    printf("1. Press SPACE or W to jump.\n");
    printf("2. Avoid pipes.\n");
    printf("3. Do not hit top or bottom.\n");
    printf("4. Score increases whenever you pass a pipe.\n\n");
    printf("Press any key to return...");
    getch();
}

void showAbout() {
    clearScreen();
    printAnimated("\nABOUT THE GAME\n", 20, 14);

    printf("Project: ASCII Flappy Bird Game in C\n");
    printf("Description: This is a console-based recreation of the popular Flappy Bird game.\n");
    printf("The game includes difficulty levels, a bird shop, high score tracking, and interactive gameplay.\n");

    // Animate the "Made by" line
    printAnimated("Made by: Muhammad Shuraim\n", 20, 10);
    printf("Press any key to return...");
    getch();
}

void showMenu() {
    loadHighScore();

    showLoader();  // Show loader only once

    int choice;

    while (1) {
        clearScreen();
        printAnimated("\nASCII FLAPPY BIRD\n", 20, 10);

        printf("------------------------\n");
        printf("1. Start Game\n");
        printf("2. Instructions\n");
        printf("3. About\n");
        printf("4. Shop\n");
        printf("5. Exit\n");
        printf("------------------------\n");
        printf("Enter choice: ");

        scanf("%d", &choice);

        switch (choice) {
            case 1:
                clearScreen();
                printf("Enter your name: ");
                scanf("%29s", playerName);
                setDifficulty();
                startGame();
                break;

            case 2:
                showInstructions();
                break;

            case 3:
                showAbout();
                break;

            case 4:
                showShop();
                break;

            case 5:
                clearScreen();
                printf("Goodbye!\n");
                Sleep(500);
                exit(0);

            default:
                printf("Invalid choice!");
                getch();
        }
    }
}

// MAIN 

int main() {
	
    consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    showMenu();
    return 0;
}

