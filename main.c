/*******************************************************************************************
*
*   raylib [models] example - Draw textured cube
*
*   Example originally created with raylib 4.5, last time updated with raylib 4.5
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2022-2024 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include <time.h>
#include "airhockey.h"

bool xflag = true;
bool flag  = true;

const Color
Paddle_Colors[] = {
    RED,
    BLUE,
    YELLOW,
    GREEN,
};

//GameState gamestate;
Playfield playfield;
Puck      *puck;

void
draw_playfield(void) {
    Vector3 horiz_side = {16.0f, 1.0f, 1.0f};
    Vector3 vert_side  = { 1.0f, 1.0f, 3.0f}; 
    
    DrawCubeV( /* Top edge of playfield */
        (Vector3){.x = 0.0f, .y = 0.5f, .z = -5.5f},
        horiz_side, GRAY );
    DrawCubeV( /* Bottom edge of playfield */
        (Vector3){.x = 0.0f, .y = 0.5f, .z = 5.5f},
        horiz_side, GRAY );

    DrawCubeV( /* Bottom of right side */
        (Vector3){.x = 7.5f, .y = 0.5f, .z = 3.5f},
        vert_side, GRAY );
    DrawCubeV( /* Top of right side */
        (Vector3){.x = 7.5f, .y = 0.5f, .z = -3.5f},
        vert_side, GRAY );
    
    DrawCubeV( /* Bottom of left side */
        (Vector3){.x = -7.5f, .y = 0.5f, .z = 3.5f},
        vert_side, GRAY );
    DrawCubeV( /* Top of left side */
        (Vector3){.x = -7.5f, .y = 0.5f, .z = -3.5f},
        vert_side, GRAY );
}

int
get_digit(int number, int digit_index) {
    for (int i = 0; i < digit_index; i++) {
        number /= 10;
    }
    return number % 10;
}



void
draw_scores(GameState *gamestate, int screen_width, uint8_t font_size, uint8_t num_digits) {
    uint8_t num_teams = gamestate->num_teams;
    uint8_t increment = (num_digits + 3) * font_size;
    uint8_t padding   = 3 * font_size;
    int score_width   = (num_teams * increment) - padding;
    int start_x = (screen_width-score_width)/2;
    for (int i = 0; i < gamestate->num_teams; i++) {
        int x = start_x + (i*increment);
        DrawText(TextFormat("%02d",gamestate->scores[i]),x,10,font_size,Paddle_Colors[i]);
        if (i == 0) continue;
        DrawText(" | ",x-padding,10,font_size,BLACK);
    }
}

extern Texture2D getFontGLTextureId(char *text);
Rectangle src={0.0f,0.0f,5.0f,10.0f};
Rectangle dst={0.0f,0.0f,32.0f,32.0f};


void
update_paddles(GameState *gamestate) {
    for (int i = 0; i < gamestate->num_players; i++) {
        if (gamestate->paddles[i] == NULL)         continue;
        if (gamestate->paddles[i]->_.kind   != PADDLE) continue;
        if (gamestate->paddles[i]->_.update == NULL) continue;
        gamestate->paddles[i]->update(gamestate->paddles[i], gamestate->delta);
    }
}

void
update_pucks(GameState *gamestate) {
    for (int i = 0; i < JOYPAD_PORT_COUNT; i++) {
        if (gamestate->pucks[i] == NULL)         continue;
        if (gamestate->pucks[i]->update == NULL) continue;
        gamestate->pucks[i]->update(gamestate->pucks[i], gamestate->delta);
    }
}

void
draw_paddles(GameState *gamestate) {
    for (int i = 0; i < gamestate->num_players; i++) {
        if (gamestate->paddles[i] == NULL)         continue;
        if (gamestate->paddles[i]->_.kind != PADDLE) continue;
        Vector3 pos = (Vector3){
            .x = gamestate->paddles[i]->_.position.x,
            .y = 0.125f,
            .z = gamestate->paddles[i]->_.position.y
        };
        //Paddle *paddle = (Paddle *)gamestate.paddles[i]->_.data;
        DrawCylinder(pos, gamestate->paddles[i]->_.radius, gamestate->paddles[i]->_.radius, 0.25f, 8, gamestate->paddles[i]->color);
    }
}

void
draw_puck(void) {
    Vector3 pos = (Vector3){
        .x = puck->_.position.x,
        .y = 0.05f,
        .z = puck->_.position.y
    };
    DrawCylinder(pos, puck->_.radius, puck->_.radius, 0.1f, 8, BLACK);
}

void
setup_players(GameState *gamestate, uint8_t num_players, uint8_t num_teams) {
    for (int i = 0; i < num_teams; i++) gamestate->scores[i] = 0;
    for (int i = 0; i < num_players; i++) {
        if ( !joypad_is_connected(i) ) continue;
        gamestate->num_players++;
        if ( joypad_get_accessory_type(i)==JOYPAD_ACCESSORY_TYPE_RUMBLE_PAK ) gamestate->rumble[i] = true;

        gamestate->paddles[i] = Paddle_new(
            Vector2Lerp(gamestate->playfield->team_bounds[i].tl,gamestate->playfield->team_bounds[i].br, 0.5f),
            0.5f, 25.0f, gamestate, Paddle_Colors[i], i, i);
    }
}


GameState
*GameState_new(uint8_t max_score) {
    GameState *new_gamestate = (GameState *)malloc(sizeof(GameState));

    new_gamestate->max_score = max_score;
    new_gamestate->delta     = 0.0f;

    return new_gamestate;
}


