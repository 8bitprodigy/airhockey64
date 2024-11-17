#ifndef AIRHOCKEY_H
#define AIRHOCKEY_H


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <libdragon.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>

#include "things.h"
#include "timer.h"

//#include "raylib_font_RGBA16_5551.h"
#define ATTR_NINTENDO64_WIDTH 320
#define ATTR_NINTENDO64_HEIGHT 240
extern bool flag;
extern bool xflag;

typedef struct
Area{
    Vector2 tl;
    Vector2 br;
} Area;

typedef struct
Playfield{
    Area    bounds;
    Area    team_bounds[JOYPAD_PORT_COUNT];
    Vector2 goals[JOYPAD_PORT_COUNT];
} Playfield;


typedef struct
GameState{
    struct Paddle *paddles[JOYPAD_PORT_COUNT];
    struct Puck   *pucks[JOYPAD_PORT_COUNT];
    bool           rumble[JOYPAD_PORT_COUNT];
    uint8_t        num_players;
    unsigned int   scores[JOYPAD_PORT_COUNT];
    uint8_t        max_score;
    uint8_t        num_teams;
    float          delta;
    Playfield     *playfield;
    Timer         *timers;
} GameState;

extern const Color Paddle_Colors[];
//extern struct GameState gamestate;
extern struct Puck      *puck;
extern struct Playfield playfield;

#endif /* AIRHOCKEY_H */
