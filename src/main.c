#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "keyboard.h"
#include "screen.h"
#include "timer.h"

#define PLAYER1 '@'  // Jogador 1 (ijkl)
#define PLAYER2 'O'  // Jogador 2 (WASD)
#define INITIAL_X1 10
#define INITIAL_Y1 10
#define INITIAL_X2 60
#define INITIAL_Y2 20

#define MAZE_WIDTH (MAXX - 2)
#define MAZE_HEIGHT (MAXY - 2)

char maze[MAZE_HEIGHT + 2][MAZE_WIDTH + 2]; // Labirinto atual
char maze2[MAZE_HEIGHT + 2][MAZE_WIDTH + 2]; // Segundo labirinto

// Definindo as posições dos jogadores
int player1X = INITIAL_X1;
int player1Y = INITIAL_Y1;
int player2X = INITIAL_X2; // Jogador 2 começa em uma posição diferente
int player2Y = INITIAL_Y2;

// Ponteiro para o labirinto atual
char (*currentMaze)[MAZE_WIDTH + 2];

void movePlayer1(char direction);
void movePlayer2(char direction);
void drawPlayers();
void clearPlayers();
void generateMaze(char maze[MAZE_HEIGHT + 2][MAZE_WIDTH + 2]);
void drawMaze(char maze[MAZE_HEIGHT + 2][MAZE_WIDTH + 2]);
int checkCollision();

int main() {
    // Inicializando componentes
    keyboardInit();
    char chave;
    int running = 1;

    while (running) {
        if (keyhit()) {
            chave = readch();
            if (chave == 32) {
                running = 0;
            } else {
                printf("Bem-vindo ao jogo! Um jogador controla com WASD e o outro com IJKL.\n");
                printf("Para começar, pressione ESPAÇO.");
            }
        }
    }

    screenInit(1);
    timerInit(100); // Inicializa o timer com um intervalo de 100ms
    srand(time(NULL)); // Inicializa o gerador de números aleatórios

    // Gerando o labirinto
    generateMaze(maze);
    drawMaze(maze);
    drawPlayers();

    // Define o labirinto inicial
    currentMaze = maze;

    char key;
    running = 1;

    // Loop principal
    while (running) {
        if (keyhit()) {
            key = readch();
            if (key == 27) {  // ESC para sair do jogo
                running = 0;
            } else {
                // Verifica se a tecla pressionada é uma das teclas de controle
                if (key == 'w' || key == 'a' || key == 's' || key == 'd') {
                    movePlayer2(key); // Mover jogador 2
                } else if (key == 'i' || key == 'j' || key == 'k' || key == 'l') {
                    movePlayer1(key); // Mover jogador 1
                }
                
                // Verifica colisão entre os jogadores
                if (checkCollision()) {
                    // Muda para o segundo labirinto
                    generateMaze(maze2); // Gera um novo labirinto
                    drawMaze(maze2); // Desenha o novo labirinto
                    currentMaze = maze2; // Aponta para o novo labirinto
                    player1X = INITIAL_X1; // Reinicia a posição do jogador 1
                    player1Y = INITIAL_Y1;
                    player2X = INITIAL_X2; // Reinicia a posição do jogador 2
                    player2Y = INITIAL_Y2;
                    drawPlayers(); // Desenha os jogadores na nova posição
                }
            }
        }
    }

    // Finalizando o jogo
    keyboardDestroy();
    screenDestroy();
    timerDestroy();
    return 0;
}

void drawPlayers() {
    // Desenha o assassino em vermelho (PLAYER2) e a presa em branco (PLAYER1)
    screenSetColor(RED, BLACK);  // Assassino
    screenGotoxy(player2X + 1, player2Y + 1); // Ajusta para o formato de tela
    printf("%c", PLAYER2);

    screenSetColor(WHITE, DARKGRAY);  // Presa
    screenGotoxy(player1X + 1, player1Y + 1); // Ajusta para o formato de tela
    printf("%c", PLAYER1);

    screenUpdate();
}

void clearPlayers() {
    screenGotoxy(player1X + 1, player1Y + 1); // Ajusta para o formato de tela
    printf(" ");
    screenGotoxy(player2X + 1, player2Y + 1); // Ajusta para o formato de tela
    printf(" ");
    screenUpdate();
}

void movePlayer1(char direction) {
    clearPlayers();
    int newX = player1X;
    int newY = player1Y;

    switch (direction) {
        case 'i': newY--; break; // Para cima (I)
        case 'k': newY++; break; // Para baixo (K)
        case 'j': newX--; break; // Para esquerda (J)
        case 'l': newX++; break; // Para direita (L)
    }

    // Verifica limites do labirinto
    if (newX >= 1 && newX <= MAZE_WIDTH && newY >= 1 && newY <= MAZE_HEIGHT) {
        if (currentMaze[newY][newX] != '|') { // Verifica se não está colidindo com a parede
            player1X = newX;
            player1Y = newY;
        }
    }

    drawPlayers();
}

void movePlayer2(char direction) {
    clearPlayers();
    int newX = player2X;
    int newY = player2Y;

    switch (direction) {
        case 'w': newY--; break; // Para cima (W)
        case 's': newY++; break; // Para baixo (S)
        case 'a': newX--; break; // Para esquerda (A)
        case 'd': newX++; break; // Para direita (D)
    }

    // Verifica limites do labirinto
    if (newX >= 1 && newX <= MAZE_WIDTH && newY >= 1 && newY <= MAZE_HEIGHT) {
        if (currentMaze[newY][newX] != '|') { // Verifica se não está colidindo com a parede
            player2X = newX;
            player2Y = newY;
        }
    }

    drawPlayers();
}

void generateMaze(char maze[MAZE_HEIGHT + 2][MAZE_WIDTH + 2]) {
    // Preenche as bordas com paredes
    for (int x = 0; x < MAZE_WIDTH + 2; x++) {
        maze[0][x] = '-'; // Bordas superiores
        maze[MAZE_HEIGHT + 1][x] = '-'; // Bordas inferiores
    }
    for (int y = 1; y <= MAZE_HEIGHT; y++) {
        maze[y][0] = '|'; // Bordas esquerdas
        maze[y][MAZE_WIDTH + 1] = '|'; // Bordas direitas
    }

    // Gerar um labirinto aleatório simples
    for (int y = 1; y <= MAZE_HEIGHT; y++) {
        for (int x = 1; x <= MAZE_WIDTH; x++) {
            // Define paredes e caminhos
            if (rand() % 5 == 0) {
                maze[y][x] = '|'; // Paredes verticais
            } else {
                maze[y][x] = ' '; // Caminhos
            }
        }
    }

    maze[MAZE_HEIGHT + 1][MAZE_WIDTH + 1] = '\0'; // Finalizando a string
}

void drawMaze(char maze[MAZE_HEIGHT + 2][MAZE_WIDTH + 2]) {
    for (int y = 0; y < MAZE_HEIGHT + 2; y++) {
        for (int x = 0; x < MAZE_WIDTH + 2; x++) {
            screenSetColor(YELLOW, BLACK);
            screenGotoxy(x + 1, y + 1); // Ajustando para não "comer" as bordas
            printf("%c", maze[y][x]);
        }
    }
    screenUpdate();
}

int checkCollision() {
    return (player1X == player2X && player1Y == player2Y);
}
