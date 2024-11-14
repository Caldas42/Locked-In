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

void gerarLabirinto(char labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2]);
void printarLabirinto(char labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2]);
void printarJogadores();
void limparJogadores();
void movimentarJogador1(char direction, char labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2]);
void movimentarJogador2(char direction, char labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2]);
int assassinato();

int main() {
    keyboardInit();

    int menu = 1, running = 1;
    char key, labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2], *nome;

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
                        printf("Digite seu nome:\n");

                        ranking = fopen("ranking.txt", "a");

                        if(ranking == NULL){
                            printf("Erro no ranking\n");
                            exit(1);
                        }

                        nome = 

                        printf("Digite o nome: ");
                        fgets(nome, sizeof(nome), stdin);

                        // Remove o caractere de nova linha que fgets armazena, se presente
                        nome[strcspn(nome, "\n")] = 0;

                        // Escreve o nome e o número no arquivo
                        fprintf(ranking, "%s\n", nome);

                        fclose(ranking);

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
                            } else if(key == 'i' || key == 'j' || key == 'k' || key == 'l' || key == 'I' || key == 'J' || key == 'K' || key == 'L') {
                                movimentarJogador2(key, labirinto);
                            }
                            
                            if(assassinato()) {
                                screenClear();
                                printf("O Assassino venceu!");
                                printf("Digite seu nome:\n");

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