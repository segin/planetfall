#include <stdio.h>
#include <stdlib.h>
#include "planetfall.h"
#include "feinstein.h"
#include "events.h"

// State Variables
int blather_leave_counter = 0;
int ambassador_leave_counter = 0;
int blowup_counter = 0;
int trip_counter = 0;
int sink_counter = 0;

void init_feinstein_act() {
    // === ROOMS ===
    
    // R_BRIG
    ZObject* r = &objects[R_BRIG];
    r->id = R_BRIG;
    r->description = "Brig";
    r->long_description = "You are in the Feinstein's brig. Graffiti cover the walls. The cell door to the south is locked.";
    r->flags = F_RLANDBIT | F_ONBIT;
    
    // R_DECK_NINE (Already partially init, but fully setting here)
    r = &objects[R_DECK_NINE];
    r->id = R_DECK_NINE;
    r->description = "Deck Nine";
    r->long_description = "This is a featureless corridor similar to every other corridor on the ship.\n"
                          "It curves away to starboard, and a gangway leads up.\n"
                          "To port is the entrance to one of the ship's primary escape pods.";
    r->flags = F_RLANDBIT | F_ONBIT;
    r->east = R_REACTOR_LOBBY;
    r->west = R_ESCAPE_POD;
    r->in = R_ESCAPE_POD;
    r->up = R_GANGWAY;
    r->globals[0] = O_POD_DOOR;
    
    // R_REACTOR_LOBBY
    r = &objects[R_REACTOR_LOBBY];
    r->id = R_REACTOR_LOBBY;
    r->description = "Reactor Lobby";
    r->long_description = "The corridor widens here as it nears the main drive area. To starboard is\n"
                          "the Ion Reactor that powers the vessel, and aft of here is the Auxiliary\n"
                          "Control Room. The corridor continues to port.";
    r->flags = F_RLANDBIT | F_ONBIT;
    r->west = R_DECK_NINE;
    
    // R_GANGWAY
    r = &objects[R_GANGWAY];
    r->id = R_GANGWAY;
    r->description = "Gangway";
    r->long_description = "This is a steep metal gangway connecting Deck Eight, above, and Deck Nine, below.";
    r->flags = F_RLANDBIT | F_ONBIT;
    r->up = R_DECK_EIGHT;
    r->down = R_DECK_NINE;
    
    // R_DECK_EIGHT
    r = &objects[R_DECK_EIGHT];
    r->id = R_DECK_EIGHT;
    r->description = "Deck Eight";
    r->long_description = "This is a featureless corridor leading port and starboard. A gangway leads\n"
                          "down, and to fore is the Hyperspatial Jump Machinery Room.";
    r->flags = F_RLANDBIT | F_ONBIT;
    r->down = R_GANGWAY;

    // R_ESCAPE_POD
    r = &objects[R_ESCAPE_POD];
    r->id = R_ESCAPE_POD;
    r->description = "Escape Pod";
    r->synonyms[0] = "pod";
    r->long_description = "This is one of the Feinstein's primary escape pods. A mass of safety webbing fills half the pod.";
    r->flags = F_RLANDBIT | F_ONBIT | F_VEHBIT; 
    r->east = R_DECK_NINE;
    r->out = R_DECK_NINE;
    r->globals[0] = O_POD_DOOR;

    // R_CRAG
    r = &objects[R_CRAG];
    r->id = R_CRAG;
    r->description = "Crag";
    r->long_description = "You are on a rocky crag. A landing balcony is high above you.";
    r->flags = F_RLANDBIT | F_ONBIT;
    r->up = R_BALCONY;
    r->in = R_ESCAPE_POD; // Can re-enter pod

    // R_BALCONY
    r = &objects[R_BALCONY];
    r->id = R_BALCONY;
    r->description = "Balcony";
    r->long_description = "You are on a wide balcony overlooking the ocean. A crag is far below.";
    r->flags = F_RLANDBIT | F_ONBIT;
    r->down = R_CRAG;
    r->up = R_WINDING_STAIR;

    // R_WINDING_STAIR
    r = &objects[R_WINDING_STAIR];
    r->id = R_WINDING_STAIR;
    r->description = "Winding Stair";
    r->long_description = "A winding stair leads up to a courtyard and down to a balcony.";
    r->flags = F_RLANDBIT | F_ONBIT;
    r->down = R_BALCONY;
    r->up = R_COURTYARD;

    // R_COURTYARD
    r = &objects[R_COURTYARD];
    r->id = R_COURTYARD;
    r->description = "Courtyard";
    r->long_description = "You are in a large open courtyard. A winding stair leads down.";
    r->flags = F_RLANDBIT | F_ONBIT;
    r->down = R_WINDING_STAIR;

    // === OBJECTS ===
    
    // GLOBAL POD (Representing the pod from outside)
    // We'll reuse R_ESCAPE_POD ID for the room, but we need an object in DECK-NINE to refer to.
    // Let's create a separate object O_GLOBAL_POD.
    
    // GLOBAL POD (To allow 'enter pod' from Deck Nine)
    ZObject* o = &objects[O_GLOBAL_POD];
    o->id = O_GLOBAL_POD;
    o->description = "escape pod";
    o->synonyms[0] = "pod";
    o->flags = F_VEHBIT | F_NDESCBIT; 
    obj_move(O_GLOBAL_POD, R_DECK_NINE);

    // POD-DOOR
    o = &objects[O_POD_DOOR];
    o->id = O_POD_DOOR;
    o->description = "escape pod bulkhead";
    o->synonyms[0] = "door"; o->synonyms[1] = "bulkhead";
    o->flags = F_DOORBIT | F_NDESCBIT; // Initially closed
    obj_move(O_POD_DOOR, OBJ_LOCAL_GLOBALS);
    
    // BLATHER
    o = &objects[O_BLATHER];
    o->id = O_BLATHER;
    o->description = "Ensign First Class Blather"; 
    o->synonyms[0] = "blather"; o->synonyms[1] = "ensign";
    o->flags = F_ACTORBIT;
    
    // AMBASSADOR
    o = &objects[O_AMBASSADOR];
    o->id = O_AMBASSADOR;
    o->description = "alien ambassador";
    o->synonyms[0] = "ambassador"; o->synonyms[1] = "alien";
    o->flags = F_ACTORBIT;
    
    // CELERY
    o = &objects[O_CELERY];
    o->id = O_CELERY;
    o->description = "piece of celery";
    o->synonyms[0] = "celery";
    o->flags = F_FOODBIT | F_TAKEBIT;

    // BROCHURE
    o = &objects[O_BROCHURE];
    o->id = O_BROCHURE;
    o->description = "brochure";
    o->synonyms[0] = "brochure";
    o->flags = F_TAKEBIT | F_READBIT;
    o->text = "\"The leading export of Blow'k-bibben-Gordo is the adventure game PLANETFALL.\"";

    // SAFETY-WEB
    o = &objects[O_SAFETY_WEB];
    o->id = O_SAFETY_WEB;
    o->description = "safety web";
    o->synonyms[0] = "web"; o->synonyms[1] = "webbing";
    o->flags = F_VEHBIT | F_NDESCBIT | F_CLIMBBIT;
    obj_move(O_SAFETY_WEB, R_ESCAPE_POD);

    // TOWEL
    o = &objects[O_TOWEL];
    o->id = O_TOWEL;
    o->description = "towel";
    o->synonyms[0] = "towel";
    o->flags = F_TAKEBIT | F_READBIT;
    o->text = "\"S.P.S. FEINSTEIN\n  Escape Pod #42\n   Don't Panic!\"";
    // Towel appears later in pod, so don't place yet.

    // GROUND (Generic floor)
    o = &objects[O_GROUND];
    o->id = O_GROUND;
    o->description = "floor";
    o->synonyms[0] = "floor"; o->synonyms[1] = "deck";
    o->flags = F_NDESCBIT | F_SURFACEBIT;
    obj_move(O_GROUND, OBJ_GLOBAL_OBJECTS); // Should be everywhere, but for now we rely on explicit placement or global search
    // Since my parser doesn't do "global objects" well yet (only checks room/inv), I'll put it in Deck Nine for now.
    // Ideally engine_core needs a "global" concept.
    obj_move(O_GROUND, R_DECK_NINE); 

    // SLIME (Pseudo object for Ambassador's slime)
    o = &objects[O_SLIME];
    o->id = O_SLIME;
    o->description = "slime";
    o->synonyms[0] = "slime";
    o->flags = F_NDESCBIT | F_TRYTAKEBIT;
    // Don't place yet, place when ambassador appears.

    // FOOD-KIT
    o = &objects[O_FOOD_KIT];
    o->id = O_FOOD_KIT;
    o->description = "survival kit";
    o->synonyms[0] = "kit";
    o->flags = F_TAKEBIT | F_CONTBIT;
    // Contents
    objects[O_RED_GOO].id = O_RED_GOO; objects[O_RED_GOO].description = "blob of red goo"; objects[O_RED_GOO].synonyms[0] = "goo"; objects[O_RED_GOO].flags = F_FOODBIT | F_TAKEBIT;
    obj_move(O_RED_GOO, O_FOOD_KIT);
    
    objects[O_BROWN_GOO].id = O_BROWN_GOO; objects[O_BROWN_GOO].description = "blob of brown goo"; objects[O_BROWN_GOO].synonyms[0] = "goo"; objects[O_BROWN_GOO].flags = F_FOODBIT | F_TAKEBIT;
    obj_move(O_BROWN_GOO, O_FOOD_KIT);
    
    objects[O_GREEN_GOO].id = O_GREEN_GOO; objects[O_GREEN_GOO].description = "blob of green goo"; objects[O_GREEN_GOO].synonyms[0] = "goo"; objects[O_GREEN_GOO].flags = F_FOODBIT | F_TAKEBIT;
    obj_move(O_GREEN_GOO, O_FOOD_KIT);

    // Start Daemons
    // Blather appears randomly or via logic, but we'll queue the checker
    queue_event(EVT_BLATHER, -1);
    queue_event(EVT_AMBASSADOR, -1);
    queue_event(EVT_BLOWUP_FEINSTEIN, -1); // Starts the sequence logic
    queue_event(EVT_HUNGER_WARNINGS, -1);
}

