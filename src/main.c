#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "keyboard.h"
#include "screen.h"
#include "timer.h"

#define PLAYER1 'V'  // Jogador 1 (ijkl)
#define PLAYER2 'A'  // Jogador 2 (WASD)
#define INITIAL_X1 10
#define INITIAL_Y1 10
#define INITIAL_X2 60
#define INITIAL_Y2 20

#define MAZE_WIDTH (MAXX - 2)
#define MAZE_HEIGHT (MAXY - 2)
#define MAX_NAME_LEN 50
#define MAX_PLAYERS 100

typedef struct {
    char name[MAX_NAME_LEN];
    int wins;
} PlayerRecord;

char maze[MAZE_HEIGHT + 2][MAZE_WIDTH + 2]; // Labirinto atual

int player1X = INITIAL_X1;
int player1Y = INITIAL_Y1;
int player2X = INITIAL_X2;
int player2Y = INITIAL_Y2;

char (*currentMaze)[MAZE_WIDTH + 2];

void movePlayer1(char direction);
void movePlayer2(char direction);
void drawPlayers();
void clearPlayers();
void generateMaze(char maze[MAZE_HEIGHT + 2][MAZE_WIDTH + 2]);
void drawMaze(char maze[MAZE_HEIGHT + 2][MAZE_WIDTH + 2]);
int checkCollision();
void updatePlayerRecord(const char *winnerName, const char *filename);
void loadPlayerRecords(const char *filename, PlayerRecord players[], int *playerCount);
void sortPlayersByWins(PlayerRecord players[], int playerCount);
void printPlayerRecords(PlayerRecord players[], int playerCount);

int main() {
    keyboardInit();
    int starting = 1, running = 1;
    char key;

    printf("██░░░░░░░▄█████▄░░░▄█████░░░██░░██░░░▄█████░░░██████▄░░░░░░░░░░░░██████░░░██████▄\n");
    printf("██░░░░░░░██░░░██░░░██░░░░░░░██░░██░░░██░░░░░░░██░░░██░░░░░░░░░░░░░░██░░░░░██░░░██\n");
    printf("██░░░░░░░██░░░██░░░██░░░░░░░█████░░░░█████░░░░██░░░██░░░░░░░░░░░░░░██░░░░░██░░░██\n");
    printf("██░░░░░░░██░░░██░░░██░░░░░░░██░░██░░░██░░░░░░░██░░░██░░░██████░░░░░██░░░░░██░░░██\n");
    printf("██████░░░▀█████▀░░░▀█████░░░██░░██░░░▀█████░░░██████▀░░░░░░░░░░░░██████░░░██░░░██\n");
    printf("Bem-vindo ao jogo! O assassino controla com WASD e a vítima com IJKL.\n");
    printf("Pressione espaço para começar!\n");

    while(starting) {
        if(keyhit()) {
            key = readch();

            if(key == 32) {
                screenInit(1);
                timerInit(100); // Inicializa o timer com um intervalo de 100ms
                srand(time(NULL)); // Inicializa o gerador de números aleatórios

                // Gerando o labirinto
                generateMaze(maze);
                drawMaze(maze);
                drawPlayers();

                // Define o labirinto inicial
                currentMaze = maze;
                
                while(running) {
                    if(keyhit()) {
                        key = readch();

                        if(key == 27) {
                            running = 0;
                            starting = 0;
                        } else {
                            if(key == 'w' || key == 'a' || key == 's' || key == 'd') {
                                movePlayer2(key);
                            } else if(key == 'i' || key == 'j' || key == 'k' || key == 'l') {
                                movePlayer1(key);
                            }
                            
                            if(checkCollision()) {
                                char winnerName[MAX_NAME_LEN];
                                screenClear();
                                printf("Colisão! Digite o nome do vencedor: ");
                                fgets(winnerName, MAX_NAME_LEN, stdin);
                                winnerName[strcspn(winnerName, "\n")] = '\0'; // Remover o caractere de nova linha

                                updatePlayerRecord(winnerName, "arquivo.txt");
                                running = 0;
                                starting = 0;
                            }
                        }
                    }
                }
            } else if(key == 27) {
                starting = 0;
            }
        }
    }

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    // Exibir o ranking dos vencedores
    PlayerRecord players[MAX_PLAYERS];
    int playerCount = 0;
    loadPlayerRecords("arquivo.txt", players, &playerCount);
    sortPlayersByWins(players, playerCount);
    printPlayerRecords(players, playerCount);

    return 0;
}

