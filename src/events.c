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

// Forward declarations of logic handlers (to be implemented in logic files)
void routine_blowup_feinstein();
void routine_blather();
void routine_ambassador();
void routine_pod_trip();
void routine_sink_pod();
void routine_hunger();

bool run_events() {
    for (int i = 0; i < MAX_EVENTS; i++) {
        if (!events[i].enabled) continue;
        
        bool run_now = false;
        
        if (events[i].ticks == -1) {
            // Daemon: Runs every turn
            run_now = true;
        } else if (events[i].ticks > 0) {
            // Timer: Decrement
            events[i].ticks--;
            if (events[i].ticks == 0) {
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
                default: break;
            }
            
            // Check for death after every event?
            // In a real engine, JIGS-UP would set a flag.
            // For now, we assume if JIGS-UP happens, it might exit or set a state.
        }
    }
    return true;
}
