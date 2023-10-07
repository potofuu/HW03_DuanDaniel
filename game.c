#include <stdlib.h>
#include "game.h"
#include "gba.h"
#include "analogSound.h"

//player
PLAYER player;
//bullet shot by player
BULLET bullet;
//big boss
BOSS boss;
ENEMIES enemies[ENEMYCOUNT];
SPEED speedUp;
ZONE zone;
int score;
int frames;

//run all the initalizations for game objects
//init extra adds speedup powerup and special zone where player cannot shoot
void initGame() {
    score = 0;

    initPlayer();
    initBullets();
    initBoss();
    initEnemies();
    initExtra();
    initZone();
}

//initialize the player at bottom center
//starts with velocity of 2, power up can make faster/slower
void initPlayer() {
    player.y = 143;
    player.x = 115;
    player.oldy = player.y;
    player.oldx = player.x;
    player.velocity = 2;
    player.height = 15;
    player.width = 15;
    player.color = GREEN;
}

//initialize the bullet parameters, start with it inactive until player shoots it
void initBullets() {
    bullet.active = 0;
    bullet.color = YELLOW;
    bullet.velocity = 2;
}

//initialize the boss
void initBoss() {
    boss.active = 1;
    boss.color = BLUE;
    boss.x = 0;
    boss.y = 20;
    boss.oldx = boss.x;
    boss.oldy = boss.y;
    boss.velocity = 4;
    boss.veloY = 2;
    boss.lives = 3;
}

//initialize enemies
void initEnemies() {
    frames = 0;
    //iterate through the array of enemies, initializing each one of them
    for (int i = 0; i < ENEMYCOUNT; i++) {
        enemies[i].y = 40 * (i % 2) + 40;
        enemies[i].active = 1;
        enemies[i].x = 10 + i * 40;
        if (i % 2 == 0) {
            enemies[i].velocity = 3;
        } else {
            enemies[i].velocity = -3;
        }
        int colorPicker = rand() % 3;
        switch (colorPicker) {
            case 0:
                enemies[i].color = RED;
                break;
            case 1:
                enemies[i].color = MAGENTA;
                break;
            case 2:
                enemies[i].color = BROWN;
                break;
        }
    }
}

//initialize the power up, start it as speedup mode on the left
void initExtra() {
    speedUp.x = 15;
    speedUp.y = 140;
    speedUp.color = GREEN;
    speedUp.change = 0;
    speedUp.speedup = 2;
}

//initialize the no shoot zone
void initZone() {
    zone.color = BLACK;
    zone.x = 0;
    zone.oldx = zone.x;
}

//update the game, incrementing frames and calling all the update functions
void updateGame() {
    frames++;
    updatePlayer();
    updateBullets();
    updateBoss();
    //iterate through the enemies, changing their movement every once in a while
    for (int i = 0; i < ENEMYCOUNT; i++) {
        if (frames <= 45) {
            if (i % 2 == 0) {
                enemies[i].velocity = 2;
            } else {
                enemies[i].velocity = -2;
            }
        } else {
            if (i % 2 == 0) {
                enemies[i].velocity = -2;
            } else {
                enemies[i].velocity = 2;
            }   
        }
        //update each enemey individually as it moves thorugh the array
        updateEnemies(&enemies[i]);
    }

    updateExtra();

    updateZone();

    //reset frames when needed
    if (frames >= 90) {
        frames = 0;
    }

}

//player update function, checks for movement inputs as well as bounds
void updatePlayer() {
    if (BUTTON_HELD(BUTTON_LEFT) && (player.x - 1 > 0)) {
        player.oldx = player.x;
        player.x -= player.velocity;
    }
    if (BUTTON_HELD(BUTTON_RIGHT) && (player.x + player.width < SCREENWIDTH - 10)) {
        player.oldx = player.x;
        player.x += player.velocity;
    } 
    if (BUTTON_HELD(BUTTON_UP) && (player.y > 0)) {
        player.oldy = player.y;
        player.y -= player.velocity;
    }
    if (BUTTON_HELD(BUTTON_DOWN) && (player.y < 145)) {
        player.oldy = player.y;
        player.y += player.velocity;
    }
    //check for collision with the boss
    if (collision(boss.x, boss.y, 10, 10, player.x, player.y, 10, 10)) {
        score = -1;
    }
}

