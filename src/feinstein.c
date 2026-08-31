#include "feinstein.h"
#include "actions.h"
#include "events.h"
#include "output.h"
#include "planetfall.h"
#include <stdio.h>
#include <stdlib.h>

// Note: Globals moved to game_state in planetfall.h

void init_feinstein_act() {
  // === ROOMS ===

  // R_BRIG
  ZObject *r = &objects[R_BRIG];
  r->id = R_BRIG;
  r->description = "Brig";
  r->long_description = "You are in the Feinstein's brig. Graffiti cover\n"
                        "the walls. The cell door to the south is locked.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->globals[0] = O_GRAFFITI_PSEUDO;
  r->globals[1] = O_BRIG_DOOR_PSEUDO;

  // R_DECK_NINE (Already partially init, but fully setting here)
  r = &objects[R_DECK_NINE];
  r->id = R_DECK_NINE;
  r->description = "Deck Nine";
  r->long_description =
      "This is a featureless corridor similar to every other corridor on the "
      "ship.\n"
      "It curves away to starboard, and a gangway leads up.\n"
      "To port is the entrance to one of the ship's primary escape pods.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->east = R_REACTOR_LOBBY;
  r->west = R_ESCAPE_POD;
  r->in = R_ESCAPE_POD;
  r->up = R_GANGWAY;
  r->globals[0] = O_POD_DOOR;
  r->globals[1] = O_CORRIDOR_DOOR;
  r->globals[2] = O_GANGWAY_DOOR;
  // DECK-NINE-F reports whether the pod bulkhead is open or closed, which the
  // static long_description above cannot do.
  r->action = deck_nine_f;

  // R_REACTOR_LOBBY
  r = &objects[R_REACTOR_LOBBY];
  r->id = R_REACTOR_LOBBY;
  r->description = "Reactor Lobby";
  r->long_description = "The corridor widens here as it nears the main drive "
                        "area. To starboard is\n"
                        "the Ion Reactor that powers the vessel, and aft of "
                        "here is the Auxiliary\n"
                        "Control Room. The corridor continues to port.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->west = R_DECK_NINE;
  r->globals[0] = O_CORRIDOR_DOOR;

  // R_GANGWAY
  r = &objects[R_GANGWAY];
  r->id = R_GANGWAY;
  r->description = "Gangway";
  r->long_description = "This is a steep metal gangway connecting Deck Eight, "
                        "above, and Deck Nine, below.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->up = R_DECK_EIGHT;
  r->down = R_DECK_NINE;
  r->globals[0] = O_GANGWAY_DOOR;
  r->action = gangway_f;

  // R_DECK_EIGHT
  r = &objects[R_DECK_EIGHT];
  r->id = R_DECK_EIGHT;
  r->description = "Deck Eight";
  r->long_description =
      "This is a featureless corridor leading port and starboard. A gangway "
      "leads\n"
      "down, and to fore is the Hyperspatial Jump Machinery Room.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->down = R_GANGWAY;

  // R_ESCAPE_POD
  r = &objects[R_ESCAPE_POD];
  r->id = R_ESCAPE_POD;
  r->description = "Escape Pod";
  r->synonyms[0] = "pod";
  r->long_description = "This is one of the Feinstein's primary escape pods. A "
                        "mass of safety webbing fills half the pod.";
  r->flags = F_RLANDBIT | F_ONBIT | F_VEHBIT;
  r->value = 3;
  // POD-EXIT-F: before launch the pod opens back onto Deck Nine and UP goes
  // nowhere; after the landing it opens into the water instead and EAST goes
  // nowhere. See routine_pod_trip for the switch.
  r->east = R_DECK_NINE;
  r->out = R_DECK_NINE;
  r->globals[0] = O_POD_DOOR;
  r->globals[1] = O_CONTROLS;
  r->globals[2] = O_LIGHTS;
  r->globals[3] = O_GLOBAL_POD;
  r->globals[4] = O_WINDOW;
  r->action = escape_pod_f;

  // === OBJECTS ===

  // GLOBAL POD (Representing the pod from outside)
  // We'll reuse R_ESCAPE_POD ID for the room, but we need an object in
  // DECK-NINE to refer to. Let's create a separate object O_GLOBAL_POD.

  // GLOBAL POD (To allow 'enter pod' from Deck Nine)
  ZObject *o = &objects[O_GLOBAL_POD];
  o->id = O_GLOBAL_POD;
  o->description = "escape pod";
  o->synonyms[0] = "pod";
  o->adjectives[0] = "escape";
  o->adjectives[1] = "emergency";
  o->adjectives[2] = "primary";
  o->flags = F_VOWELBIT | F_VEHBIT | F_NDESCBIT;
  o->action = global_pod_f;
  obj_move(O_GLOBAL_POD, R_DECK_NINE);

  // CONTROLS / LIGHTS / WINDOW: fittings shared by the pod, the elevators, the
  // shuttle cars and the helicopter.
  o = &objects[O_CONTROLS];
  o->id = O_CONTROLS;
  o->description = "set of controls";
  o->synonyms[0] = "controls";
  o->synonyms[1] = "control";
  o->synonyms[2] = "panel";
  o->flags = F_NDESCBIT;
  o->action = controls_f;
  obj_move(O_CONTROLS, OBJ_LOCAL_GLOBALS);

  o = &objects[O_LIGHTS];
  o->id = O_LIGHTS;
  o->description = "lights";
  o->synonyms[0] = "lights";
  o->synonyms[1] = "light";
  o->flags = F_NDESCBIT;
  o->action = lights_f;
  obj_move(O_LIGHTS, OBJ_LOCAL_GLOBALS);

  o = &objects[O_WINDOW];
  o->id = O_WINDOW;
  o->description = "window";
  o->synonyms[0] = "window";
  o->synonyms[1] = "port";
  o->synonyms[2] = "viewport";
  o->adjectives[0] = "view";
  o->flags = F_NDESCBIT;
  o->action = window_f;
  obj_move(O_WINDOW, OBJ_LOCAL_GLOBALS);

  // HANDS: always available, so that SHAKE HANDS has something to resolve to.
  o = &objects[O_HANDS];
  o->id = O_HANDS;
  o->description = "pair of hands";
  o->synonyms[0] = "hands";
  o->synonyms[1] = "hand";
  o->flags = F_NDESCBIT;
  o->action = hands_f;
  obj_move(O_HANDS, OBJ_GLOBAL_OBJECTS);

  // BRIG scenery: PSEUDO "GRAFFITI" and "DOOR" on the BRIG room.
  o = &objects[O_GRAFFITI_PSEUDO];
  o->id = O_GRAFFITI_PSEUDO;
  o->description = "graffiti";
  o->synonyms[0] = "graffiti";
  o->synonyms[1] = "writing";
  o->synonyms[2] = "wall";
  o->synonyms[3] = "walls";
  o->flags = F_NDESCBIT | F_READBIT;
  o->action = graffiti_f;
  obj_move(O_GRAFFITI_PSEUDO, OBJ_LOCAL_GLOBALS);

  o = &objects[O_BRIG_DOOR_PSEUDO];
  o->id = O_BRIG_DOOR_PSEUDO;
  o->description = "cell door";
  o->synonyms[0] = "door";
  o->synonyms[1] = "cell";
  o->adjectives[0] = "cell";
  o->flags = F_NDESCBIT;
  o->action = brig_door_f;
  obj_move(O_BRIG_DOOR_PSEUDO, OBJ_LOCAL_GLOBALS);

  // POD-DOOR
  o = &objects[O_POD_DOOR];
  o->id = O_POD_DOOR;
  o->description = "escape pod bulkhead";
  o->synonyms[0] = "door";
  o->synonyms[1] = "bulkhead";
  o->flags = F_DOORBIT | F_VOWELBIT | F_NDESCBIT; // Initially closed
  o->action = pod_door_f;
  obj_move(O_POD_DOOR, OBJ_LOCAL_GLOBALS);

  // CORRIDOR-DOOR and GANGWAY-DOOR: the emergency bulkheads sealing Deck Nine's
  // starboard corridor and gangway. Both start open and unseen, and stay that
  // way until the second wave of explosions crashes them shut.
  o = &objects[O_CORRIDOR_DOOR];
  o->id = O_CORRIDOR_DOOR;
  o->description = "wide bulkhead";
  o->synonyms[0] = "door";
  o->synonyms[1] = "bulkhead";
  o->adjectives[0] = "emergency";
  o->adjectives[1] = "wide";
  o->flags = F_INVISIBLE | F_DOORBIT | F_OPENBIT | F_NDESCBIT;
  o->action = gangway_door_f;
  obj_move(O_CORRIDOR_DOOR, OBJ_LOCAL_GLOBALS);

  o = &objects[O_GANGWAY_DOOR];
  o->id = O_GANGWAY_DOOR;
  o->description = "narrow bulkhead";
  o->synonyms[0] = "door";
  o->synonyms[1] = "bulkhead";
  o->adjectives[0] = "emergency";
  o->adjectives[1] = "narrow";
  o->flags = F_INVISIBLE | F_DOORBIT | F_OPENBIT | F_NDESCBIT;
  o->action = gangway_door_f;
  obj_move(O_GANGWAY_DOOR, OBJ_LOCAL_GLOBALS);

  // BLATHER
  o = &objects[O_BLATHER];
  o->id = O_BLATHER;
  o->description = "Ensign First Class";
  o->long_description =
      "Ensign First Class Blather is standing before you, furiously scribbling\n"
      "demerits onto an oversized clipboard.";
  o->synonyms[0] = "ensign";
  o->synonyms[1] = "blather";
  o->adjectives[0] = "ensign";
  o->adjectives[1] = "first";
  o->adjectives[2] = "class";
  o->size = 150;
  o->flags = F_VOWELBIT | F_ACTORBIT;
  o->action = blather_f;

  // AMBASSADOR
  o = &objects[O_AMBASSADOR];
  o->id = O_AMBASSADOR;
  o->description = "alien ambassador";
  o->long_description =
      "A high-ranking ambassador from a newly-contacted alien race is standing\n"
      "here on three of his legs, and watching you with seven of his eyes.";
  o->synonyms[0] = "ambassador";
  o->adjectives[0] = "alien";
  o->adjectives[1] = "high-ranking";
  o->adjectives[2] = "important";
  o->adjectives[3] = "very";
  o->size = 150;
  o->flags = F_VOWELBIT | F_ACTORBIT;
  o->action = ambassador_f;

  // CELERY -- deliberately not takeable; CELERY-F rebuffs the attempt.
  o = &objects[O_CELERY];
  o->id = O_CELERY;
  o->description = "piece of celery";
  o->synonyms[0] = "celery";
  o->synonyms[1] = "piece";
  o->synonyms[2] = "stalk";
  o->flags = F_NDESCBIT | F_FOODBIT;
  o->action = celery_f;

  // BROCHURE
  o = &objects[O_BROCHURE];
  o->id = O_BROCHURE;
  o->description = "brochure";
  o->long_description =
      "Unfortunately, one of those stupid Blow'k-bibben-Gordo brochures is "
      "here.";
  o->synonyms[0] = "brochure";
  o->synonyms[1] = "pamphlet";
  o->synonyms[2] = "leaflet";
  o->size = 4;
  o->flags = F_ACIDBIT | F_TAKEBIT | F_READBIT;
  o->text = "\"The leading export of Blow'k-bibben-Gordo is the adventure "
            "game\n\n"
            "          *** PLANETFALL ***\n\n"
            "written by S. Eric Meretzky.\n"
            "Buy one today. Better yet, buy a thousand.\"";

  // SAFETY-WEB
  o = &objects[O_SAFETY_WEB];
  o->id = O_SAFETY_WEB;
  o->description = "safety web";
  o->synonyms[0] = "web";
  o->synonyms[1] = "webbing";
  o->synonyms[2] = "mass";
  o->synonyms[3] = "net";
  o->adjectives[0] = "safety";
  o->flags = F_VEHBIT | F_NDESCBIT | F_CLIMBBIT;
  o->action = safety_web_f;
  obj_move(O_SAFETY_WEB, R_ESCAPE_POD);

  // TOWEL
  o = &objects[O_TOWEL];
  o->id = O_TOWEL;
  o->description = "towel";
  o->synonyms[0] = "towel";
  o->size = 10;
  o->action = towel_f;
  o->flags = F_TAKEBIT | F_READBIT;
  o->text = "\"S.P.S. FEINSTEIN\n  Escape Pod #42\n   Don't Panic!\"";
  // Towel appears later in pod, so don't place yet.

  // GROUND (Generic floor)
  o = &objects[O_GROUND];
  o->id = O_GROUND;
  o->description = "floor";
  o->synonyms[0] = "floor";
  o->synonyms[1] = "deck";
  o->synonyms[2] = "ground";
  o->flags = F_NDESCBIT | F_SURFACEBIT;
  o->action = ground_f;
  // GROUND lives in GLOBAL-OBJECTS so it is reachable everywhere. It used to be
  // parked in Deck Nine as a workaround for a parser that only searched the room
  // and inventory; snarf_objects walks the global scopes now.
  obj_move(O_GROUND, OBJ_GLOBAL_OBJECTS);

  // SLIME (Pseudo object for Ambassador's slime)
  o = &objects[O_SLIME];
  o->id = O_SLIME;
  o->description = "slime";
  o->synonyms[0] = "slime";
  o->adjectives[0] = "green";
  o->flags = F_NDESCBIT | F_TRYTAKEBIT;
  o->action = slime_f;
  // Don't place yet, place when ambassador appears.

  // FOOD-KIT
  o = &objects[O_FOOD_KIT];
  o->id = O_FOOD_KIT;
  o->description = "survival kit";
  o->synonyms[0] = "kit";
  o->synonyms[1] = "provisions";
  o->adjectives[0] = "survival";
  o->adjectives[1] = "emergency";
  o->size = 10;
  o->action = food_kit_f;
  o->flags = F_TAKEBIT | F_CONTBIT | F_SEARCHBIT;

  // The goo is deliberately not takeable -- GOO-F insists you eat it straight
  // from the kit.
  o = &objects[O_RED_GOO];
  o->id = O_RED_GOO;
  o->description = "blob of red goo";
  o->synonyms[0] = "goo";
  o->synonyms[1] = "blob";
  o->adjectives[0] = "red";
  o->flags = F_ACIDBIT | F_FOODBIT;
  o->action = goo_f;
  obj_move(O_RED_GOO, O_FOOD_KIT);

  o = &objects[O_BROWN_GOO];
  o->id = O_BROWN_GOO;
  o->description = "blob of brown goo";
  o->synonyms[0] = "goo";
  o->synonyms[1] = "blob";
  o->adjectives[0] = "brown";
  o->flags = F_ACIDBIT | F_FOODBIT;
  o->action = goo_f;
  obj_move(O_BROWN_GOO, O_FOOD_KIT);

  o = &objects[O_GREEN_GOO];
  o->id = O_GREEN_GOO;
  o->description = "blob of green goo";
  o->synonyms[0] = "goo";
  o->synonyms[1] = "blob";
  o->adjectives[0] = "green";
  o->flags = F_ACIDBIT | F_FOODBIT;
  o->action = goo_f;
  obj_move(O_GREEN_GOO, O_FOOD_KIT);

  // Start Daemons
  // Blather appears randomly or via logic, but we'll queue the checker
  queue_event(EVT_BLATHER, -1);
  queue_event(EVT_AMBASSADOR, -1);
  // <ENABLE <QUEUE I-BLOWUP-FEINSTEIN <+ <RANDOM 90> 240>>> from
  // I-RANDOM-INTERRUPTS in misc.zil. 241..330 GST units, which is a dozen-odd
  // rooms of walking or forty-some turns of standing still -- enough to look
  // around the Feinstein before it comes apart.
  queue_event(EVT_BLOWUP_FEINSTEIN, 240 + (rand() % 90) + 1);
  queue_event(EVT_HUNGER_WARNINGS, -1);
}

