#ifndef FEINSTEIN_H
#define FEINSTEIN_H

#include "planetfall.h"

// Setup Function
void init_feinstein_act();

// Globals specific to this Act
extern int blather_leave_counter;
extern int ambassador_leave_counter;
extern int blowup_counter;
extern int trip_counter;
extern int sink_counter;
extern int brigs_up;

// Logic handlers
void routine_blowup_feinstein();
void routine_blather();
void routine_ambassador();
void routine_pod_trip();
void routine_sink_pod();

// Action routines
bool deck_nine_f(int arg);
bool pod_door_f(int arg);
bool gangway_door_f(int arg);
bool gangway_f(int arg);
bool blather_f(int arg);
bool celery_f(int arg);
bool slime_f(int arg);
bool escape_pod_f(int arg);
bool safety_web_f(int arg);
bool controls_f(int arg);
bool food_kit_f(int arg);
bool goo_f(int arg);
bool towel_f(int arg);
bool global_pod_f(int arg);
bool cretin_f(int arg);
bool hands_f(int arg);
bool graffiti_f(int arg);
bool brig_door_f(int arg);
bool window_f(int arg);
bool lights_f(int arg);
const char *ddesc(ZObjectID door);
bool ground_f(void);
bool chronometer_f(int arg);
bool patrol_uniform_f(void);

#endif
