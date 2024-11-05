/**
 * main.h
 * Created on Oct, 29th 2024
 * Author: Célio Dias, João Danilo Jota e Pedro Caldas
*/

#include <stdio.h>
#include <stdlib.h>
#include "keyboard.h"
#include "screen.h"
#include "timer.h"

// Definição do personagem
#define PLAYER '@'
#define INITIAL_X 40
#define INITIAL_Y 12

// Definindo a posição do personagem
int playerX = INITIAL_X;
int playerY = INITIAL_Y;

void drawPlayer();
void clearPlayer();
void movePlayer(char direction);

int main() {
    // Inicializando componentes
    keyboardInit();
    screenInit(1);
    timerInit(100);

    // Desenhando o personagem inicial
    drawPlayer();

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
    switch (direction) {
        case 'w': if (playerY > MINY + 1) playerY--; break;
        case 's': if (playerY < MAXY - 1) playerY++; break;
        case 'a': if (playerX > MINX + 1) playerX--; break;
        case 'd': if (playerX < MAXX - 2) playerX++; break;
    }
    drawPlayer();
}