void routine_blowup_feinstein() {
  // Re-queue as daemon if it was a timer
  queue_event(EVT_BLOWUP_FEINSTEIN, -1);

  game_state.blowup_counter++;

  // ZIL Parity Logic
  if (game_state.blowup_counter == 1) {
    // <SETG BRIGS-UP 0> -- Blather has bigger problems now than your demerits.
    game_state.brigs_up = 0;
    tellf("\nA massive explosion rocks the ship. Echoes from the explosion "
          "resound\n"
          "deafeningly down the halls.\n");
    if (current_room == R_DECK_NINE) {
      tellf("The door to port slides open.\n");
      obj_set_flag(O_POD_DOOR, F_OPENBIT);

      if (obj_in(O_AMBASSADOR, R_DECK_NINE)) {
        obj_remove(O_AMBASSADOR);
        obj_remove(O_CELERY);
        tellf("The ambassador squawks frantically, evacuates a massive load of "
              "gooey\n"
              "slime, and rushes away.\n");
      } else if (obj_in(O_BLATHER, R_DECK_NINE)) {
        obj_remove(O_BLATHER);
        tellf("Blather, confused by this non-routine occurrence, orders you to "
              "continue\n"
              "scrubbing the floor, and then dashes off.\n");
      }
    } else {
      obj_set_flag(O_POD_DOOR, F_OPENBIT);
    }
  } else if (game_state.blowup_counter == 2) {
    // The emergency bulkheads crash shut, sealing Deck Nine off from the
    // gangway and the starboard corridor. They become visible in the process.
    obj_clear_flag(O_CORRIDOR_DOOR, F_OPENBIT);
    obj_clear_flag(O_CORRIDOR_DOOR, F_INVISIBLE);
    obj_clear_flag(O_GANGWAY_DOOR, F_OPENBIT);
    obj_clear_flag(O_GANGWAY_DOOR, F_INVISIBLE);

    if (current_room == R_DECK_NINE) {
      tellf("\nMore distant explosions! A narrow emergency bulkhead at the base "
            "of the\n"
            "gangway and a wider one along the corridor to starboard both crash "
            "shut!\n");
    } else if (current_room == R_ESCAPE_POD || current_room == R_BRIG) {
      tellf("\nThe ship shakes again. You hear, from close by, the sounds of "
            "emergency\n"
            "bulkheads closing.\n");
    } else if (current_room == R_GANGWAY) {
      tellf("\nAnother explosion. A narrow bulkhead at the base of the\n"
            "gangway slams shut!\n");
    } else {
      tellf("\nYou are deafened by more explosions and by the sound of "
            "emergency bulkheads\n"
            "slamming closed. ");
      if (obj_in(O_BLATHER, current_room)) {
        tellf("Blather, foaming slightly at the mouth, screams at you to swab "
              "the decks");
      } else {
        obj_move(O_BLATHER, current_room);
        tellf("Blather enters, looking confused, and begins ranting madly at "
              "you");
      }
      tellf(".\n");
    }
  } else if (game_state.blowup_counter == 3) {
    obj_clear_flag(O_POD_DOOR, F_OPENBIT); // Close Door
    if (current_room == R_DECK_NINE) {
      tellf("\nMore powerful explosions buffet the ship. The lights flicker "
            "madly,\n"
            "and the escape-pod bulkhead clangs shut.\n");
    } else if (current_room == R_ESCAPE_POD) {
      tellf("\nThe pod door clangs shut as heavy explosions continue to buffet "
            "the\n"
            "Feinstein.\n");
      // Check if we should launch?
      if (!is_event_enabled(EVT_POD_TRIP)) {
        queue_event(EVT_POD_TRIP, -1);
      }
    } else {
      jigs_up("\nThe ship rocks from the force of multiple explosions. The "
              "lights go out, and\n"
              "you feel a sudden drop in pressure accompanied by a loud "
              "hissing. Too bad you\n"
              "weren't in the escape pod...");
    }
  } else if (game_state.blowup_counter == 4) {
    if (current_room == R_DECK_NINE) {
      tellf("\nExplosions continue to rock the ship.\n");
    } else if (current_room == R_ESCAPE_POD) {
      tellf("\nYou feel the pod begin to slide down its ejection tube as "
            "explosions shake\n"
            "the mother ship.\n");
    }
  } else if (game_state.blowup_counter == 5) {
    if (current_room == R_DECK_NINE) {
      jigs_up("\nAn enormous explosion tears the walls of the ship apart. If "
              "only you\n"
              "had made it to an escape pod...");
    } else {
      // Success - we are away.
      tellf("\nThrough the viewport of the pod you see the Feinstein dwindle "
            "as you head\n"
            "away. Bursts of light dot its hull. Suddenly, a huge explosion "
            "blows the\n"
            "Feinstein into tiny pieces, sending the escape pod tumbling "
            "away!\n");

      // Start trip if not started? (Should have started at 3)
      if (!is_event_enabled(EVT_POD_TRIP)) {
        queue_event(EVT_POD_TRIP, -1);
      }
      dequeue_event(EVT_BLOWUP_FEINSTEIN);
    }
  }
}

