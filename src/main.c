/**
 * main.h
 * Created on Oct, 29th 2024
 * Author: Célio Dias, João Danilo Jota e Pedro Caldas
*/


#include <string.h>

#include "screen.h"
#include "keyboard.h"
#include "timer.h"

int main() {
    screenInit(1);
    keyboardInit();
    timerInit(50);

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    return 0;
}