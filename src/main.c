#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "keyboard.h"
#include "screen.h"
#include "timer.h"

#define COMPRIMENTO_LABIRINTO (MAXX - 2)
#define LARGURA_LABIRINTO (MAXY - 2)

#define VITIMA 'V'
#define ASSASSINO 'A'
#define X1_INICIAL 10
#define Y1_INICIAL 10
#define X2_INICIAL 60
#define Y2_INICIAL 20

int vitimaX = X1_INICIAL;
int vitimaY = Y1_INICIAL;
int assassinoX = X2_INICIAL;
int assassinoY = Y2_INICIAL;

#define TEMPO_DE_JOGO 30000

struct rank {
    char nome[50];
    int vitorias;
};

void gerarLabirinto(char labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2]);
void imprimirJogadores();
void movimentarVitima(char direction, char labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2]);
void movimentarAssassino(char direction, char labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2]);
void atualizarRanking(char *vencedor, char *arquivo);
void imprimirRanking(char *arquivo);

int main() {
    keyboardInit();

    int menu = 1, running = 1;
    char key, labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2], vencedor[50];

    FILE *ranking;

    printf("██░░░░░░░▄█████▄░░░▄█████░░░██░░██░░░▄█████░░░██████▄░░░░░░░░░░░░██████░░░██████▄\n");
    printf("██░░░░░░░██░░░██░░░██░░░░░░░██░░██░░░██░░░░░░░██░░░██░░░░░░░░░░░░░░██░░░░░██░░░██\n");
    printf("██░░░░░░░██░░░██░░░██░░░░░░░█████░░░░█████░░░░██░░░██░░░░░░░░░░░░░░██░░░░░██░░░██\n");
    printf("██░░░░░░░██░░░██░░░██░░░░░░░██░░██░░░██░░░░░░░██░░░██░░░██████░░░░░██░░░░░██░░░██\n");
    printf("██████░░░▀█████▀░░░▀█████░░░██░░██░░░▀█████░░░██████▀░░░░░░░░░░░░██████░░░██░░░██\n");
    printf("A Vítima (WASD) acorda presa em um labirinto e o Assassino (IJKL) está em sua procura! A Vítima precisa fugir até o tempo acabar e o Assassino precisa pegá-la antes disso.\n");
    printf("Pressione espaço para começar.\n");

    while (menu) {
        if (keyhit()) {
            key = readch();

            if (key == 32) {
                srand(time(NULL));
                screenInit(1);
                gerarLabirinto(labirinto);
                imprimirJogadores();
                timerInit(TEMPO_DE_JOGO);
                
                while (running) {
                    screenSetColor(YELLOW, BLACK);
                    screenGotoxy(1, 1);

                    timerPrint();
                    screenUpdate();

                    if (timerTimeOver()) {
                        screenClear();
                        printf("A Vítma escapou! Tempo esgotado.\n");
                        printf("Digite seu nome:\n");

                        int ch;
                        while ((ch = getchar()) != '\n');

                        scanf("%s", vencedor);
                        atualizarRanking(vencedor, "rankingVitimas.txt");
                        
                        running = 0;
                        menu = 0;
                    }

                    if (keyhit()) {
                        key = readch();

                        if (key == 27) {
                            running = 0;
                            menu = 0;
                        } else {
                            if (key == 'w' || key == 'a' || key == 's' || key == 'd' || key == 'W' || key == 'A' || key == 'S' || key == 'D') {
                                movimentarVitima(key, labirinto);
                            }
                            
                            if (key == 'i' || key == 'j' || key == 'k' || key == 'l' || key == 'I' || key == 'J' || key == 'K' || key == 'L') {
                                movimentarAssassino(key, labirinto);
                            }
                            
                            if (vitimaX == assassinoX && vitimaY == assassinoY) {
                                screenClear();
                                printf("O Assassino venceu!");
                                printf("Digite seu nome:\n");
                                
                                int ch;
                                while ((ch = getchar()) != '\n');

                                scanf("%s", vencedor);
                                atualizarRanking(vencedor, "rankingAssassinos.txt");

                                running = 0;
                                menu = 0;
                            }
                        }
                    }
                }
            } else if (key == 27) {
                menu = 0;
            }
        }
    }

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    printf("\nRanking de Vítimas:\n");
    imprimirRanking("rankingVitimas.txt");

    printf("\nRanking de Assassinos:\n");
    imprimirRanking("rankingAssassinos.txt");

    return 0;
}