// I-BLATHER (globals.zil). Blather patrols two beats: he hunts you down if you
// wander off Deck Nine, and he drops by to sneer while you are working.
void routine_blather() {
  if (current_room == R_DECK_EIGHT || current_room == R_REACTOR_LOBBY) {
    if (obj_in(O_BLATHER, current_room)) {
      game_state.brigs_up++;
      if (game_state.brigs_up > 3) {
        tellf("\nBlather loses his last vestige of patience and drags you to "
              "the Feinstein's\n"
              "brig. He throws you in, and the door clangs shut behind you.\n\n");
        obj_move(player, R_BRIG);
        current_room = R_BRIG;
        // Your belongings do not come with you; they turn up much later.
        obj_rob(player, R_CRAG);
        obj_move(O_PADLOCK, current_room);
        obj_clear_flag(O_PADLOCK, F_TAKEBIT);
        perform_look();
      } else {
        tellf("\n\"I said to return to your post, Ensign Seventh Class!\" "
              "bellows Blather,\n"
              "turning a deepening shade of crimson.\n");
      }
    } else if (game_state.blowup_counter == 0) {
      obj_move(O_BLATHER, current_room);
      tellf("\nEnsign Blather, his uniform immaculate, enters and notices you "
            "are away\n"
            "from your post. \"Twenty demerits, Ensign Seventh Class!\" bellows "
            "Blather.\n"
            "\"Forty if you're not back on Deck Nine in five seconds!\" He "
            "curls his face\n"
            "into a hideous mask of disgust at your unbelievable negligence.\n");
    }
  } else if (current_room == R_DECK_NINE) {
    if (game_state.blather_leave_counter == 3 &&
        obj_in(O_BLATHER, current_room)) {
      game_state.blather_leave_counter = 0;
      obj_remove(O_BLATHER);
      tellf("\nBlather, adding fifty more demerits for good measure, moves off "
            "in search\n"
            "of more young ensigns to terrorize.\n");
    } else if (obj_in(O_BLATHER, R_DECK_NINE)) {
      game_state.blather_leave_counter++;
    } else if (!obj_in(O_AMBASSADOR, current_room) &&
               game_state.blowup_counter == 0 && (rand() % 100) < 5) {
      obj_move(O_BLATHER, current_room);
      tellf("\nEnsign First Class Blather swaggers in. He studies your work "
            "with half-closed\n"
            "eyes. \"You call this polishing, Ensign Seventh Class?\" he "
            "sneers. \"We have\n"
            "a position for an Ensign Ninth Class in the toilet-scrubbing "
            "division,\n"
            "you know. Thirty demerits.");
      if (!obj_has_flag(O_PATROL_UNIFORM, F_WORNBIT)) {
        tellf(" And another sixty for improper dress!");
      }
      tellf("\" He glares at you, his arms crossed.\n");
    }
  }
}

