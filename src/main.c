#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "keyboard.h"
#include "screen.h"
#include "timer.h"

#define COMPRIMENTO_LABIRINTO (MAXX - 2)
#define LARGURA_LABIRINTO (MAXY - 2)

#define JOGADOR1 'V'
#define JOGADOR2 'A'
#define X1_INICIAL 10
#define Y1_INICIAL 10
#define X2_INICIAL 60
#define Y2_INICIAL 20

int jogador1X = X1_INICIAL;
int jogador1Y = Y1_INICIAL;
int jogador2X = X2_INICIAL;
int jogador2Y = Y2_INICIAL;

#define TEMPO 60000

#define JOGADORES 100

struct rank {
    char nome[50];
    int vitorias;
};

void gerarLabirinto(char labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2]);
void printarLabirinto(char labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2]);
void printarJogadores();
void limparJogadores();
void movimentarJogador1(char direction, char labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2]);
void movimentarJogador2(char direction, char labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2]);
int assassinato();

void updatePlayerRecord(char *vencedor, char *filename);
void loadPlayerRecords(const char *filename, struct rank players[], int *playerCount);
void sortPlayersByWins(struct rank players[], int playerCount);
void printPlayerRecords(struct rank players[], int playerCount);

int main() {
    keyboardInit();

    int menu = 1, running = 1;
    char key, labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2], vencedor[50];

    srand(time(NULL));
    gerarLabirinto(labirinto);

    FILE *ranking;

    printf("██░░░░░░░▄█████▄░░░▄█████░░░██░░██░░░▄█████░░░██████▄░░░░░░░░░░░░██████░░░██████▄\n");
    printf("██░░░░░░░██░░░██░░░██░░░░░░░██░░██░░░██░░░░░░░██░░░██░░░░░░░░░░░░░░██░░░░░██░░░██\n");
    printf("██░░░░░░░██░░░██░░░██░░░░░░░█████░░░░█████░░░░██░░░██░░░░░░░░░░░░░░██░░░░░██░░░██\n");
    printf("██░░░░░░░██░░░██░░░██░░░░░░░██░░██░░░██░░░░░░░██░░░██░░░██████░░░░░██░░░░░██░░░██\n");
    printf("██████░░░▀█████▀░░░▀█████░░░██░░██░░░▀█████░░░██████▀░░░░░░░░░░░░██████░░░██░░░██\n");
    printf("A Vítima (WASD) acorda presa em um labirinto e o Assassino (IJKL) está em sua procura! A Vítima precisa fugir até o tempo acabar e o Assassino precisa pegá-la antes disso.\n");
    printf("Pressione espaço para começar.\n");

    while(menu) {
        if(keyhit()) {
            key = readch();

            if(key == 32) {
                screenInit(1);
                printarLabirinto(labirinto);
                printarJogadores();
                timerInit(TEMPO);
                
                while(running) {
                    if(timerTimeOver()) {
                        screenClear();
                        printf("A Vítma escapou! Tempo esgotado.\n");

                        

                        
                        

                        running = 0;
                        menu = 0;
                    }

                    if(keyhit()) {
                        key = readch();

                        if(key == 27) {
                            running = 0;
                            menu = 0;
                        } else {
                            if(key == 'w' || key == 'a' || key == 's' || key == 'd' || key == 'W' || key == 'A' || key == 'S' || key == 'D') {
                                movimentarJogador1(key, labirinto);
                            }
                            
                            if(key == 'i' || key == 'j' || key == 'k' || key == 'l' || key == 'I' || key == 'J' || key == 'K' || key == 'L') {
                                movimentarJogador2(key, labirinto);
                            }
                            
                            if(assassinato()) {
                                screenClear();
                                printf("O Assassino venceu!");
                                printf("Digite seu nome:\n");
                                
                                fgets(vencedor, 50, stdin);
                                vencedor[strcspn(vencedor, "\n")] = '\0';
                                updatePlayerRecord(vencedor, "rankingAssassino.txt");

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
    timerDestroy();

    // Exibir o ranking dos vencedores
    struct rank players[JOGADORES];
    int playerCount = 0;
    loadPlayerRecords("rankingAssassino.txt", players, &playerCount);
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

void limparJogadores() {
    screenGotoxy(jogador1X + 1, jogador1Y + 1);
    printf(" ");

    screenGotoxy(jogador2X + 1, jogador2Y + 1);
    printf(" ");

    screenUpdate();
}

void movimentarJogador1(char tecla, char labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2]) {
    limparJogadores();
    int novoX = jogador1X, novoY = jogador1Y;

    switch(tecla) {
        case 'w': 
            novoY--;
            break;
        case 'a': 
            novoX--;
            break;
        case 's': 
            novoY++;
            break;
        case 'd': 
            novoX++;
            break;
        case 'W': 
            novoY--;
            break;
        case 'A': 
            novoX--;
            break;
        case 'S': 
            novoY++;
            break;
        case 'D': 
            novoX++;
            break;
    }

    if (labirinto[novoY][novoX] == ' ') {
        jogador1X = novoX;
        jogador1Y = novoY;
    }

    printarJogadores();
}

void movimentarJogador2(char tecla, char labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2]) {
    limparJogadores();
    int novoX = jogador2X, novoY = jogador2Y;

    switch(tecla) {
        case 'i': 
            novoY--;
            break;
        case 'j': 
            novoX--;
            break;
        case 'k': 
            novoY++;
            break;
        case 'l': 
            novoX++;
            break;
        case 'I': 
            novoY--;
            break;
        case 'J': 
            novoX--;
            break;
        case 'K': 
            novoY++;
            break;
        case 'L': 
            novoX++;
            break;
    }

    if (labirinto[novoY][novoX] == ' ') {
        jogador2X = novoX;
        jogador2Y = novoY;
    }

    printarJogadores();
}

int assassinato() {
    return jogador1X == jogador2X && jogador1Y == jogador2Y;
}

void updatePlayerRecord(char *vencedor, char *file) {
    struct rank jogadores[JOGADORES];
    int cont = 0;
    int found = 0;

    FILE *ranking = fopen(file, "r");

    if (ranking == NULL) {
       exit(1);
    } else {
        while (fscanf(ranking, "%s %d", jogadores[cont].nome, &jogadores[cont].vitorias) == 2) {
            if (strcmp(jogadores[cont].nome, vencedor) == 0) {
                jogadores[cont].vitorias++;
                found = 1;
            }
            cont++;
            if (cont >= JOGADORES) break;
        }

        fclose(ranking);
    }

    // Se o vencedor não foi encontrado, adiciona-o ao final da lista
    if (!found && cont < JOGADORES) {
        strncpy(jogadores[cont].nome, vencedor, 50);
        jogadores[cont].vitorias = 1;
        cont++;
    }

    // Abre o arquivo em modo de adição e grava apenas o novo vencedor
    if (!found) {
        ranking = fopen(file, "a");
        if (ranking) {
            fprintf(ranking, "%s %d\n", vencedor, 1);
            fclose(ranking);
        } else {
            printf("Erro ao abrir o arquivo %s para gravação.\n", file);
        }
    } else {
        // Sobrescreve o arquivo somente quando o vencedor já existe
        ranking = fopen(file, "w");
        if (ranking) {
            for (int i = 0; i < cont; i++) {
                fprintf(ranking, "%s %d\n", jogadores[i].nome, jogadores[i].vitorias);
            }
            fclose(ranking);
        } else {
            printf("Erro ao abrir o arquivo %s para gravação.\n", file);
        }
    }
}

void loadPlayerRecords(const char *filename, struct rank players[], int *playerCount) {
    *playerCount = 0;
    FILE *file = fopen(filename, "r");
    
    if (file) {
        while (fscanf(file, "%s %d", players[*playerCount].nome, &players[*playerCount].vitorias) == 2) {
            (*playerCount)++;
            if (*playerCount >= JOGADORES) break;
        }
        fclose(file);
    } else {
        printf("Erro ao abrir o arquivo %s.\n", filename);
    }
}
void sortPlayersByWins(struct rank players[], int playerCount) {
    for (int i = 0; i < playerCount - 1; i++) {
        for (int j = i + 1; j < playerCount; j++) {
            if (players[i].vitorias < players[j].vitorias) {
                struct rank temp = players[i];
                players[i] = players[j];
                players[j] = temp;
            }
        }
    }
}
void printPlayerRecords(struct rank players[], int playerCount) {
    printf("\n\t\t\tRanking de Vencedores:\n");
    
    for (int i = 0; i < playerCount; i++) {
        printf("\t\t\t%s: %d vitórias\n", players[i].nome, players[i].vitorias);
    }
}