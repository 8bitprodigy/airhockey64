#ifndef TIMER_H
#define TIMER_H


#include <time.h>


typedef enum {
    ONE_SHOT,
    CONTINUOUS,
}Timer_kinds;

typedef enum {
    RUNNING,
    PAUSED,
    STOPPED,
    TIMEOUT,
}Timer_states;

typedef struct
Timer{
    float           run_time;
    struct timespec elapsed_time;
    void            (*callback)(void *args);
    void            *callback_args;

    Timer_kinds     kind;
    Timer_states    state;

    struct Timer    *prev;
    struct Timer    *next;
} Timer;

int  Timer_new(     Timer *timer,  float run_time,   void *callback);
void Timer_tick(    Timer *timer,  float delta_time);
void Timer_add(     Timer *timer,  Timer *new_timer);
void Timer_destruct(Timer *timers);

#endif /* TIMER_H */
