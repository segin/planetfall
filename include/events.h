#ifndef EVENTS_H
#define EVENTS_H

#include <stdbool.h>
#include <stdio.h>

// Event IDs (mapping to ZIL routines)
typedef enum {
    EVT_NONE = 0,
    EVT_BLOWUP_FEINSTEIN,
    EVT_BLATHER,
    EVT_AMBASSADOR,
    EVT_POD_TRIP,
    EVT_SINK_POD,
    EVT_HUNGER_WARNINGS,
    EVT_SLEEP_WARNINGS,
    EVT_FALL_ASLEEP,
    // Add others as needed
    MAX_EVENTS
} EventID;

// Queue an event to run after 'ticks' turns.
// If ticks is -1, it is a daemon (runs every turn until disabled).
void queue_event(EventID id, int ticks);

// Disable an event
void dequeue_event(EventID id);

// Run all active events/timers
// Returns true if the game should continue, false if player died/quit during event.
bool run_events();

// Check if event is enabled
bool is_event_enabled(EventID id);

void save_events(FILE* f);
void restore_events(FILE* f);

#endif
