#include <math.h>
#include <float.h>
#include "collision.h"

const Vector2 V2_NAN = (Vector2){NAN, NAN};

bool is_in_circle(Vector2 point1, Vector2 point2, float radius1, Vector2 center, float radius2){
    float dist = Vector2Distance(point1, point2);
    float intervals = floorf(dist / radius1) + 1.0f;
    float increment = dist/intervals;
    for (int i = 0; i < (int)intervals; i++) {
        return CheckCollisionCircles(Vector2Lerp(point1, point2, i*increment), radius1, center, radius2);
    }
    return false;
}

Vector2 line_circle_intersection_point(Vector2 point1, Vector2 point2, Vector2 center, float radius){
    Vector2 direction = Vector2Subtract(point2, point1);
    Vector2 relative  = Vector2Subtract(point1, center);

    float a = direction.x * direction.x + direction.y * direction.y;
    float b = 2 * (relative.x * direction.x + relative.y * direction.y);
    float c = relative.x * relative.x + relative.y * relative.y - radius * radius;

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) return V2_NAN;

    float sqrt_discriminant = sqrtf(discriminant);
    float t1 = (-b - sqrt_discriminant) / (2 * a);
    float t2 = (-b + sqrt_discriminant) / (2 * a);

    float t = t1;

    Vector2 intersection = {
        point1.x + t * direction.x,
        point1.y + t * direction.y
    };

    return intersection;
}

void resolve_puck_paddle_collision(Thing *puck, Vector2 new_pos, Thing *paddle) {
    float move_dist, col_dist, leftover;
    Vector2 col_pos, col_normal, paddle_move;
    Vector2 relative_velocity, impulse;

    // Calculate movement and intersection details
    move_dist = Vector2Distance(puck->position, new_pos);
    paddle_move = Vector2Subtract(paddle->position, paddle->prev_pos);

    // Adjust the new position to account for paddle movement
    col_pos = line_circle_intersection_point(
        puck->position,
        Vector2Subtract(new_pos, paddle_move),
        paddle->position,
        puck->radius + paddle->radius
    );

    col_dist = Vector2Distance(puck->position, col_pos);
    leftover = move_dist - col_dist;

    // Compute collision normal and relative velocity
    col_normal = Vector2Normalize(Vector2Subtract(puck->position, paddle->position));
    relative_velocity = Vector2Subtract(puck->velocity, paddle->velocity);

    // Project the relative velocity along the normal
    float velocity_along_normal = Vector2DotProduct(relative_velocity, col_normal);

    // If the objects are moving apart, skip the response
    if (velocity_along_normal > 0) {
        puck->position = new_pos;
        return;
    }

    // Compute impulse for an elastic collision
    impulse = Vector2Scale(col_normal, -2 * velocity_along_normal);

    // Update the puck's speed based on the impulse magnitude
    float impulse_magnitude = Vector2Length(impulse);
    puck->speed += impulse_magnitude;

    // Normalize velocity to maintain direction
    puck->velocity = Vector2Normalize(Vector2Add(puck->velocity, impulse));

    // Overlap correction to avoid embedding objects
    Vector2 correction = Vector2Scale(col_normal, puck->radius + paddle->radius + 0.01f);
    puck->prev_pos = puck->position;
    puck->position = Vector2Add(puck->position, Vector2Scale(puck->velocity, leftover));
    //puck->position = Vector2Add(col_pos, correction);

    // Apply leftover movement along the new velocity direction
}

void Puck_move(Puck *self, Vector2 new_pos){
    Thing     *parent    = &self->_;
    GameState *gamestate = parent->gamestate;
    Puck      *data      = parent->data;

    for (int i = 0; i < gamestate->num_players; i++) {
        Paddle *paddle = gamestate->paddles[i];
        if (paddle == NULL || paddle->_.kind != PADDLE) continue;

        if (!is_in_circle(parent->position, new_pos, parent->radius, paddle->_.position, paddle->_.radius)) continue;
        resolve_puck_paddle_collision(parent, new_pos, &paddle->_);
        data->last_collider = paddle;
        return;
    }
    parent->position = new_pos;
}

