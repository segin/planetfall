#include <stdio.h>
#include <assert.h>
#include "planetfall.h"
#include "events.h"
#include "parser.h"

int brigs_up = 0;
Command current_cmd; // Defined for linking

// Mock logic
void routine_mock() {
    printf("Mock Event Ran\n");
}

void perform_look() {
    // Stub
}

void jigs_up(const char *msg) {
    printf("JIGS UP: %s\n", msg);
}

void test_events() {
    printf("Testing Event Queue...\n");
    // We can't easily hook into static 'events' array in events.c without exposing it.
    // But we can test behavior.
    
    // Clear events
    // EventID max is MAX_EVENTS. But events are mapped.
    // We assume test works with current event IDs.
    
    int test_evt = EVT_SINK_POD; 
    queue_event(test_evt, 2);
    // Since run_events() calls routine_sink_pod() which calls jigs_up(),
    // and routine_sink_pod calls obj_move which uses objects... we need objects initialized.
    // init_game() is in engine_core.c which is linked.
    init_game();

    // However, run_events checks if event is enabled.
    assert(is_event_enabled(test_evt));
    
    // Run events (Tick 1)
    
    run_events(); // Tick 1 (Remaining: 1)
    assert(is_event_enabled(test_evt));
    
    run_events(); // Tick 2 (Remaining: 0 -> Run)
    assert(!is_event_enabled(test_evt)); // Should be disabled after running (timer)
    
    printf("Event Queue Passed.\n");
}

int main() {
    test_events();
    return 0;
}
