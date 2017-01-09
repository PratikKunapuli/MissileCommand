// ============================================
// The header file is for module "player
// This module is provdied in C style for new programmers in C
// Fall 2016 Gatech ECE 2035
//=============================================
#ifndef PLAYER_PRIVATE_H
#define PLAYER_PRIVATE_H

#include "mbed.h"
#include "globals.h"
#include "player_public.h"

//==== [private settings] ====
#define PLAYER_INIT_X 60
#define PLAYER_INIT_Y 100
#define PLAYER_DELTA 3 // used in design of player, pixels to move, euclidean distance
#define PLAYER_WIDTH 10 
#define PLAYER_HEIGHT 3
#define PLAYER_COLOR 0x0000FF //blue
#define PLAYER_MISSILE_SPEED 3
#define PLAYER_MISSILE_COLOR 0x0000FF //blue


//==== [private type] ====

void player_draw(int color);
void player_missile_draw(PLAYER_MISSILE* missile, int color);

//==== [private function] ====


#endif //PLAYER_PRIVATE_H

