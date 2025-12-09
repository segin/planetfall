#include <stdio.h>
#include <assert.h>
#include "planetfall.h"
#include "events.h"

// Mock logic
void routine_mock() {
    printf("Mock Event Ran\n");
}

void perform_look() {
    // Stub
}

void test_events() {
    printf("Testing Event Queue...\n");
    // We can't easily hook into static 'events' array in events.c without exposing it.
    // But we can test behavior.
    
    // Clear events
    for (int i=0; i<MAX_EVENTS; i++) dequeue_event(i);
    
    // Queue event 1 for 2 ticks
    queue_event(1, 2);
    assert(is_event_enabled(1));
    
    // Run events (Tick 1)
    // run_events() returns bool.
    // It calls routines based on ID. 
    // We can't mock the switch statement inside events.c easily without refactoring events.c to use function pointers.
    // For this test, we verify the enabling logic.
    
    run_events(); // Tick 1 (Remaining: 1)
    assert(is_event_enabled(1));
    
    run_events(); // Tick 2 (Remaining: 0 -> Run)
    assert(!is_event_enabled(1)); // Should be disabled after running (timer)
    
    printf("Event Queue Passed.\n");
}

int main() {
    test_events();
    return 0;
}