// AMBASSADOR-QUOTES (globals.zil), picked at random each turn he lingers.
static const char *ambassador_quotes[] = {
    "introduces himself as Br'gun-te'elkner-ipg'nun.",
    "asks if you are performing some sort of religious ceremony.",
    "inquires whether you are interested in a game of Bocci.",
    "recites a plea for coexistence between your races.",
    "asks where Admiral Smithers can be found.",
    "remarks that all humans look alike to him.",
    "offers you a bit of celery.",
};
#define NUM_AMBASSADOR_QUOTES                                                  \
  (sizeof(ambassador_quotes) / sizeof(ambassador_quotes[0]))

// I-AMBASSADOR (globals.zil). He shows up once, small-talks for three turns,
// then leaves for good -- the interrupt disables itself on his way out.
void routine_ambassador() {
  if (game_state.ambassador_leave_counter > 2 &&
      obj_in(O_AMBASSADOR, current_room)) {
    obj_remove(O_AMBASSADOR);
    obj_remove(O_CELERY);
    if (current_room == R_DECK_NINE) {
      tellf("\nThe ambassador grunts a polite farewell, and disappears up the "
            "gangway,\n"
            "leaving a trail of dripping slime.\n");
    }
    dequeue_event(EVT_AMBASSADOR);
  } else if (obj_in(O_AMBASSADOR, R_DECK_NINE)) {
    game_state.ambassador_leave_counter++;
    if (current_room == R_DECK_NINE) {
      tellf("\nThe ambassador %s\n",
            ambassador_quotes[rand() % NUM_AMBASSADOR_QUOTES]);
    }
  } else if (current_room == R_DECK_NINE) {
    if (!obj_in(O_AMBASSADOR, current_room) &&
        !obj_in(O_BLATHER, current_room) && game_state.blowup_counter == 0 &&
        (rand() % 100) < 15) {
      obj_move(O_AMBASSADOR, current_room);
      obj_move(O_CELERY, current_room);
      obj_move(O_SLIME, current_room);
      // He hands the brochure straight to you.
      obj_move(O_BROCHURE, player);
      tellf("\nThe alien ambassador from the planet Blow'k-bibben-Gordo ambles "
            "toward you\n"
            "from down the corridor. He is munching on something resembling an "
            "enormous\n"
            "stalk of celery, and he leaves a trail of green slime on the deck. "
            "He stops\n"
            "nearby, and you wince as a pool of slime begins forming beneath "
            "him on your\n"
            "newly-polished deck. The ambassador wheezes loudly and hands you a "
            "brochure\n"
            "outlining his planet's major exports.\n");
    }
  }
}

