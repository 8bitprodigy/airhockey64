#ifndef THINGS_H
#define THINGS_H

#include "airhockey.h"
/**********************
 * Thing parent class *
 **********************/

typedef enum {
    NONE,
    PUCK,
    PADDLE,
    CPU
} ThingKind;

typedef struct Thing{
    ThingKind kind;
    Vector2   position;
    Vector2   prev_pos;
    Vector2   velocity;
    float     radius;
    float     speed;
    void      *data;
    struct GameState *gamestate;
    void      (*update)(struct Thing *self, float _delta);
    void      (*hit)(   struct Thing *self, struct Thing *collider);
} Thing;

/* Paddle subclass */
typedef struct Paddle{
    struct Thing _;
    Color        color;
    int          controller_port;
    int          team;
    void         (*update)(struct Paddle *self, float _delta);
} Paddle;

Paddle *Paddle_new(Vector2 position, float radius, float speed, struct GameState *gamestate, Color color, uint8_t controller_port, uint8_t team);
void   Paddle_update(Paddle *self, float _delta);

Thing  *CPU_new(Vector2 position, float radius, float speed, struct GameState *gamestate, Color color, uint8_t controller_port, uint8_t team);
void   CPU_update(Thing *self, float _delta);

typedef enum {
    MOVING,
    DEAD,
} PuckState;

/* Puck subclass */
typedef struct Puck{
    struct Thing  _;
    float         min_speed;
    float         max_speed;
    struct Paddle *last_collider;
    void          (*update)(struct Puck *self, float _delta);
} Puck;

Puck *Puck_new(Vector2 position, float radius, float speed, float min_speed, float max_speed, struct GameState *gamestate);
void Puck_reset(Puck *self);
void Puck_update(Puck *self, float _delta);

#endif /* THINGS_H */