void routine_blowup_feinstein() {
    // If pod trip has started (counter > 0), behave differently
    if (trip_counter > 0) {
        // We are away.
        // ZIL Logic: At counter 5 (relative to explosion start), explode ship visible from window.
        // My blowup_counter tracks separately.
        // If trip is active, we just wait for the right moment or disable if we are safe.
        // ZIL: If BLOWUP-COUNTER == 5 ... 
        
        // Let's increment blowup counter even if away
        blowup_counter++;
        
        if (blowup_counter == 20) {
             printf("\nThrough the viewport of the pod you see the Feinstein dwindle as you head\n"
                    "away. Bursts of light dot its hull. Suddenly, a huge explosion blows the\n"
                    "Feinstein into tiny pieces, sending the escape pod tumbling away!\n");
             dequeue_event(EVT_BLOWUP_FEINSTEIN);
        }
        return;
    }

    blowup_counter++;
    
    // Trigger points
    if (game_state.internal_moves == 10 && blowup_counter < 5) {
         printf("\nEnsign Blather rushes past you, screaming about 'Explosions! abandon ship!'\n");
    }
    
    if (blowup_counter == 20) {
        if (current_room == R_DECK_NINE || current_room == R_BRIG || current_room == R_REACTOR_LOBBY) {
            printf("\nAn enormous explosion tears the walls of the ship apart. You die.\n");
            // exit(0);
        } else {
            // In Pod but haven't launched? Or somewhere else?
            // If in pod and door closed, trip should have started.
            // If in pod and door open, you die.
            printf("\nThe ship explodes around you. You die.\n");
        }
    }
}

