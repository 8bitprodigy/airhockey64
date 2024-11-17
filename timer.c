#include <stdlib.h>
#include "timer.h"

int Timer_new(Timer *timer, float run_time, void *callback) {
    Timer *new_timer = (Timer *)malloc(sizeof(Timer));
    if (!new_timer) return 0;

    new_timer->elapsed_time = (struct timespec){0,0};
    new_timer->run_time     = run_time;
    new_timer->callback     = callback;
    new_timer->state        = RUNNING;

    Timer_add(timer, new_timer);
    return 1;
}

float ts2f(struct timespec ts) {
    return (float)ts.tv_sec + (float)(ts.tv_nsec/1e9f);
}

void Timer_tick(Timer *timer, float delta_time) {
    if (!timer) return;
    if (timer->state==RUNNING) {
        time_t seconds   = (time_t)delta_time;
        long nanoseconds = (long)((delta_time - seconds) * 1e9);

        timer->elapsed_time.tv_sec  += seconds;
        timer->elapsed_time.tv_nsec += nanoseconds;

        if (timer->run_time <= ts2f(timer->elapsed_time)) {
            timer->callback(timer->callback_args);
            if (timer->kind == ONE_SHOT) {
                timer->state = TIMEOUT;
                Timer_destruct(timer);
            }
        }
    }

    Timer_tick(timer->next, delta_time);
}


void Timer_add(Timer *timers, Timer *new_timer) {
    if (!timers) {
        timers = new_timer;
        return;
    }
    if (!timers->next) {
        timers->next = new_timer;
        return;
    }
    Timer_add(timers->next, new_timer);
}

void Timer_destruct(Timer *timers){
    if (timers->state != TIMEOUT) return;

    if (!timers->next) {
        free(timers);
        return;
    }

    if (!timers->prev) {
        Timer *t = timers->next;

        timers->run_time     = t->run_time;
        timers->elapsed_time = t->elapsed_time;
        timers->callback     = t->callback;

        timers->kind         = t->kind;
        timers->state        = t->state;

        timers->next         = t->next;

        free(t);
        Timer_destruct(timers);
        return;
    }

    timers->prev->next = timers->next;
    timers->next->prev = timers->prev;
    free(timers);
}
