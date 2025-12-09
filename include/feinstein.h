#ifndef FEINSTEIN_H
#define FEINSTEIN_H

#include "planetfall.h"

// Room IDs
#define R_BRIG 90
#define R_DECK_NINE 100
#define R_REACTOR_LOBBY 101
#define R_GANGWAY 102
#define R_DECK_EIGHT 103
#define R_ESCAPE_POD 104
#define R_CRAG 105
#define R_BALCONY 106
#define R_WINDING_STAIR 107
#define R_COURTYARD 108

// Object IDs
#define O_SCRUB_BRUSH 200
#define O_PATROL_UNIFORM 201
#define O_CHRONOMETER 202
#define O_ID_CARD 203
#define O_POD_DOOR 300
#define O_CORRIDOR_DOOR 301
#define O_GANGWAY_DOOR 302
#define O_SAFETY_WEB 303
#define O_FOOD_KIT 304
#define O_RED_GOO 305
#define O_BROWN_GOO 306
#define O_GREEN_GOO 307
#define O_TOWEL 308

#define O_BLATHER 400
#define O_AMBASSADOR 401
#define O_CELERY 402
#define O_BROCHURE 403
#define O_GLOBAL_POD 404
#define O_GROUND 405
#define O_SLIME 406

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

#endif