void routine_pod_trip() {
  game_state.trip_counter++;

  // Based on globals.zil I-POD-TRIP
  if (game_state.trip_counter == 1) {
    tellf("\nAs the escape pod tumbles away from the former location of the "
          "Feinstein, its\n"
          "gyroscopes whine. The pod slowly stops tumbling. Lights on the "
          "control panel\n"
          "blink furiously as the autopilot searches for a reasonable "
          "destination.\n");
  } else if (game_state.trip_counter == 2) {
    tellf("\nThe auxiliary rockets fire briefly, and a nearby planet swings "
          "into view\n"
          "through the port. It appears to be almost entirely ocean, with just "
          "a few\n"
          "visible islands and an unusually small polar ice cap. A moment "
          "later, the\n"
          "system's sun swings into view, and the viewport polarizes into a "
          "featureless\n"
          "black rectangle.\n");
  } else if (game_state.trip_counter == 3) {
    tellf(
        "\nThe main thrusters fire a long, gentle burst. A monotonic voice "
        "issues\n"
        "from the control panel. \"Approaching planet...human-habitable.\"\n");
  } else if (game_state.trip_counter == 7) {
    tellf("\nThe pod is buffeted as it enters the planet's atmosphere.\n");
  } else if (game_state.trip_counter == 8) {
    tellf("\nYou feel the temperature begin to rise, and the pod's climate\n"
          "control system roars as it labors to compensate.\n");
  } else if (game_state.trip_counter == 9) {
    tellf(
        "\nThe viewport suddenly becomes transparent again, giving you a view "
        "of\n"
        "endless ocean below. The lights on the control panel flash madly as\n"
        "the pod's computer searches for a suitable landing site. The "
        "thrusters fire\n"
        "long and hard, slowing the pod's descent.\n");
  } else if (game_state.trip_counter == 10) {
    tellf("\nThe pod is now approaching the closer of a pair of islands. It "
          "appears\n"
          "to be surrounded by sheer cliffs rising from the water, and is "
          "topped by\n"
          "a wide plateau. The plateau seems to be covered by a sprawling "
          "complex\n"
          "of buildings.\n");
  } else if (game_state.trip_counter == 11) {
    // Landing Logic
    if (obj_in(player, O_SAFETY_WEB)) {
      tellf(
          "\nThe pod lands with a thud. Through the viewport you can see a "
          "rocky cleft\n"
          "and some water below. The pod rocks gently back and forth as if it "
          "was\n"
          "precariously balanced. A previously unseen panel slides open, "
          "revealing\n"
          "some emergency provisions, including a survival kit and a towel.\n");

      // Trigger provisions
      obj_move(O_TOWEL, R_ESCAPE_POD);
      obj_move(O_FOOD_KIT, R_ESCAPE_POD);

      // POD-EXIT-F's post-landing branch: the pod is wedged above open water,
      // so OUT and UP put you in it and EAST is no longer anywhere.
      objects[R_ESCAPE_POD].out = R_UNDERWATER;
      objects[R_ESCAPE_POD].up = R_UNDERWATER;
      objects[R_ESCAPE_POD].east = NOTHING;

      // Move the visible pod object to the Crag so it can be seen from outside
      obj_move(O_GLOBAL_POD, R_CRAG);

      // TRIP-COUNTER is parked at 15, which is what SAFETY-WEB-F and POD-DOOR-F
      // test to know the pod is down and the water is waiting.
      game_state.trip_counter = 15;
      dequeue_event(EVT_POD_TRIP);

    } else {
      jigs_up("\nThe pod, whose automated controls were unfortunately designed "
              "by computer\n"
              "scientists, lands with a good deal of force. Your body sails "
              "across the pod\n"
              "until it is stopped by one of the sharper corners of the "
              "control panel.");
    }
  }
}

