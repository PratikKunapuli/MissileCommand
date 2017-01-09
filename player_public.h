// ============================================
// The header file is for module "player"
// This module is provdied in C style for new programmers in C
// Fall 2016 Gatech ECE 2035
//=============================================
/** @file city_landscape_public.h */
#ifndef PLAYER_PUBLIC_H
#define PLAYER_PUBLIC_H

#include "doubly_linked_list.h"

typedef enum {
    PMISSILE_EXPLODED = 0,
    PMISSILE_ACTIVE = 1
} PLAYER_MISSILE_STATUS; // is missile active or deactive?

typedef struct {
    int x;                   ///< The x-coordinate of missile current position
    int y;                   ///< The y-coordinate of missile current position
    PLAYER_MISSILE_STATUS status;   ///< The missile status, see MISSILE_STATUS
} PLAYER_MISSILE; // infomration about missile position and status

typedef enum {
    ALIVE = 1,
    DESTROYED = 0
} PLAYER_STATUS; // is player alive or destroyed?

typedef struct {
    int x; int y;       // x,y-coordinate of player - top left pixel
    int delta;     // delta x,y
    int width; int height;
    PLAYER_STATUS status;
    DLinkedList* playerMissiles;
} PLAYER; // structure for player


PLAYER player_get_info(void);
void player_init(void); // initialize the player's attributes
void player_moveLeft(void); // move delta pixels to the left 
void player_moveRight(void); // move delta pixels to the right
void player_fire(void); // fire missiles

void player_missile_draw(void); // updates the drawing of missiles on screen
void player_draw(int color);

//void player_missile_exploded(int i);
//void player_missile_exploded(PLAYER_MISSILE *playerMissile); //method overload

void player_destroy(void); // destroy the player to end game

#endif //PLAYER_PUBLIC_H