void updatePlayerRecord(const char *winnerName, const char *filename) {
    PlayerRecord players[MAX_PLAYERS];
    int playerCount = 0;
    int found = 0;

    // Abre o arquivo em modo de leitura para carregar dados existentes
    FILE *file = fopen(filename, "r");
    if (file) {
        while (fscanf(file, "%s %d", players[playerCount].name, &players[playerCount].wins) == 2) {
            if (strcmp(players[playerCount].name, winnerName) == 0) {
                players[playerCount].wins++;
                found = 1;
            }
            playerCount++;
            if (playerCount >= MAX_PLAYERS) break;
        }
        fclose(file);
    }

    // Se o vencedor não foi encontrado, adiciona-o ao final da lista
    if (!found && playerCount < MAX_PLAYERS) {
        strncpy(players[playerCount].name, winnerName, MAX_NAME_LEN);
        players[playerCount].wins = 1;
        playerCount++;
    }

    // Abre o arquivo em modo de adição e grava apenas o novo vencedor
    if (!found) {
        file = fopen(filename, "a");
        if (file) {
            fprintf(file, "%s %d\n", winnerName, 1);
            fclose(file);
        } else {
            printf("Erro ao abrir o arquivo %s para gravação.\n", filename);
        }
    } else {
        // Sobrescreve o arquivo somente quando o vencedor já existe
        file = fopen(filename, "w");
        if (file) {
            for (int i = 0; i < playerCount; i++) {
                fprintf(file, "%s %d\n", players[i].name, players[i].wins);
            }
            fclose(file);
        } else {
            printf("Erro ao abrir o arquivo %s para gravação.\n", filename);
        }
    }
}

void loadPlayerRecords(const char *filename, PlayerRecord players[], int *playerCount) {
    *playerCount = 0;
    FILE *file = fopen(filename, "r");
    
    if (file) {
        while (fscanf(file, "%s %d", players[*playerCount].name, &players[*playerCount].wins) == 2) {
            (*playerCount)++;
            if (*playerCount >= MAX_PLAYERS) break;
        }
        fclose(file);
    } else {
        printf("Erro ao abrir o arquivo %s.\n", filename);
    }
}

void sortPlayersByWins(PlayerRecord players[], int playerCount) {
    for (int i = 0; i < playerCount - 1; i++) {
        for (int j = i + 1; j < playerCount; j++) {
            if (players[i].wins < players[j].wins) {
                PlayerRecord temp = players[i];
                players[i] = players[j];
                players[j] = temp;
            }
        }
    }
}

void printPlayerRecords(PlayerRecord players[], int playerCount) {
    printf("\n\t\t\tRanking de Vencedores:\n");
    
    for (int i = 0; i < playerCount; i++) {
        printf("\t\t\t%s: %d vitórias\n", players[i].name, players[i].wins);
    }
}

void drawPlayers() {
    screenSetColor(RED, BLACK);  // Assassino
    screenGotoxy(player2X + 1, player2Y + 1);
    printf("%c", PLAYER2);

    screenSetColor(WHITE, DARKGRAY);  // Presa
    screenGotoxy(player1X + 1, player1Y + 1);
    printf("%c", PLAYER1);

    screenUpdate();
}

void clearPlayers() {
    screenGotoxy(player1X + 1, player1Y + 1);
    printf(" ");
    screenGotoxy(player2X + 1, player2Y + 1);
    printf(" ");
    screenUpdate();
}

void movePlayer1(char direction) {
    clearPlayers();
    int newX = player1X, newY = player1Y;

    switch (direction) {
        case 'i': newY--; break;
        case 'k': newY++; break;
        case 'j': newX--; break;
        case 'l': newX++; break;
    }

    if (currentMaze[newY][newX] == ' ') {
        player1X = newX;
        player1Y = newY;
    }

    drawPlayers();
}

void movePlayer2(char direction) {
    clearPlayers();
    int newX = player2X, newY = player2Y;

    switch (direction) {
        case 'w': newY--; break;
        case 's': newY++; break;
        case 'a': newX--; break;
        case 'd': newX++; break;
    }

    if (currentMaze[newY][newX] == ' ') {
        player2X = newX;
        player2Y = newY;
    }

    drawPlayers();
}

int checkCollision() {
    return player1X == player2X && player1Y == player2Y;
}

void generateMaze(char maze[MAZE_HEIGHT + 2][MAZE_WIDTH + 2]) {
    for (int x = 0; x < MAZE_WIDTH + 2; x++) {
        maze[0][x] = '-';
        maze[MAZE_HEIGHT + 1][x] = '-';
    }
    for (int y = 1; y <= MAZE_HEIGHT; y++) {
        maze[y][0] = '|';
        maze[y][MAZE_WIDTH + 1] = '|';
    }

    for (int y = 1; y <= MAZE_HEIGHT; y++) {
        for (int x = 1; x <= MAZE_WIDTH; x++) {
            if (rand() % 5 == 0) {
                maze[y][x] = '|';
            } else {
                maze[y][x] = ' ';
            }
        }
    }

    maze[MAZE_HEIGHT + 1][MAZE_WIDTH + 1] = '\0';
}

void drawMaze(char maze[MAZE_HEIGHT + 2][MAZE_WIDTH + 2]) {
    screenSetColor(WHITE, BLACK);
    for (int y = 0; y < MAZE_HEIGHT + 2; y++) {
        for (int x = 0; x < MAZE_WIDTH + 2; x++) {
            screenGotoxy(x + 1, y + 1);
            printf("%c", maze[y][x]);
        }
    }
    screenUpdate();
}