int brigs_up = 0;

void routine_blather() {
    // Simplified Blather
    if (current_room == R_DECK_NINE && blather_leave_counter == 0) {
        // 20% prob he appears if not already here
        if (!obj_in(O_BLATHER, current_room) && (rand() % 100) < 20) {
            obj_move(O_BLATHER, current_room);
            printf("\nEnsign Blather swaggers in. 'You call this polishing? 30 demerits!'\n");
        }
    }
    
    // If you are thrown in Brig logic
    // Usually triggered by specific actions (leaving post repeatedly)
    // Here we simulate it by accumulation of 'brigs_up' via main loop or direct interaction
    if (brigs_up > 3) {
        printf("\nBlather loses his patience. 'That's it, Ensign! To the Brig!'\n");
        printf("He drags you to the brig and throws you in. The door clangs shut.\n");
        obj_move(player, R_BRIG);
        current_room = R_BRIG;
        obj_rob(player, R_DECK_NINE); // Rob player, put items on Deck Nine (or Crag per ZIL oddity)
        // Let's put them in Deck Nine so you lose them.
        brigs_up = 0;
        perform_look();
    }
}

void routine_ambassador() {
    if (current_room == R_DECK_NINE && ambassador_leave_counter == 0) {
        if (!obj_in(O_AMBASSADOR, current_room) && (rand() % 100) < 15) {
             obj_move(O_AMBASSADOR, current_room);
             obj_move(O_CELERY, O_AMBASSADOR);
             
             printf("\nThe alien ambassador ambles toward you. He drops a brochure.\n");
             obj_move(O_BROCHURE, current_room);
             
             // Leave slime
             obj_move(O_SLIME, current_room);
             printf("He leaves a trail of green slime on the deck.\n");
        }
    }
}