/**************
 *            *
 * S E T U P  *
 *            *
 **************/
GameState
*setup(void){

    int max_score = 7;
    int selection = 0;
    joypad_buttons_t pressed;

    if (!joypad_is_connected(0)) {

        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText(      "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!",50, 20, 10, BLACK);
            DrawText(      "!! Player 1 controller disconnected !!",50, 30, 10, BLACK);
            DrawText(      "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!",50, 40, 10, BLACK);

            DrawText("Please connect a controller to port 1.",      10, 60, 10, BLACK);
        EndDrawing();
        return NULL;
    }

    if (!joypad_is_connected(1)) {
        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText(      "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", 50, 20, 10, BLACK);
            DrawText(      "!! Player 2 controller disconnected !!", 50, 30, 10, BLACK);
            DrawText(      "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", 50, 40, 10, BLACK);

            DrawText("This is currently a 2-player game.",           10, 60, 10, BLACK);
            DrawText("Please connect a controller to port 2.",       10, 70, 10, BLACK);
        EndDrawing();
        return NULL;
    }

    while (true) {
        BeginDrawing();
            ClearBackground(RAYWHITE);

            joypad_poll();
            pressed = joypad_get_buttons_pressed(0);
            if (pressed.d_up)   selection--;
            if (pressed.d_down) selection++;

            if (selection<0) selection = 0;
            if (selection>1) selection = 1;

            if (pressed.d_right) max_score++;
            if (pressed.d_left)  max_score--;

            if (max_score<3)   max_score = 3;
            if (max_score>255) max_score = 255;

            if (pressed.start) break;

            DrawText(      "A I R   H O C K E Y", 55, 40, 20, BLACK);

            DrawText("Use D-pad to change value, press START to begin.", 10, 80,  10, BLACK);

            DrawText(TextFormat("> Play to a score of %d.", max_score),  10, 100, 10, BLACK);

        EndDrawing();
    }
    return GameState_new(max_score);
}


int
win_screen(int player_num){
    if (player_num < 0) return 1;
    joypad_buttons_t pressed;

    while (true){
        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText(           "!!!!!!!!!!!!!!!!!!!!!!!", 90, 20, 10, BLACK);
            DrawText(           "!!                   !!", 90, 30, 10, BLACK);
            DrawText(TextFormat("!!  Player %d Wins!   !!",player_num+1), 90, 40, 10, BLACK);
            DrawText(           "!!                   !!", 90, 50, 10, BLACK);
            DrawText(           "!!!!!!!!!!!!!!!!!!!!!!!", 90, 60, 10, BLACK);


            DrawText(TextFormat("Press START on joypad %d to return to main menu.", player_num+1), 10, 70, 10, BLACK);

        EndDrawing();

        joypad_poll();
        pressed = joypad_get_buttons_pressed(player_num);
        if (pressed.start) break;
    }
    return 0;
}

/*************
 *           *
 *  G A M E  *
 *           *
 *************/
int
game(GameState *gamestate) {
    if (!gamestate) return 0;
    int i=0;

    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 15.0f, 4.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;


    playfield = (Playfield){
        .bounds      = (Area){(Vector2){-7.0f, -5.0f}, (Vector2){7.0f, 5.0f}},
        .team_bounds = {
            (Area){(Vector2){-7.0f, -5.0f}, (Vector2){-1.0f, 5.0f}},
            (Area){(Vector2){ 1.0f, -5.0f}, (Vector2){ 7.0f, 5.0f}}
        },
        .goals = { (Vector2){0.0f, 4.0f}, (Vector2){0.0f, 4.0f} }
    };
    gamestate->playfield = &playfield;
    gamestate->num_teams = 2;

    setup_players(gamestate, 2,2);
    puck = Puck_new(
        (Vector2){0.0f, 0.0f},
        0.25f, 0.0f, 2.5f, 50.0f,
        gamestate
    );

    puck->_.velocity = Vector2Rotate(Vector2One(), PI*((float)rand()/RAND_MAX));
    puck->_.speed    = 50.0f;
    gamestate->pucks[0] = puck;

    if (!puck->update) return -1;

    /* Game Loop */
    /*----------------------------------------------------------*/
    while (flag)
    {
        /* Update */
        gamestate->delta = GetFrameTime();
        joypad_poll();

        update_paddles(gamestate);
        update_pucks(gamestate);
        Timer_tick(gamestate->timers, gamestate->delta);

        /* Drawing */
        BeginDrawing();

            ClearBackground(RAYWHITE);
            /* 3D */
            BeginMode3D(camera);

                //DrawGrid(16, 1.0f);        // Draw a grid

                draw_playfield();
                draw_paddles(gamestate);
                draw_puck();

            EndMode3D();

            DrawFPS(10, 10);
            draw_scores(gamestate, ATTR_NINTENDO64_WIDTH,20,2);

        EndDrawing();


        for (i = 0; i < JOYPAD_PORT_COUNT; i++) {
            if (gamestate->max_score <= gamestate->scores[i]){
                free(gamestate);
                return i;
            }
        }
        //-----------------------------------------------------
    }

    return i;
}


/*************
 *           *
 *  M A I N  *
 *           *
 *************/
int
main(void) {
    const int screenWidth = ATTR_NINTENDO64_WIDTH;
    const int screenHeight = ATTR_NINTENDO64_HEIGHT;
    InitWindow(screenWidth, screenHeight, "Air Hockey 64");
    joypad_init();

    SetTargetFPS(60);
    while (true) {
        if (win_screen(game(setup()))) break;
    }

    CloseWindow();
}
