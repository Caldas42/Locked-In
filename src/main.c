#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "keyboard.h"
#include "screen.h"
#include "timer.h"

#define PLAYER '@'
#define INITIAL_X 40
#define INITIAL_Y 12

#define MAZE_WIDTH (MAXX - 2)
#define MAZE_HEIGHT (MAXY - 2)

char maze[MAZE_HEIGHT][MAZE_WIDTH + 1];
char maze2[MAZE_HEIGHT][MAZE_WIDTH + 1];

// Definindo a posição do personagem
int playerX = INITIAL_X;
int playerY = INITIAL_Y;

// Ponteiro para o labirinto atual
char (*currentMaze)[MAZE_WIDTH + 1];

void drawMaze(char maze[MAZE_HEIGHT][MAZE_WIDTH + 1]);
void generateMaze(char maze[MAZE_HEIGHT][MAZE_WIDTH + 1]);
void movePlayer(char direction);
void drawPlayer();
void clearPlayer();

void generateMaze(char maze[MAZE_HEIGHT][MAZE_WIDTH + 1]) {
    // Gerar um labirinto aleatório simples
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            // Define paredes e caminhos
            if (rand() % 5 == 0) {
                maze[y][x] = '#'; // Paredes
            } else {
                maze[y][x] = ' '; // Caminhos
            }
        }
    }

    // Adicionando o símbolo '%' como a porta para o segundo labirinto
    maze[MAZE_HEIGHT / 2][MAZE_WIDTH / 2] = '%'; // Colocando a porta no meio
    maze[MAZE_HEIGHT][MAZE_WIDTH] = '\0'; // Finalizando a string
}

void drawMaze(char maze[MAZE_HEIGHT][MAZE_WIDTH + 1]) {
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            screenGotoxy(x + 1, y + 1); // Ajustando para não "comer" as bordas
            printf("%c", maze[y][x]);
        }
    }
    screenUpdate();
}

int main() {
    // Inicializando componentes
    keyboardInit();
    screenInit(1);
    timerInit(100);
    
    // Gerando o labirinto
    generateMaze(maze);
    drawMaze(maze);
    drawPlayer();
    
    // Define o labirinto inicial
    currentMaze = maze;

    char key;
    int running = 1;

    // Loop principal
    while (running) {
        if (keyhit()) {
            key = readch();
            if (key == 'q') {  // 'q' para sair do jogo
                running = 0;
            } else {
                movePlayer(key);
                // Verifica se o jogador atingiu o símbolo '%'
                if (currentMaze[playerY - 1][playerX - 1] == '%') {
                    // Muda para o segundo labirinto
                    printf("Você entrou em um novo labirinto!\n");
                    generateMaze(maze2); // Gera um novo labirinto
                    drawMaze(maze2); // Desenha o novo labirinto
                    currentMaze = maze2; // Aponta para o novo labirinto
                    playerX = INITIAL_X; // Reinicia a posição do jogador
                    playerY = INITIAL_Y; // Reinicia a posição do jogador
                    drawPlayer(); // Desenha o jogador na nova posição
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

void drawPlayer() {
    screenGotoxy(playerX, playerY);
    printf("%c", PLAYER);
    screenUpdate();
}

void clearPlayer() {
    screenGotoxy(playerX, playerY);
    printf(" ");
    screenUpdate();
}

void movePlayer(char direction) {
    clearPlayer();
    int newX = playerX;
    int newY = playerY;

    switch (direction) {
        case 'w': newY--; break; // Para cima
        case 's': newY++; break; // Para baixo
        case 'a': newX--; break; // Para esquerda
        case 'd': newX++; break; // Para direita
    }

    // Ajuste para a verificação de colisão no labirinto atual
    if (newX >= 1 && newX <= MAZE_WIDTH && newY >= 1 && newY <= MAZE_HEIGHT) {
        if (currentMaze[newY - 1][newX - 1] != '#') {
            // Movimento válido
            playerX = newX;
            playerY = newY;
        }
    }

    drawPlayer();
}