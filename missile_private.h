// ============================================
// The header file define the missile module
// Fall 2016 Gatech ECE2035
//=============================================
#ifndef MISSILE_PRIVATE_H
#define MISSILE_PRIVATE_H

#include "mbed.h"
#include "globals.h"
#include "missile_public.h"

//==== [private settings] ====
int MISSILE_INTERVAL = 10;
int MISSILE_SPEED = 6;
#define MISSILE_COLOR    0xFF0000

//==== [private type] ====

//==== [private function] ====
void missile_create(void);
void missile_update_position(void);
void missile_draw(MISSILE* missile, int color);

#endif //MISSILE_PRIVATE_H


