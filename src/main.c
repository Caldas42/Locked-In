#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "keyboard.h"
#include "screen.h"
#include "timer.h"

#define COMPRIMENTO_LABIRINTO (MAXX - 2)
#define LARGURA_LABIRINTO (MAXY - 2)

#define JOGADOR1 '0'
#define JOGADOR2 'A'
#define X1_INICIAL 10
#define Y1_INICIAL 10
#define X2_INICIAL 60
#define Y2_INICIAL 20

int jogador1X = X1_INICIAL;
int jogador1Y = Y1_INICIAL;
int jogador2X = X2_INICIAL;
int jogador2Y = Y2_INICIAL;

#define MAX_NAME_LEN 50
#define MAX_PLAYERS 100

typedef struct {
    char name[MAX_NAME_LEN];
    int wins;
} PlayerRecord;

char (*currentMaze)[COMPRIMENTO_LABIRINTO + 2];

void gerarLabirinto(char labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2]);
void printarLabirinto(char labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2]);
void printarJogadores();

void movePlayer1(char direction);
void movePlayer2(char direction);
void clearPlayers();
int checkCollision();
void updatePlayerRecord(const char *winnerName, const char *filename);
void loadPlayerRecords(const char *filename, PlayerRecord players[], int *playerCount);
void sortPlayersByWins(PlayerRecord players[], int playerCount);
void printPlayerRecords(PlayerRecord players[], int playerCount);

int main() {
    keyboardInit();
    int menu = 1, running = 1;
    char key, labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2];
    srand(time(NULL));
    gerarLabirinto(labirinto);

    printf("██░░░░░░░▄█████▄░░░▄█████░░░██░░██░░░▄█████░░░██████▄░░░░░░░░░░░░██████░░░██████▄\n");
    printf("██░░░░░░░██░░░██░░░██░░░░░░░██░░██░░░██░░░░░░░██░░░██░░░░░░░░░░░░░░██░░░░░██░░░██\n");
    printf("██░░░░░░░██░░░██░░░██░░░░░░░█████░░░░█████░░░░██░░░██░░░░░░░░░░░░░░██░░░░░██░░░██\n");
    printf("██░░░░░░░██░░░██░░░██░░░░░░░██░░██░░░██░░░░░░░██░░░██░░░██████░░░░░██░░░░░██░░░██\n");
    printf("██████░░░▀█████▀░░░▀█████░░░██░░██░░░▀█████░░░██████▀░░░░░░░░░░░░██████░░░██░░░██\n");
    printf("Você (WASD) acorda preso em um labirinto e o Assassino (IJKL) está em sua procura! Fuja até o tempo acabar ou seja a sua próxima vítma.\n");
    printf("Pressione espaço para começar.\n");

    while(menu) {
        if(keyhit()) {
            key = readch();

            if(key == 32) {
                screenInit(1);
                printarLabirinto(labirinto);
                printarJogadores();

                // Define o labirinto inicial
                currentMaze = labirinto;
                
                while(running) {
                    if(keyhit()) {
                        key = readch();

                        if(key == 27) {
                            running = 0;
                            menu = 0;
                        } else {
                            if(key == 'w' || key == 'a' || key == 's' || key == 'd') {
                                movePlayer1(key);
                            } else if(key == 'i' || key == 'j' || key == 'k' || key == 'l') {
                                movePlayer2(key);
                            }
                            
                            if(checkCollision()) {
                                char winnerName[MAX_NAME_LEN];
                                screenClear();
                                printf("Colisão! Digite o nome do vencedor: ");
                                fgets(winnerName, MAX_NAME_LEN, stdin);
                                winnerName[strcspn(winnerName, "\n")] = '\0'; // Remover o caractere de nova linha

                                updatePlayerRecord(winnerName, "arquivo.txt");
                                running = 0;
                                menu = 0;
                            }
                        }
                    }
                }
            } else if(key == 27) {
                menu = 0;
            }
        }
    }

    keyboardDestroy();
    screenDestroy();

    // Exibir o ranking dos vencedores
    PlayerRecord players[MAX_PLAYERS];
    int playerCount = 0;
    loadPlayerRecords("arquivo.txt", players, &playerCount);
    sortPlayersByWins(players, playerCount);
    printPlayerRecords(players, playerCount);

    return 0;
}

void gerarLabirinto(char labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2]) {
    for(int i = 0; i < COMPRIMENTO_LABIRINTO + 2; i++) {
        labirinto[0][i] = '-';
        labirinto[LARGURA_LABIRINTO + 1][i] = '-';
    }

    for(int i = 1; i <= LARGURA_LABIRINTO; i++) {
        labirinto[i][0] = '|';
        labirinto[i][COMPRIMENTO_LABIRINTO + 1] = '|';
    }

    for(int i = 1; i <= LARGURA_LABIRINTO; i++) {
        for(int j = 1; j <= COMPRIMENTO_LABIRINTO; j++) {
            if(rand() % 5 == 0) {
                labirinto[i][j] = '|';
            } else {
                labirinto[i][j] = ' ';
            }
        }
    }
}

void printarLabirinto(char labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2]) {
    screenSetColor(WHITE, BLACK);

    for(int i = 0; i < LARGURA_LABIRINTO + 2; i++) {
        for(int j = 0; j < COMPRIMENTO_LABIRINTO + 2; j++) {
            screenGotoxy(j + 1, i + 1);
            printf("%c", labirinto[i][j]);
        }
    }

    screenUpdate();
}

void printarJogadores() {
    screenSetColor(BLUE, DARKGRAY);
    screenGotoxy(jogador1X + 1, jogador1Y + 1);
    printf("%c", JOGADOR1);

    screenSetColor(RED, BLACK);
    screenGotoxy(jogador2X + 1, jogador2Y + 1);
    printf("%c", JOGADOR2);

    screenUpdate();
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

void clearPlayers() {
    screenGotoxy(jogador1X + 1, jogador1Y + 1);
    printf(" ");
    screenGotoxy(jogador2X + 1, jogador2Y + 1);
    printf(" ");
    screenUpdate();
}

void movePlayer1(char direction) {
    clearPlayers();
    int newX = jogador1X, newY = jogador1Y;

    switch (direction) {
        case 'w': newY--; break;
        case 's': newY++; break;
        case 'a': newX--; break;
        case 'd': newX++; break;
    }

    if (currentMaze[newY][newX] == ' ') {
        jogador1X = newX;
        jogador1Y = newY;
    }

    printarJogadores();
}

void movePlayer2(char direction) {
    clearPlayers();
    int newX = jogador2X, newY = jogador2Y;

    switch (direction) {
        case 'i': newY--; break;
        case 'k': newY++; break;
        case 'j': newX--; break;
        case 'l': newX++; break;
    }

    if (currentMaze[newY][newX] == ' ') {
        jogador2X = newX;
        jogador2Y = newY;
    }

    printarJogadores();
}

int checkCollision() {
    return jogador1X == jogador2X && jogador1Y == jogador2Y;
}