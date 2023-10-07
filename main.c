#include <stdlib.h>
#include <stdio.h>
#include "gba.h"
#include "print.h"
#include "game.h"
#include "analogSound.h"

void initialize();
void goToStart();
void start();
void goToGame();
void game();
void goToPause();
void pause();
void goToWin();
void win();
void goToLose();
void lose();

char buffer[7];

enum {
    START,
    GAME,
    PAUSE,
    WIN,
    LOSE
};

//state
int state;
//frames for timing
int frames;

unsigned short buttons;
unsigned short oldButtons;

int main() {

    initialize();

    while (1) {
        //update buttons
        oldButtons = buttons;
        buttons = REG_BUTTONS;
        
        //state machine
        switch (state) {
            case START:
                start();
                break;
            case GAME:
                game();
                break;
            case PAUSE:
                pause();
                break;
            case WIN:
                win();
                break;
            case LOSE:
                lose();
                break;
        }

    }

}

void initialize() {
    mgba_open();

    REG_DISPCTL = MODE(3) | BG2_ENABLE;
    
    REG_SOUNDCNT_X = SND_ENABLED;
    REG_SOUNDCNT_L = DMG_VOL_LEFT(5) |
                DMG_VOL_RIGHT(5) |
                DMG_SND1_LEFT |
                DMG_SND1_RIGHT |
                DMG_SND2_LEFT |
                DMG_SND2_RIGHT |
                DMG_SND3_LEFT |
                DMG_SND3_RIGHT |
                DMG_SND4_LEFT |
                DMG_SND4_RIGHT;
    REG_SOUNDCNT_H = DMG_MASTER_VOL(2);
    
    buttons = REG_BUTTONS;
    oldButtons = 0;

    goToStart();
}

//Begin the game
void goToStart() {
    fillScreen(GRAY);
    drawString(84, 80, "Geo Shooter", BLUE);
    drawString(84, 95, "Press Start", GREEN);
    state = START;
}

//Wait for player to start the game
void start() {
    waitForVBlank();
    if (BUTTON_PRESSED(BUTTON_START)) {
        goToGame();
        initGame();
    }
}

//Begin the game
void goToGame() {
    fillScreen(BLUETWO);
    // mgba_printf("game");

    state = GAME;
}


//Run the game
void game() {
    //Updates all the game objects, mostly stored in game.c
    updateGame();

    //update the score
    sprintf(buffer, "%d", score);
    mgba_printf(score);
    waitForVBlank();

    //redraw the score
    drawRectangle(220, 145, 6, 8, BLUETWO);
    drawString(220, 145, buffer, HOTPINK);

    //draw the game objects now that they are updated
    drawGame();

    //check for pause
    if (BUTTON_PRESSED(BUTTON_START)) {
        goToPause();
    }
    
    //check for win and lose conditions
    if (score == -1) {
        goToLose();
    } else if (score == 7) {
        goToWin();
    }
}

//move to pause state
void goToPause() {
    fillScreen(BLUE);
    drawString(84, 80, "Game Paused", GREEN);
    waitForVBlank();
    state = PAUSE;
}

//game paused, will resume when start is clicked again
void pause() {
    waitForVBlank();
    if (BUTTON_PRESSED(BUTTON_START)) {
        goToGame();
    }
}

//called when score is 7(every enemy defeated), moves to win state
void goToWin() {
    fillScreen(BLUE);
    drawString(40, 80, "You Win!", RED);
    state = WIN;
}

void win() {
    //flashes player green, indicating win
    if (frames > 60) {
        frames = 0;
    }
    if (frames < 30) {
        player.color = WHITE;
    } else {
        player.color = GREEN;
    }
    frames++;
    drawRectangle(player.x, player.y, player.width, player.height, player.color);
}

//called when score drops below zero(when player collides with an enemy), move to lose state
void goToLose() {
    fillScreen(BLACK);
    drawString(40, 80, "You Lose!", RED);
    state = LOSE;
}

//flashes player red, indicating a loss
void lose() {
    if (frames > 60) {
        frames = 0;
    }
    if (frames < 30) {
        player.color = WHITE;
    } else {
        player.color = RED;
    }
    frames++;
    drawRectangle(player.x, player.y, player.width, player.height, player.color);
}