// I-SINK-POD (globals.zil). Once you climb out of the webbing the pod slips off
// its ledge; you have a handful of turns to get out before it crushes or floods.
void routine_sink_pod() {
  game_state.sink_counter++;

  if (current_room != R_ESCAPE_POD)
    return;

  if (game_state.sink_counter == 3) {
    tellf("\nThe pod is now completely submerged, and you feel it smash against "
          "underwater\n"
          "rocks. Bubbles streaming upward past the window indicate that the "
          "pod is\n"
          "continuing to sink.\n");
  } else if (game_state.sink_counter == 4 &&
             !obj_has_flag(O_POD_DOOR, F_OPENBIT)) {
    tellf("\nThe pod creaks ominously from the increasing pressure.\n");
  } else if (game_state.sink_counter == 5) {
    if (obj_has_flag(O_POD_DOOR, F_OPENBIT)) {
      jigs_up("\nBetween the swirling waters and the increasing pressure, it's "
              "curtains\n"
              "for you. Perhaps you should have left the pod a bit sooner.");
    } else {
      jigs_up("\nThe pod splits open, and water pours in.");
    }
  }
}

void routine_hunger() {
  game_state.hunger_level++;
  int h = game_state.hunger_level;

  if (h == 200) {
    tellf("You are beginning to feel a bit hungry.\n");
  } else if (h == 300) {
    tellf("You are getting pretty hungry.\n");
  } else if (h == 400) {
    tellf("You are famished.\n");
  } else if (h == 500) {
    tellf("You are starting to faint from lack of food.\n");
  } else if (h == 600) {
    jigs_up("\nYou have starved to death.");
  }
}