void routine_pod_trip() {
    trip_counter++;
    
    // Based on globals.zil I-POD-TRIP
    if (trip_counter == 1) {
        printf("\nAs the escape pod tumbles away from the former location of the Feinstein, its\n"
               "gyroscopes whine. The pod slowly stops tumbling. Lights on the control panel\n"
               "blink furiously as the autopilot searches for a reasonable destination.\n");
    }
    else if (trip_counter == 2) {
        printf("\nThe auxiliary rockets fire briefly, and a nearby planet swings into view\n"
               "through the port. It appears to be almost entirely ocean, with just a few\n"
               "visible islands and an unusually small polar ice cap. A moment later, the\n"
               "system's sun swings into view, and the viewport polarizes into a featureless\n"
               "black rectangle.\n");
    }
    else if (trip_counter == 3) {
        printf("\nThe main thrusters fire a long, gentle burst. A monotonic voice issues\n"
               "from the control panel. \"Approaching planet...human-habitable.\"\n");
    }
    else if (trip_counter == 7) {
        printf("\nThe pod is buffeted as it enters the planet's atmosphere.\n");
    }
    else if (trip_counter == 8) {
        printf("\nYou feel the temperature begin to rise, and the pod's climate\n"
               "control system roars as it labors to compensate.\n");
    }
    else if (trip_counter == 9) {
        printf("\nThe viewport suddenly becomes transparent again, giving you a view of\n"
               "endless ocean below. The lights on the control panel flash madly as\n"
               "the pod's computer searches for a suitable landing site. The thrusters fire\n"
               "long and hard, slowing the pod's descent.\n");
    }
    else if (trip_counter == 10) {
        printf("\nThe pod is now approaching the closer of a pair of islands. It appears\n"
               "to be surrounded by sheer cliffs rising from the water, and is topped by\n"
               "a wide plateau. The plateau seems to be covered by a sprawling complex\n"
               "of buildings.\n");
    }
    else if (trip_counter == 11) {
        // Landing Logic
        if (obj_in(player, O_SAFETY_WEB)) {
            printf("\nThe pod lands with a thud. Through the viewport you can see a rocky cleft\n"
                   "and some water below. The pod rocks gently back and forth as if it was\n"
                   "precariously balanced. A previously unseen panel slides open, revealing\n"
                   "some emergency provisions, including a survival kit and a towel.\n");
            
            // Trigger provisions
            obj_move(O_TOWEL, R_ESCAPE_POD);
            obj_move(O_FOOD_KIT, R_ESCAPE_POD);
            
            // Update connections
            objects[R_ESCAPE_POD].out = R_CRAG;
            objects[R_ESCAPE_POD].east = R_CRAG;
            objects[R_ESCAPE_POD].up = R_CRAG;
            
            // Move the visible pod object to the Crag so it can be entered
            obj_move(O_GLOBAL_POD, R_CRAG);
            
            // printf("DEBUG: Updated R_ESCAPE_POD (ID %d) exits. Out=%d, East=%d\n", R_ESCAPE_POD, objects[R_ESCAPE_POD].out, objects[R_ESCAPE_POD].east);
            
            dequeue_event(EVT_POD_TRIP);
            
            // Start Sinking Logic?
            // "If you stand... shift... falling... water rising".
            // This is triggered by standing up, not automatic unless you wait too long?
            // ZIL: "As you stand... I-SINK-POD enabled".
            // We need to hook V_STAND/DISEMBARK to enable I-SINK-POD.
        } else {
            printf("\nThe pod, whose automated controls were unfortunately designed by computer\n"
                   "scientists, lands with a good deal of force. Your body sails across the pod\n"
                   "until it is stopped by one of the sharper corners of the control panel.\n");
            printf("**** You have died ****\n");
            // exit(0);
        }
    }
}

void routine_sink_pod() {
    sink_counter++;
    if (sink_counter > 5) {
        printf("\nThe pod sinks. You drown.\n");
        // exit(0);
    }
}

void routine_hunger() {
    game_state.hunger_level++;
    int h = game_state.hunger_level;
    
    // Based on typical thresholds
    if (h == 200) {
        printf("You are beginning to feel a bit hungry.\n");
    } else if (h == 300) {
        printf("You are getting pretty hungry.\n");
    } else if (h == 400) {
        printf("You are famished.\n");
    } else if (h == 500) {
        printf("You are starting to faint from lack of food.\n");
    } else if (h == 600) {
        printf("\nYou have starved to death.\n");
        printf("**** You have died ****\n");
        // exit(0);
    }
}
