#include <stdio.h>
#include "planetfall.h"
#include "events.h"

typedef struct {
    EventID id;
    int ticks; // -1 for daemon, >0 for timer
    bool enabled;
} GameEvent;

static GameEvent events[MAX_EVENTS];

void queue_event(EventID id, int ticks) {
    if (id <= 0 || id >= MAX_EVENTS) return;
    events[id].id = id;
    events[id].ticks = ticks;
    events[id].enabled = true;
}

void dequeue_event(EventID id) {
    if (id <= 0 || id >= MAX_EVENTS) return;
    events[id].enabled = false;
}

bool is_event_enabled(EventID id) {
    if (id <= 0 || id >= MAX_EVENTS) return false;
    return events[id].enabled;
}

void save_events(FILE* f) {
    fwrite(events, sizeof(events), 1, f);
}

void restore_events(FILE* f) {
    fread(events, sizeof(events), 1, f);
}

// Forward declarations of logic handlers (to be implemented in logic files)
void routine_blowup_feinstein();
void routine_blather();
void routine_ambassador();
void routine_pod_trip();
void routine_sink_pod();
void routine_hunger();
void i_magnet();
void i_reactor_door_close();
void i_upper_elevator_arrive();
void i_lower_elevator_arrive();
void i_upper_elevator_trip();
void i_lower_elevator_trip();
void i_turnoff_upper_elevator();
void i_turnoff_lower_elevator();
void i_unenter();

bool run_events() {
    for (int i = 0; i < MAX_EVENTS; i++) {
        if (!events[i].enabled) continue;
        
        bool run_now = false;
        
        if (events[i].ticks == -1) {
            // Daemon: Runs every turn
            run_now = true;
        } else if (events[i].ticks > 0) {
            // Timer: count down by the cost of the action just taken, not by
            // one. Ticks are Galactic Standard Time units, so a timer queued
            // for 300 fires after ~15 rooms walked or ~43 turns of standing
            // around. Matches CLOCKER in planetfall.clocker, including its
            // fire-at-or-below-1 boundary.
            events[i].ticks -= game_state.c_elapsed;
            if (events[i].ticks <= 1) {
                events[i].ticks = 0;
                run_now = true;
                events[i].enabled = false; // Timers fire once
            }
        }
        
        if (run_now) {
            switch (events[i].id) {
                case EVT_BLOWUP_FEINSTEIN: routine_blowup_feinstein(); break;
                case EVT_BLATHER: routine_blather(); break;
                case EVT_AMBASSADOR: routine_ambassador(); break;
                case EVT_POD_TRIP: routine_pod_trip(); break;
                case EVT_SINK_POD: routine_sink_pod(); break;
                case EVT_HUNGER_WARNINGS: routine_hunger(); break;
                case EVT_MAGNET: i_magnet(); break;
                case EVT_REACTOR_DOOR_CLOSE: i_reactor_door_close(); break;
                case EVT_UPPER_ELEVATOR_ARRIVE: i_upper_elevator_arrive(); break;
                case EVT_LOWER_ELEVATOR_ARRIVE: i_lower_elevator_arrive(); break;
                case EVT_UPPER_ELEVATOR_TRIP: i_upper_elevator_trip(); break;
                case EVT_LOWER_ELEVATOR_TRIP: i_lower_elevator_trip(); break;
                case EVT_TURNOFF_UPPER_ELEVATOR: i_turnoff_upper_elevator(); break;
                case EVT_TURNOFF_LOWER_ELEVATOR: i_turnoff_lower_elevator(); break;
                case EVT_UNENTER: i_unenter(); break;
                default: break;
            }
        }
    }
    return true;
}