void gerarLabirinto(char labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2]) {
    for (int i = 0; i < COMPRIMENTO_LABIRINTO + 2; i++) {
        labirinto[0][i] = '-';
        labirinto[LARGURA_LABIRINTO + 1][i] = '-';
    }

    for (int i = 1; i <= LARGURA_LABIRINTO; i++) {
        labirinto[i][0] = '|';
        labirinto[i][COMPRIMENTO_LABIRINTO + 1] = '|';
    }

    for (int i = 1; i <= LARGURA_LABIRINTO; i++) {
        for (int j = 1; j <= COMPRIMENTO_LABIRINTO; j++) {
            if (rand() % 5 == 0) {
                labirinto[i][j] = '|';
            } else {
                labirinto[i][j] = ' ';
            }
        }
    }

    screenSetColor(WHITE, BLACK);

    for (int i = 0; i < LARGURA_LABIRINTO + 2; i++) {
        for (int j = 0; j < COMPRIMENTO_LABIRINTO + 2; j++) {
            screenGotoxy(j + 1, i + 1);
            printf("%c", labirinto[i][j]);
        }
    }

    screenUpdate();
}

void imprimirJogadores() {
    screenSetColor(BLUE, BLACK);
    screenGotoxy(vitimaX + 1, vitimaY + 1);
    printf("%c", VITIMA);

    screenSetColor(RED, BLACK);
    screenGotoxy(assassinoX + 1, assassinoY + 1);
    printf("%c", ASSASSINO);

    screenUpdate();
}

void movimentarVitima(char tecla, char labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2]) {
    int novoX = vitimaX, novoY = vitimaY;

    screenGotoxy(vitimaX + 1, vitimaY + 1);
    printf(" ");
    screenUpdate();

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
        vitimaX = novoX;
        vitimaY = novoY;
    }

    imprimirJogadores();
}

void movimentarAssassino(char tecla, char labirinto[LARGURA_LABIRINTO + 2][COMPRIMENTO_LABIRINTO + 2]) {
    int novoX = assassinoX, novoY = assassinoY;

    screenGotoxy(assassinoX + 1, assassinoY + 1);
    printf(" ");
    screenUpdate();

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
        assassinoX = novoX;
        assassinoY = novoY;
    }

    imprimirJogadores();
}

void atualizarRanking(char *vencedor, char *arquivo) {
    struct rank *jogadores = NULL;
    int cont = 0, encontrado = 0, capacidade = 1;

    FILE *ranking = fopen(arquivo, "r");
    jogadores = malloc(capacidade * sizeof(struct rank));

    if (ranking == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", arquivo);
        exit(1);
    } else {
        while (fscanf(ranking, "%s %d", jogadores[cont].nome, &jogadores[cont].vitorias) == 2) {
            if (strcmp(jogadores[cont].nome, vencedor) == 0) {
                jogadores[cont].vitorias++;
                encontrado = 1;
            }

            cont++;

            if (cont >= capacidade) {
                capacidade += 1;
                jogadores = realloc(jogadores, capacidade * sizeof(struct rank));
            }
        }

        fclose(ranking);
    }

    if (!encontrado) {
        strncpy(jogadores[cont].nome, vencedor, 50);
        jogadores[cont].vitorias = 1;

        ranking = fopen(arquivo, "a");

        if (ranking == NULL) {
            printf("Erro ao abrir o arquivo %s.\n", arquivo);
            exit(1);
        } else {
            fprintf(ranking, "%s %d\n", vencedor, 1);
            fclose(ranking);
        }
    } else {
        ranking = fopen(arquivo, "w");

        if (ranking == NULL) {
            printf("Erro ao abrir o arquivo %s.\n", arquivo);
            exit(1);
        } else {
            for (int i = 0; i < cont; i++) {
                fprintf(ranking, "%s %d\n", jogadores[i].nome, jogadores[i].vitorias);
            }

            fclose(ranking);
        }
    }

    free(jogadores);
}

void imprimirRanking(char *arquivo) {
    struct rank *jogadores = NULL;
    int cont = 0, capacidade = 1;

    FILE *ranking = fopen(arquivo, "r");
    jogadores = malloc(capacidade * sizeof(struct rank));
    
    if (ranking == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", arquivo);
        exit(1);
    } else {
        while (fscanf(ranking, "%s %d", jogadores[cont].nome, &jogadores[cont].vitorias) == 2) {
            cont++;

            if (cont >= capacidade) {
                capacidade += 1;
                jogadores = realloc(jogadores, capacidade * sizeof(struct rank));
            }
        }

        fclose(ranking);
    }

    for (int i = 0; i < cont - 1; i++) {
        for (int j = i + 1; j < cont; j++) {
            if (jogadores[i].vitorias < jogadores[j].vitorias) {
                struct rank temp = jogadores[i];
                jogadores[i] = jogadores[j];
                jogadores[j] = temp;
            }
        }
    }

    for (int i = 0; i < cont; i++) {
        printf("\t\t\t%s: %d vitória(s)\n", jogadores[i].nome, jogadores[i].vitorias);
    }

    free(jogadores);
}