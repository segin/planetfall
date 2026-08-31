#include <stdio.h>
#include <assert.h>
#include "planetfall.h"
#include "events.h"

int brigs_up = 0;

// Mock logic
void routine_mock() {
    printf("Mock Event Ran\n");
}

void routine_blowup_feinstein() {}
void routine_blather() {}
void routine_ambassador() {}
void routine_pod_trip() {}
void routine_sink_pod() {}
void routine_hunger() {}
void i_magnet() {}
void i_reactor_door_close() {}
void i_upper_elevator_arrive() {}
void i_lower_elevator_arrive() {}
void i_upper_elevator_trip() {}
void i_lower_elevator_trip() {}
void i_turnoff_upper_elevator() {}
void i_turnoff_lower_elevator() {}
void i_unenter() {}

void test_events() {
    printf("Testing Event Queue...\n");
    // We can't easily hook into static 'events' array in events.c without exposing it.
    // But we can test behavior.
    
    // Clear events
    // EventID max is MAX_EVENTS. But events are mapped.
    // We assume test works with current event IDs.
    
    int test_evt = EVT_SINK_POD; 
    queue_event(test_evt, 2);
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
