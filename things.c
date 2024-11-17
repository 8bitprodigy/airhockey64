#include "things.h"
#include "collision.h"

bool
is_in_goal(Puck *self, Vector2 new_pos, uint8_t goal){
    GameState *gamestate = self->_.gamestate;
    Playfield *playfield = gamestate->playfield;
    Vector2 g = playfield->goals[goal];
    g = (Vector2){g.x, g.y - (self->_.radius*2.0f)};
    float top = g.x + (g.y/2.0f);
    float bot = g.x - (g.y/2.0f);
    return (bot < self->_.position.y) && (self->_.position.y < top);
}

/********************
 * Paddle functions *
 ********************/
Paddle
*Paddle_new(Vector2 position, float radius, float speed, struct GameState *gamestate, Color color, uint8_t controller_port, uint8_t team){
    Paddle *paddle = malloc(sizeof(Paddle));

    paddle->_ = (Thing){
        .kind      = PADDLE,
        .position  = position,
        .velocity  = Vector2Zero(),
        .radius    = radius,
        .speed     = speed,
        .gamestate = gamestate,
        .data      = paddle,
    };

    paddle->color           = color;
    paddle->controller_port = controller_port;
    paddle->team            = team;
    paddle->update          = &Paddle_update;

    return paddle;
}

void
Paddle_update(Paddle *self, float _delta) {
    Vector2 stick;
    Vector2 new_pos;

    Thing *thing = &self->_;

    /* Minkowski bounds */
    float mink_x1 = playfield.team_bounds[self->team].tl.x + thing->radius;
    float mink_y1 = playfield.team_bounds[self->team].tl.y + thing->radius;
    float mink_x2 = playfield.team_bounds[self->team].br.x - thing->radius;
    float mink_y2 = playfield.team_bounds[self->team].br.y - thing->radius;

    joypad_buttons_t pressed = joypad_get_buttons_pressed(self->controller_port);
    joypad_buttons_t held    = joypad_get_buttons_held(   self->controller_port);
    joypad_inputs_t  inputs  = joypad_get_inputs(         self->controller_port);

    stick = (Vector2){(float)inputs.stick_x/128.0f, -(float)inputs.stick_y/128.0f};
    thing->velocity = Vector2Scale(stick, thing->speed);

    new_pos = Vector2Add(thing->position, Vector2Scale(thing->velocity,_delta));

    if (new_pos.x < mink_x1)   new_pos.x = mink_x1;
    if (mink_x2   < new_pos.x) new_pos.x = mink_x2;
    if (new_pos.y < mink_y1)   new_pos.y = mink_y1;
    if (mink_y2   < new_pos.y) new_pos.y = mink_y2;

    thing->prev_pos = thing->position;
    thing->position = new_pos;
}

/******************
 * Puck functions *
 ******************/
Puck
*Puck_new(Vector2 position, float radius, float speed, float min_speed, float max_speed, struct GameState *gamestate){
    Puck *puck = malloc(sizeof(Puck));

    puck->_ = (Thing){
        .kind = PUCK,
        .position = position,
        .velocity = Vector2Zero(),
        .radius = radius,
        .speed = speed,
        .gamestate = gamestate,
        .data = puck,
    };
   puck->min_speed = min_speed;
   puck->max_speed = max_speed;
   puck->update    = &Puck_update;

   return puck;
}

void
Puck_reset(Puck *self){

}

void
Puck_update(Puck *self, float _delta) {
    Vector2 new_pos;
    Thing *thing = &self->_;
    Playfield *playfield = thing->gamestate->playfield;
    new_pos = Vector2Add(thing->position, Vector2Scale(thing->velocity, thing->speed*_delta));

    /* Minkowski bounds */
    float mink_x1 = playfield->bounds.tl.x + thing->radius;
    float mink_y1 = playfield->bounds.tl.y + thing->radius;
    float mink_x2 = playfield->bounds.br.x - thing->radius;
    float mink_y2 = playfield->bounds.br.y - thing->radius;

    if (new_pos.x < mink_x1) {
        if (!is_in_goal(puck, new_pos, 0)) {
            new_pos.x = mink_x1 + (mink_x1 - new_pos.x);
            thing->velocity.x = -thing->velocity.x;
            thing->speed = thing->speed/1.5f;
        } else if (thing->position.x < -7.5f) {
            new_pos = Vector2Zero();
            thing->velocity = Vector2Normalize(thing->velocity);
            thing->speed = 5.0f;
            if (puck->last_collider != NULL) thing->gamestate->scores[1]++;
        }
    }
    if (mink_x2   < new_pos.x) {
        if (!is_in_goal(puck, new_pos, 1)) {
            new_pos.x = mink_x2 - (new_pos.x - mink_x2);
            thing->velocity.x = -thing->velocity.x;
            thing->speed = thing->speed/1.5f;
        } else if (7.5f < thing->position.x) {
            new_pos = Vector2Zero();
            thing->velocity = Vector2Normalize(thing->velocity);
            thing->speed = 5.0f;
            if (puck->last_collider != NULL) thing->gamestate->scores[0]++;
        }
    }
    if (new_pos.y < mink_y1) {
        new_pos.y = mink_y1 + (mink_y1 - new_pos.y);
        thing->velocity.y = -thing->velocity.y;
        thing->speed = thing->speed/1.5f;
    }
    if (mink_y2    < new_pos.y) {
        new_pos.y = mink_y2 - (new_pos.y - mink_y2);
        thing->velocity.y = -thing->velocity.y;
        thing->speed = thing->speed/1.5f;
    }
    if (thing->speed < puck->min_speed) thing->speed = puck->min_speed;



    Puck_move(puck, new_pos);
}
