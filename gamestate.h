#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "airhockey.h"
#include "things.h"

typedef struct {
    float x1;
    float y1;
    float x2;
    float y2;
} Area;

typedef struct {
    Area    bounds;
    Area    team_bounds[JOYPAD_PORT_COUNT];
    Vector2 goals[JOYPAD_PORT_COUNT];
} Playfield;

typedef struct{
    Paddle    paddles[JOYPAD_PORT_COUNT];
    bool      rumble[JOYPAD_PORT_COUNT];
    uint8_t   num_players;
    uint8_t   num_teams;
    float     delta;
    Playfield playfield;
} GameState;

#endif /* GAMESTATE_H */