//update the bullet fired from the player
void updateBullets() {
    // Fires the bullet, but only if there is no bullet currently active, and the player is not in the zone
    if (BUTTON_PRESSED(BUTTON_A) && !bullet.active && !collision(player.x, player.y, 10, 10, zone.x, 130, 80, 30)) {
        bullet.active = 1;
        bullet.x = player.x;
        bullet.y = player.y + 3;
        REG_SND2CNT = DMG_ENV_VOL(4) | DMG_STEP_TIME(2);
        REG_SND2FREQ = NOTE_C4 | SND_RESET;
    }
    // If the bullet is active, move it up
    if (bullet.active) {
        bullet.y -= bullet.velocity;
        if (bullet.y + 15 < 0) {
            bullet.active = 0;
        }
    }
    //check if the bullet hits the boss
    if (collision(bullet.x, bullet.y, 5, 10, boss.x, boss.y, 20, 10) && boss.active) {
        drawRectangle(bullet.x + 5, bullet.y + 2, 5, 10, BLUETWO);
        bullet.y = 160;
        bullet.active = 0;
        boss.lives--;
    }
}

//updates an enemy
void updateEnemies(ENEMIES* enemy) {
    //only runs updates if the enemy is active(alive)
    if (enemy->active) {
        enemy->oldx = enemy->x;
        enemy->oldy = enemy->y;
        enemy->y += enemy->velocity;
        //checking for bullet, if so, deactivate enemy and add to score
        if (collision(enemy->x - 2, enemy->y, 10, 10, bullet.x, bullet.y, 5, 10) && enemy->active) {
            drawRectangle(enemy->x - 1, enemy->y - 2, 15, 20, BLUETWO);
            enemy->active = 0;
            score++;
            REG_SND2CNT = DMG_ENV_VOL(4) | DMG_STEP_TIME(2);
            REG_SND2FREQ = NOTE_G6 | SND_RESET;
        }
        //check if the player runs into an enemy, and if so, kill the player
        if (collision(enemy->x, enemy->y, 10, 10, player.x, player.y, 10, 10)) {
            score = -1;
        }
    }
}

//update the boss
void updateBoss() {
    //boss has multiple lives, so only deactivate and give score when it dies
    if (boss.lives == 0) {
        boss.active = 0;
        drawRectangle(boss.x, boss.y, 20, 15, BLUETWO);
        score++;
    }

    if (boss.y < 0) {
        boss.veloY = 2;
    } 
    if (boss.y > 40) {
        boss.veloY = -2;
    }

    if (boss.x > 215) {
        boss.velocity = -3;
    } else if (boss.x < 0) {
        boss.velocity = 3;
    }
        
    boss.oldx = boss.x;
    boss.oldy = boss.y;
    boss.x += boss.velocity;
    boss.y += boss.veloY;
}

//update the power up, green on left side speeds up player, red on right side slows player down
void updateExtra() {
    if (collision(player.x, player.y, 10, 10, speedUp.x, speedUp.y, 5, 5)) {
        if (!speedUp.change) {
            //augment player speed
            player.velocity += speedUp.speedup;
            //visually indicate change in powerup
            speedUp.color = RED;
            //change to slow down
            speedUp.change = 1;
            //move to other side
            speedUp.x = 215;
        } else {
            player.velocity -= speedUp.speedup;
            speedUp.color = GREEN;
            speedUp.change = 0;
            speedUp.x = 15;
        }
    }
}

//move the zone to block powerup
void updateZone() {
    zone.oldx = zone.x;
    if (frames < 45) {
        zone.x = 0;
    } else {
        zone.x = 80;
    }
}

//calls the draw functions for all the game objects
void drawGame() {
    drawZone();
    drawExtra();
    drawPlayer();
    drawBullet();
    drawBoss();
    //iterate through entire enemy array
    for (int i = 0; i < ENEMYCOUNT; i++) {
        drawEnemies(&enemies[i]);
    }
}

//draws the player
void drawPlayer() {
    drawRectangle(player.oldx - 5, player.oldy - 5, 25, 25, BLUETWO);
    drawRectangle(player.x, player.y, player.width, player.height, player.color);
}

//draws the bullet(if it is active/fired)
void drawBullet() {
    if (bullet.active) {
        drawRectangle(bullet.x + 5, bullet.y + 2, 5, 10, BLUETWO);
        drawRectangle(bullet.x + 5, bullet.y, 5, 10, YELLOW);
    }
}

//draw the boss shape
void drawBoss() {
    if (boss.active) {
        drawRectangle(boss.oldx, boss.oldy, 20, 15, BLUETWO);
        drawRectangle(boss.x, boss.y, 20, 10, BLUE);
    }
}

//draw the enemies, given one at a time
void drawEnemies(ENEMIES* enemy) {
    if (enemy->active) {
        drawRectangle(enemy->oldx, enemy->oldy, 10, 10, BLUETWO);
        drawRectangle(enemy->x, enemy->y, 10, 10, enemy->color);
    }
}

//draw the powerup
void drawExtra() {
    drawRectangle(speedUp.x, speedUp.y, 5, 5, speedUp.color);
}

//draws the zone for no firing
void drawZone() {
    drawRectangle(zone.oldx, 130, 80, 30, BLUETWO);
    drawRectangle(zone.x, 130, 80, 30, zone.color);
}