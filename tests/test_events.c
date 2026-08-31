#include <stdio.h>
#include <assert.h>
#include "planetfall.h"
#include "events.h"

int brigs_up = 0;

// events.c reads game_state.c_elapsed to age its timers. Define it here rather
// than linking engine_core.o so the queue stays testable in isolation.
ZGameState game_state;

static int sink_pod_runs = 0;

// Mock logic
void routine_mock() {
    printf("Mock Event Ran\n");
}

void routine_blowup_feinstein() {}
void routine_blather() {}
void routine_ambassador() {}
void routine_pod_trip() {}
void routine_sink_pod() { sink_pod_runs++; }
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
void i_floyd(void) {}

// Ticks are Galactic Standard Time units, not turns. A timer queued for T fires
// on the first pass where T minus the accumulated action costs drops to 1 or
// below -- the boundary CLOCKER uses in planetfall.clocker.
static int runs_until_fire(EventID evt, int queued_ticks, int c_elapsed) {
    game_state.c_elapsed = c_elapsed;
    queue_event(evt, queued_ticks);

    int runs = 0;
    while (is_event_enabled(evt)) {
        run_events();
        runs++;
        assert(runs < 10000 && "timer never fired");
    }
    return runs;
}

void test_timer_ages_by_action_cost() {
    printf("Testing timers age by action cost...\n");

    // Standing around costs C_ELAPSED_DEFAULT, so a 300-unit timer takes 43
    // turns: 300 - 7*42 = 6, still pending; one more pass lands on -1.
    assert(runs_until_fire(EVT_SINK_POD, 300, C_ELAPSED_DEFAULT) == 43);

    // Walking costs DEFAULT_MOVE, so the same timer expires nearly three times
    // sooner. This is the whole point of the mechanic: exploring burns clock.
    assert(runs_until_fire(EVT_SINK_POD, 300, DEFAULT_MOVE) == 15);

    // The explosion is queued in the 241..330 band; confirm the slowest case
    // still leaves a usable number of turns to look around the Feinstein.
    assert(runs_until_fire(EVT_SINK_POD, 241, C_ELAPSED_DEFAULT) == 35);
    assert(runs_until_fire(EVT_SINK_POD, 330, DEFAULT_MOVE) == 17);

    printf("Timer aging passed.\n");
}

void test_timeless_actions_do_not_age_timers() {
    printf("Testing zero-cost actions do not advance timers...\n");

    game_state.c_elapsed = 0;
    queue_event(EVT_SINK_POD, 2);

    for (int i = 0; i < 100; i++) {
        run_events();
        assert(is_event_enabled(EVT_SINK_POD));
    }

    dequeue_event(EVT_SINK_POD);
    printf("Zero-cost actions passed.\n");
}

void test_daemon_runs_every_pass() {
    printf("Testing daemons run every pass...\n");

    game_state.c_elapsed = C_ELAPSED_DEFAULT;
    sink_pod_runs = 0;
    queue_event(EVT_SINK_POD, -1);

    for (int i = 0; i < 5; i++) {
        run_events();
    }
    assert(sink_pod_runs == 5);
    assert(is_event_enabled(EVT_SINK_POD)); // daemons never expire

    dequeue_event(EVT_SINK_POD);
    printf("Daemons passed.\n");
}

void test_events() {
    printf("Testing Event Queue...\n");

    int test_evt = EVT_SINK_POD;
    game_state.c_elapsed = C_ELAPSED_DEFAULT;
    queue_event(test_evt, 2);
    assert(is_event_enabled(test_evt));

    run_events();
    assert(!is_event_enabled(test_evt)); // 2 - 7 <= 1, so it fires immediately

    printf("Event Queue Passed.\n");
}

int main() {
    test_events();
    test_timer_ages_by_action_cost();
    test_timeless_actions_do_not_age_timers();
    test_daemon_runs_every_pass();
    return 0;
}
