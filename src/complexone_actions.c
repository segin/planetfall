#include "complexone_actions.h"
#include "complexone.h" // For getting objects if needed directly? or just ids.h
#include "feinstein.h"  // For global flags if any
#include "planetfall.h" // For game state
#include <stdio.h>

// Helper macros for ZIL translation
// Helper macros for ZIL translation (macros moved to output.h)
#define EQUAL(a, b) ((a) == (b))
#define IN(obj, room) obj_in((obj), (room))
#define FSET(obj, flag) obj_has_flag((obj), (flag))
#define FCLEAR(obj, flag) obj_clear_flag((obj), (flag))
#define MOVE(obj, dest) obj_move((obj), (dest))
#define REMOVE(obj) obj_remove((obj))
#define VERB_IS(v)                                                             \
  (current_verb == (v)) // Need current_verb global or passed arg
// Usually routines take RARG (context). In C, we might access global context.

// Placeholder for current verb check - needs robust parser integration
extern int current_verb; // Assume this exists in parser
// #define VERB_LOOK (ACTION_LOOK) (Need action IDs)

// --- Room Actions ---

bool underwater_f(int arg) {
  if (arg == M_END) {
    game_state.drown_counter++;
    if (game_state.drown_counter > 2) {
      jigs_up(
          "A mighty undertow drags you across some underwater obstructions.");
      return true;
    }
  }
  return false;
}

bool crag_f(int arg) {
  if (arg == M_ENTER) {
    game_state.drown_counter = 3;
  }
  return false;
}

bool balcony_f(int arg) {
  if (arg == M_LOOK) {
    TELL("This is an octagonal room, half carved into and half built out from "
         "the cliff\n"
         "wall. Through the shattered windows which ring the outer wall you "
         "can see\n"
         "ocean to the horizon. A weathered metal plaque with barely readable "
         "lettering\n"
         "rests below the windows. The language seems to be a corrupt form of\n"
         "Galalingua. A steep stairway, roughly cut into the face of the "
         "cliff, leads\n"
         "upward. ");
    if (game_state.day == 1) {
      TELL("A rocky crag can be seen about eight meters below.\n");
    } else if (game_state.day == 2) {
      TELL("The ocean waters swirl below. The crag where you landed yesterday "
           "is\n"
           "now underwater!\n");
    } else if (game_state.day >= 3) { // EQUAL? DAY 3 (or greater typically in
                                      // ZIL logic fallthrough)
      TELL("Ocean waters are lapping at the base of the balcony.\n");
    }
    return true; // We handled the description
  }
  return false;
}

bool winding_stair_f(int arg) {
  if (arg == M_LOOK) {
    TELL("The middle of a long, steep stairway carved into the face of a "
         "cliff.");
    if (game_state.day == 4) {
      TELL(" You hear the lapping of water from below.");
    } else if (game_state.day == 5) { // or greater? ZIL says EQUAL? DAY 5
      TELL(" You can see ocean water splashing against the steps below you.");
    }
    CR;
    return true;
  }
  return false;
}

bool courtyard_f(int arg) {
  if (arg == M_LOOK) {
    TELL("You are in the courtyard of an ancient stone edifice, vaguely "
         "reminiscent of\n"
         "the castles you saw during your leave on Ramos Two. It has decayed "
         "to\n"
         "the point where it can probably be termed a ruin. Openings lead "
         "north and west,\n"
         "and a stairway downward is visible to the south. ");
    if (game_state.day == 6 || game_state.day == 7) {
      TELL("From the direction of the stairway comes the sound of ocean surf.");
    } else if (game_state.day >= 8) {
      TELL("Ocean water washes against the top few steps.");
    }
    CR;
    return true;
  }
  return false;
}

ZObjectID water_level_f(void) {
  if (current_room == R_BALCONY) {
    return (game_state.day == 1) ? R_CRAG : R_UNDERWATER;
  }
  if (current_room == R_WINDING_STAIR) {
    return (game_state.day < 4) ? R_BALCONY : R_UNDERWATER;
  }
  if (current_room == R_COURTYARD) {
    return (game_state.day < 6) ? R_WINDING_STAIR : R_UNDERWATER;
  }
  return NOTHING;
}

bool structure_pseudo_action(int verb) {
  if (verb == V_EXAMINE) {
    tellf("You'd be able to tell more about it if you climbed up to it.\n");
    return true;
  }
  if (verb == V_CLIMB_UP || verb == V_CLIMB_FOO || verb == V_CLIMB_ON) {
    perform_walk(objects[current_room].up);
    return true;
  }
  return false;
}

bool cleft_pseudo_action(int verb) {
  if (verb == V_CLIMB_UP || verb == V_CLIMB_FOO || verb == V_CLIMB_ON) {
    perform_walk(objects[current_room].up);
    return true;
  }
  return false;
}

bool plaque_pseudo_action(int verb) {
  if (verb == V_READ || verb == V_EXAMINE) {
    tellf("\nSEENIK VISTA\n\n"
          "Xis stuneeng vuu uf xee Kalamontee Valee kuvurz oovur fortee skwaar miilz\n"
          "uf xat faamus tuurist spot. Xee larj bildeeng at xee bend in xee Gulmaan Rivur\n"
          "iz xee formur pravincul kapitul bildeeng.\n");
    return true;
  }
  return false;
}

bool castle_pseudo_action(int verb) {
  if (verb == V_EXAMINE) {
    tellf("The castle is ancient and crumbling.\n");
    return true;
  }
  return false;
}

bool rubble_pseudo_action(int verb) {
  if (verb == V_MOVE) {
    perform_slide();
    return true;
  }
  if (verb == V_EXAMINE) {
    tellf("Don't be silly.\n");
    return true;
  }
  return false;
}

bool games_pseudo_action(int verb) {
  if (verb == V_EXAMINE) {
    tellf("All the usual games -- Chess, Cribbage, Galactic\n"
          "Overlord, Double Fannucci...\n");
    return true;
  }
  if (verb == V_PLAY) {
    tellf("You fiddle with the games for a while, but there's nobody to play against.\n");
    return true;
  }
  return false;
}

bool tapes_pseudo_action(int verb) {
  if (verb == V_READ || verb == V_PLAY || verb == V_TAKE) {
    tellf("Hardly the time or place for reading recreational tapes.\n");
    return true;
  }
  if (verb == V_EXAMINE) {
    tellf("Let's see...here are some musical selections, here are\n"
          "some bestselling romantic novels, here is a biography of a famous Double\n"
          "Fannucci champion...\n");
    return true;
  }
  return false;
}

bool near_booth_pseudo_action(int verb) {
  if (verb == V_DROP || verb == V_EXIT || verb == V_DISEMBARK) {
    tellf("You're not in the booth!\n");
    return true;
  }
  if (verb == V_THROUGH || verb == V_BOARD || verb == V_WALK_TO || verb == V_ENTER) {
    perform_walk(objects[current_room].in);
    return true;
  }
  return false;
}

bool in_booth_pseudo_action(int verb) {
  if (verb == V_THROUGH || verb == V_BOARD || verb == V_WALK_TO || verb == V_ENTER) {
    tellf("You're already in the booth!\n");
    return true;
  }
  if (verb == V_DROP || verb == V_EXIT || verb == V_DISEMBARK) {
    perform_walk(objects[current_room].out);
    return true;
  }
  return false;
}

bool partition_pseudo_action(int verb) {
  if (verb == V_EXAMINE) {
    tellf("The partitions are very plain, and were\n"
          "obviously intended to separate this huge room into smaller areas.\n");
    return true;
  }
  return false;
}

bool toilet_pseudo_action(int verb) {
  if (verb == V_EXAMINE) {
    tellf("The fixtures are all dry and dusty.\n");
    return true;
  }
  if (verb == V_FLUSH) {
    tellf("The water seems to be turned off.\n");
    return true;
  }
  return false;
}

#include "parser.h"

// Helper macros
// Moved into output.h

// --- Room Actions ---

// ... (Previous implementations) ...

bool rec_area_f(int arg) {
  if (arg == M_LOOK) {
    TELL("This is a recreational facility of some sort. Games and tapes are "
         "scattered\n"
         "about the room. Hallways head off to the east and south, and to the "
         "north is\n"
         "a door which is ");
    if (obj_has_flag(O_CONFERENCE_DOOR, F_OPENBIT)) {
      TELL("open");
    } else {
      TELL("closed and locked. A dial on the door is currently set to %d",
           game_state.dial_number);
    }
    TELL(".\n");
    return true;
  }
  return false;
}

bool conference_room_f(int arg) {
  if (arg == M_LOOK) {
    TELL("This is a fairly square room, almost filled by a round conference "
         "table.\n"
         "To the south is a door which is ");
    // DDESC ,CONFERENCE-DOOR
    if (obj_has_flag(O_CONFERENCE_DOOR, F_OPENBIT)) {
      TELL("open");
    } else {
      TELL("closed");
    }
    TELL(". To the north is a small room about the size of a phone booth.\n");
    return true;
  }
  return false;
}

// --- Object Actions ---

bool combination_dial_f(int arg) {
  if (current_cmd.verb == V_EXAMINE) {
    TELL("The dial can be turned to any number between 0 and 1000.\n");
    return true;
  }
  if (current_cmd.verb == V_SET && current_cmd.prsi == O_INTNUM) {
    if (obj_has_flag(O_COMBINATION_DIAL, F_MUNGEDBIT)) { // Fused
      TELL("The dial has somehow become fused and won't move.\n");
      return true;
    }
    if (current_cmd.parsed_number == game_state.dial_number) {
      TELL("That's what the dial is set to now!\n");
      return true;
    }
    if (current_cmd.parsed_number ==
        game_state.number_needed) { // Logic: if set to correct number
      game_state.dial_number = 0;
      obj_set_flag(O_CONFERENCE_DOOR, F_OPENBIT);
      TELL("The door swings open, and the dial resets to 0.\n");
      return true;
    }
    if (current_cmd.parsed_number > 1000) {
      TELL("The dial cannot be turned to a number that high.\n");
      return true;
    }
    game_state.dial_number = current_cmd.parsed_number;
    TELL("The dial is now set to %d.\n", current_cmd.parsed_number);
    return true;
  }
  return false;
}

bool conference_door_f(int arg) {
  if (current_cmd.verb == V_OPEN) {
    if (obj_has_flag(O_CONFERENCE_DOOR, F_OPENBIT)) {
      TELL("It is already open.\n");
      return true;
    }
    if (current_room == R_REC_AREA) {
      TELL("The door is locked. You probably have to turn the dial to some "
           "number to\n"
           "open it.\n");
    } else {
      TELL("The door seems to be locked from the other side.\n");
    }
    return true;
  }
  if (current_cmd.verb == V_CLOSE) {
    if (obj_has_flag(O_CONFERENCE_DOOR, F_OPENBIT)) {
      obj_clear_flag(O_CONFERENCE_DOOR, F_OPENBIT);
      TELL("The door closes and you hear a click as it locks.\n");
      return true;
    }
    TELL("It is already closed.\n");
    return true;
  }
  return false;
}

bool mess_corridor_f(int arg) {
  if (arg == M_LOOK) {
    TELL("This is a wide, east-west hallway with a large portal to the south. "
         "A\n"
         "small door to the north is ");
    // DDESC STORAGE-WEST-DOOR
    if (obj_has_flag(O_STORAGE_WEST_DOOR, F_OPENBIT)) {
      TELL("open");
    } else {
      TELL("closed");
    }
    if (!game_state.padlock_removed) {
      TELL(" and hooked with a simple steel padlock");
      if (obj_has_flag(O_PADLOCK, F_OPENBIT)) {
        TELL(" which hangs unlocked");
      } else {
        TELL(" which is also closed");
      }
    }
    TELL(".\n");
    return true;
  }
  return false;
}

bool storage_west_door_f(int arg) { // Add this signature to header if not there
  if (current_cmd.verb == V_OPEN) {
    if (obj_has_flag(O_STORAGE_WEST_DOOR, F_OPENBIT)) {
      TELL("It is already open.\n");
      return true;
    }
    if (game_state.padlock_removed) {
      obj_set_flag(O_STORAGE_WEST_DOOR, F_OPENBIT);
      TELL("Opened.\n");
    } else {
      TELL("The door cannot be opened until the padlock is removed.\n");
    }
    return true;
  }
  if (current_cmd.verb == V_CLOSE) {
    if (obj_has_flag(O_STORAGE_WEST_DOOR, F_OPENBIT)) {
      obj_clear_flag(O_STORAGE_WEST_DOOR, F_OPENBIT);
      TELL("The door is now closed.\n");
    } else {
      TELL("It is already closed.\n");
    }
    return true;
  }
  if (current_cmd.verb == V_UNLOCK) {
    TELL("The door itself isn't locked.");
    if (!obj_has_flag(O_PADLOCK, F_OPENBIT)) {
      TELL(" It is the padlock on the door which is locked.");
    }
    CR;
    return true;
  }
  return false;
}

bool padlock_f(int arg) {
  if (current_room == R_BRIG) { // Is BRIG definition available? R_BRIG in ids.h
    TELL("You can't see or reach the lock from inside the cell.\n");
    return true;
  }
  // OPEN-WITH
  if (current_cmd.verb == V_OPEN_WITH &&
      current_cmd.prso_list[0] == O_PADLOCK) {
    // Perform UNLOCK with PRSI
    // dispatch_action(V_UNLOCK, O_PADLOCK, current_cmd.prsi);
    // Simplified: fallthrough to UNLOCK logic?
    current_cmd.verb = V_UNLOCK; // Hack? Or call logic
                                 // Let's copy logic:
  }
  if (current_cmd.verb == V_UNLOCK || current_cmd.verb == V_OPEN) {
    if (!obj_has_flag(O_PADLOCK, F_OPENBIT)) {
      if (current_cmd.prsi == NOTHING) { // No tool
        TELL("You can't open it with your hands.\n");
      } else if (current_cmd.prsi == O_KEY) {
        if (obj_has_flag(O_PADLOCK, F_MUNGEDBIT)) { // Fused?
          TELL("Tsk, tsk... the padlock seems to be fused shut.\n");
        } else {
          obj_set_flag(O_PADLOCK, F_OPENBIT);
          TELL("The padlock springs open.\n");
        }
      } else {
        TELL("That doesn't work.\n");
      }
    } else {
      TELL("The padlock is already unlocked.\n");
    }
    return true;
  }
  if (current_cmd.verb == V_CLOSE || current_cmd.verb == V_LOCK) {
    if (obj_has_flag(O_PADLOCK, F_OPENBIT)) {
      obj_clear_flag(O_PADLOCK, F_OPENBIT);
      TELL("The padlock closes with a sharp click.\n");
    } else {
      TELL("The padlock is already locked.\n");
    }
    return true;
  }
  if (current_cmd.verb == V_TAKE && !game_state.padlock_removed) {
    if (obj_has_flag(O_PADLOCK, F_OPENBIT)) {
      game_state.padlock_removed = true;
      obj_clear_flag(O_PADLOCK, F_TRYTAKEBIT);
      obj_clear_flag(O_PADLOCK, F_NDESCBIT);
      return false; // Allow normal TAKE to proceed now?
      // ZIL says RFALSE, which usually means "action not handled, perform
      // default". Default take will succeed if TRYTAKE bit is cleared.
    }
    if (!obj_has_flag(O_PADLOCK, F_OPENBIT)) {
      TELL("The padlock is locked to the door.\n");
      return true; // Stop take
    }
  }
  if (current_cmd.verb == V_MUNG) {
    TELL(
        "And, as we go into the next round, it's Padlock 1, Adventurer 0...\n");
    return true;
  }
  return false;
}

bool machine_shop_f(int arg) {
  if (arg == M_LOOK) {
    TELL("This room is probably some sort of machine shop filled with a "
         "variety\n"
         "of unusual machines. Doorways lead north, east, and west.\n\n"
         "Standing against the rear wall is a large dispensing machine with a\n"
         "spout. ");
    if (game_state.spout_placed != O_GROUND &&
        game_state.spout_placed != NOTHING) {
      TELL("Sitting under the spout is a object. "); // Simplified
    }
    TELL(
        "The dispenser is lined with brightly-colored buttons. The first four\n"
        "buttons, labelled \"KUULINTS 1 - 4\", are colored red, blue, green, "
        "and\n"
        "yellow. The next three buttons, labelled \"KATALISTS 1 - 3\", are "
        "colored\n"
        "gray, brown, and black. The last two buttons are both white. One of\n"
        "these is square and says \"BAAS.\" The other white button is round "
        "and\n"
        "says \"ASID.\"\n");
    return true;
  }
  return false;
}

bool chemical_dispenser_f(int arg) {
  if (current_cmd.verb == V_PUT_UNDER &&
      current_cmd.prsi == O_CHEMICAL_DISPENSER) {
    if (game_state.spout_placed == O_GROUND ||
        game_state.spout_placed == NOTHING) {
      game_state.spout_placed = current_cmd.prso_list[0];
      TELL("The object is now sitting under the spout.\n");
    } else {
      TELL("The other object is already resting under the spout.\n");
    }
    return true;
  }
  return false;
}

const char *get_color_name(int color_idx) {
  switch (color_idx) {
  case 1:
    return "red";
  case 2:
    return "blue";
  case 3:
    return "green";
  case 4:
    return "yellow";
  case 5:
    return "gray";
  case 6:
    return "brown";
  case 7:
    return "black";
  default:
    return "clear";
  }
}

bool chem_button_f(int arg) {
  if (current_cmd.verb == V_PUSH) {
    if (obj_has_flag(O_CHEMICAL_DISPENSER, F_MUNGEDBIT)) {
      TELL("The machine coughs a few times, but nothing else happens.\n");
      return true;
    }

    ZObjectID button = current_cmd.prso_list[0];
    int color_val = 0;
    if (button == O_RED_BUTTON)
      color_val = 1;
    else if (button == O_BLUE_BUTTON)
      color_val = 2;
    else if (button == O_GREEN_BUTTON)
      color_val = 3;
    else if (button == O_YELLOW_BUTTON)
      color_val = 4;
    else if (button == O_GRAY_BUTTON)
      color_val = 5;
    else if (button == O_BROWN_BUTTON)
      color_val = 6;
    else if (button == O_BLACK_BUTTON)
      color_val = 7;
    else if (button == O_ROUND_WHITE_BUTTON)
      color_val = 8;
    else if (button == O_SQUARE_WHITE_BUTTON)
      color_val = 9;

    if (game_state.spout_placed == O_FLASK) {
      if (obj_in(O_CHEMICAL_FLUID, O_FLASK)) {
        TELL("Another dose of the chemical fluid pours out of the spout, "
             "splashes over\n"
             "the already-full flask, spills onto the floor, and dries up.\n");
      } else {
        obj_move(O_CHEMICAL_FLUID, O_FLASK);
        TELL("The flask fills with some %s chemical fluid. The fluid gradually "
             "turns milky white.\n",
             get_color_name(color_val));
        game_state.chemical_flag = color_val;
      }
    } else if (game_state.spout_placed == O_CANTEEN &&
               obj_has_flag(O_CANTEEN, F_OPENBIT)) {
      TELL("Chemical fluid gushes from the spout. Unfortunately, the mouth of "
           "the canteen\n"
           "is very narrow, and the fluid just splashes over it.\n");
    } else {
      TELL("Some sort of chemical fluid pours out of the spout, spills all "
           "over the object, and dries up.\n");
    }
    return true;
  }
  return false;
}

// --- Admin Corridor & Crevice ---

bool admin_corridor_f(int arg) {
  if (arg == M_ENTER && game_state.ladder_flag) {
    obj_move(O_LADDER, current_room);
  }
  if (arg == M_LOOK) {
    TELL("The hallway, in fact the entire building, has been rent apart here, "
         "presumably\n"
         "by seismic upheaval. You can see the sky through the severed roof "
         "above, and\n"
         "the ground is thick with rubble. To the north is a gaping rift, at "
         "least eight\n"
         "meters across and thirty meters deep. ");
    if (game_state.ladder_flag) {
      TELL("A metal ladder spans the rift. ");
    }
    TELL("A wide doorway, labelled \"Sistumz Moniturz,\" leads west.\n");
    return true;
  }
  return false;
}

bool admin_corridor_n_f(int arg) {
  if (arg == M_ENTER && game_state.ladder_flag) {
    obj_move(O_LADDER, current_room);
  }
  if (arg == M_LOOK) {
    TELL("The corridor ends here. Portals lead west, north, and east. Signs "
         "above these\n"
         "portals read, respectively, \"Administraativ Awfisiz,\" "
         "\"Tranzportaashun\n"
         "Suplii,\" and \"Plan Ruum.\" To the south is a wide rift");
    if (game_state.ladder_flag) {
      TELL(", spanned by a metal ladder,");
    }
    TELL(" separating this area from the rest of the building.\n");
    return true;
  }
  return false;
}

bool admin_corridor_s_f(int arg) {
  if (arg == M_END && obj_has_flag(O_KEY, F_INVISIBLE)) {
    // PROB 20 logic
    // if (rand() % 5 == 0) // Basic prob check
    TELL("You catch, out of the corner of your eye, a glint of light from the "
         "direction\n"
         "of the floor.\n");
    return true;
  }
  return false;
}

bool crevice_f(int arg) {
  if (current_cmd.verb == V_REACH) {
    TELL("The crevice is too narrow to reach into.\n");
    return true;
  }
  if (current_cmd.verb == V_LOOK_INSIDE || current_cmd.verb == V_EXAMINE ||
      current_cmd.verb == V_SEARCH) {
    if (obj_has_flag(O_KEY, F_TOUCHBIT)) {
      TELL("Nothing there but bunches of dust.\n");
    } else {
      obj_clear_flag(O_KEY, F_INVISIBLE);
      TELL("Lying at the bottom of the narrow crack, partly covered by layers "
           "of dust,\n"
           "is a shiny steel key!\n");
    }
    return true;
  }
  return false;
}

bool key_f(int arg) {
  if ((current_cmd.verb == V_TAKE || current_cmd.verb == V_MOVE) &&
      !obj_has_flag(O_KEY, F_TOUCHBIT)) {
    if (current_cmd.prsi ==
        O_MAGNET) { // Assuming magnet implemented later, but logic is here
      // PERFORM ATTRACT MAGNET KEY
      TELL("You use the magnet to attract the key.\n"); // Placeholder for
                                                        // perform call
      obj_move(O_KEY, O_MAGNET);                        // Simplify
      obj_set_flag(O_KEY, F_TOUCHBIT);
      return true;
    }
    if (current_cmd.prsi != NOTHING) {
      TELL("Nice try.\n");
      return true;
    }
    TELL("Either the crevice is too narrow, or your fingers are too large.\n");
    return true;
  }
  return false;
}

// --- Elevator Actions ---

bool elevator_exit_f(void); // Forward decl

bool elevator_lobby_f(int arg) {
  if (arg == M_LOOK) {
    TELL("This is a wide, brightly lit lobby. A blue metal door to the north "
         "is ");
    if (obj_has_flag(O_UPPER_ELEVATOR_DOOR, F_OPENBIT) &&
        !game_state.upper_elevator_up) {
      TELL("open");
    } else {
      TELL("closed");
    }
    TELL(" and a larger red metal door to the south is ");
    if (obj_has_flag(O_LOWER_ELEVATOR_DOOR, F_OPENBIT) &&
        game_state.lower_elevator_up) {
      if (obj_has_flag(O_UPPER_ELEVATOR_DOOR, F_OPENBIT) &&
          !game_state.upper_elevator_up) {
        TELL("also ");
      }
      TELL("open");
    } else {
      if (!obj_has_flag(O_UPPER_ELEVATOR_DOOR, F_OPENBIT) ||
          game_state.upper_elevator_up) {
        TELL("also ");
      }
      TELL("closed");
    }
    TELL(". Beside the blue door is a blue button, and beside the red door is\n"
         "a red button. A corridor leads west. To the east is a small room\n"
         "about the size of a telephone booth.\n");
    return true;
  }

  // Handle movement (PER ELEVATOR-ENTER-F)
  if (arg == M_BEG && current_cmd.verb == V_WALK) {
    if (current_cmd.prso_list[0] == O_NORTH) {
      // Logic for entering upper elevator
      if (obj_has_flag(O_UPPER_ELEVATOR_DOOR, F_OPENBIT) &&
          !game_state.upper_elevator_up) {
        TELL("You enter the elevator.\n");
        obj_move(player, R_UPPER_ELEVATOR);
        return true;
      } else {
        TELL("The door is closed.\n");
        return true;
      }
    }
    if (current_cmd.prso_list[0] == O_SOUTH) {
      // Logic for entering lower elevator
      if (obj_has_flag(O_LOWER_ELEVATOR_DOOR, F_OPENBIT) &&
          game_state.lower_elevator_up) {
        TELL("You enter the elevator.\n");
        obj_move(player, R_LOWER_ELEVATOR);
        return true;
      } else {
        TELL("The door is closed.\n");
        return true;
      }
    }
  }

  return false;
}

bool elevator_exit_f(void) {
  bool open = false;

  if (current_room == R_UPPER_ELEVATOR) {
    if (obj_has_flag(O_UPPER_ELEVATOR_DOOR, F_OPENBIT) &&
        !game_state.upper_elevator_up)
      open = true;
  } else if (current_room == R_LOWER_ELEVATOR) {
    if (obj_has_flag(O_LOWER_ELEVATOR_DOOR, F_OPENBIT) &&
        game_state.lower_elevator_up)
      open = true;
  }

  if (open) {
    obj_move(player, R_ELEVATOR_LOBBY);
    TELL("You leave the elevator.\n");
    return true;
  } else {
    TELL("The doors are closed.\n");
    return true;
  }
}

bool upper_elevator_f(int arg) {
  if (arg == M_LOOK) {
    TELL("You are in the upper elevator. The door ");
    if (obj_has_flag(O_UPPER_ELEVATOR_DOOR, F_OPENBIT)) {
      TELL("is open.\n");
    } else {
      TELL("is closed.\n");
    }
    return true;
  }
  if (arg == M_BEG && current_cmd.verb == V_WALK) {
    if (current_cmd.prso_list[0] == O_SOUTH ||
        current_cmd.prso_list[0] == O_OUT) {
      elevator_exit_f();
      return true;
    }
  }
  return false;
}

bool lower_elevator_f(int arg) {
  if (arg == M_LOOK) {
    TELL("You are in the lower elevator. The door ");
    if (obj_has_flag(O_LOWER_ELEVATOR_DOOR, F_OPENBIT)) {
      TELL("is open.\n");
    } else {
      TELL("is closed.\n");
    }
    return true;
  }
  if (arg == M_BEG && current_cmd.verb == V_WALK) {
    if (current_cmd.prso_list[0] == O_NORTH ||
        current_cmd.prso_list[0] == O_OUT) {
      elevator_exit_f();
      return true;
    }
  }
  return false;
}

// --- Floyd ---

bool floyd_f(int arg) {
  if (current_cmd.verb == V_EXAMINE) {
    TELL("From its design, the robot seems to be of the multi-purpose sort. It "
         "is\n"
         "slightly cross-eyed, and its mechanical mouth forms a lopsided "
         "grin.\n");
    // TODO: check if broken/off/active
    return true;
  }
  if (current_cmd.verb == V_LAMP_ON) { // Activate Floyd
    // if not already active
    if (!obj_has_flag(O_FLOYD, F_ACTORBIT)) {
      obj_set_flag(O_FLOYD, F_ACTORBIT);
      obj_set_flag(O_FLOYD,
                   F_RLANDBIT); // Uses RLANDBIT as generic 'on'? ZIL says FLAGS
                                // RLANDBIT means active here?
      // ZIL: <ROUTINE FLOYD-COMES-ALIVE ... <FSET ,FLOYD ,RLANDBIT> ...>
      TELL("Suddenly, the robot comes to life and its head starts swivelling "
           "about.\n"
           "It notices you and bounds over. \"Hi! I'm B-19-7, but to "
           "everyperson I'm\n"
           "called Floyd. Are you a doctor-person or a planner-person? Let's "
           "play\n"
           "Hider-and-Seeker you with me.\"\n");
      // Add event I-FLOYD
    } else {
      TELL("He's already been activated.\n");
    }
    return true;
  }
  // TODO: More floyd logic
  return false;
}

// --- Systems Monitors ---

void describe_monitors() {
  TELL("The far wall is filled with a number of monitors. Of these,\n"
       "the ones labelled ");
  if (game_state.defense_fixed)
    TELL("PLANATEREE DEFENS, ");
  if (game_state.course_control_fixed)
    TELL("PLANATEREE KORS KUNTROOL, ");
  if (game_state.comm_fixed)
    TELL("KUMUUNIKAASHUNZ, ");

  TELL("LIIBREREE, REEAKTURZ, and LIIF SUPORT are green, but the one");

  bool all_fixed = game_state.defense_fixed &&
                   game_state.course_control_fixed && game_state.comm_fixed;
  if (!all_fixed)
    TELL("s");

  TELL(" labelled ");
  if (!game_state.defense_fixed)
    TELL("PLANATEREE DEFENS, ");
  if (!game_state.course_control_fixed)
    TELL("PLANATEREE KORS KUNTROOL, ");
  if (!game_state.comm_fixed)
    TELL("KUMUUNIKAASHUNZ, ");

  if (!all_fixed)
    TELL("and ");
  TELL("PRAJEKT KUNTROOL indicate");
  if (all_fixed)
    TELL("s");
  TELL(" a malfunctioning condition.\n");
}

bool systems_monitors_f(int arg) {
  if (arg == M_LOOK) {
    TELL("This is a large room filled with tables full of strange equipment. ");
    describe_monitors();
    return true;
  }
  return false;
}

// --- Mess Hall / Dispenser ---

bool mess_hall_f(int arg) { return false; }

bool dispenser_f(int arg) { // Protein dispenser
  if (current_cmd.verb == V_EXAMINE) {
    TELL(
        "This wall-mounted unit contains an octagonal niche beneath a spout. ");
    // Check "IN" relation for dispenser. Assuming O_DISPENSER is defined in
    // ids.h
    if (obj_in(O_CANTEEN, O_DISPENSER)) {
      TELL("A canteen is resting in the niche, its mouth lying just below the "
           "spout. ");
    }
    TELL("Above the spout is a button. The machine is labelled \"Hii Prooteen "
         "Likwid\n"
         "Dispensur.\"\n");
    return true;
  }
  if (current_cmd.verb == V_PUT && current_cmd.prso_list[0] == O_CANTEEN) {
    // MOVE CANTEEN DISPENSER
    obj_move(O_CANTEEN, O_DISPENSER);
    TELL("The canteen fits snugly into the octagonal niche,\n"
         "its mouth resting just below the spout of the machine.\n");
    return true;
  }
  if (current_cmd.verb == V_PUT && current_cmd.prso_list[0] != O_CANTEEN &&
      current_cmd.prsi == O_DISPENSER) {
    TELL("It doesn't fit in the niche.\n");
    return true;
  }
  return false;
}

bool high_protein_f(int arg) {
  if (current_cmd.verb == V_EAT) {
    if (!obj_in(O_CANTEEN, player)) {
      TELL("You are not holding the canteen.\n");
      return true;
    }
    if (game_state.hunger_level == 0) {
      TELL("You are not hungry.\n");
      return true;
    }
    obj_remove(O_HIGH_PROTEIN);
    game_state.c_elapsed = 15; // Set elapsed
    game_state.hunger_level = 0;
    TELL("Mmmm....that was good. It certainly quenched your thirst and "
         "satisfied your\n"
         "hunger.\n");
    return true;
  }
  if (current_cmd.verb == V_POUR &&
      current_cmd.prso_list[0] == O_HIGH_PROTEIN) {
    obj_remove(O_HIGH_PROTEIN);
    TELL("The protein-rich fluid pours out and dries up.\n");
    return true;
  }
  return false;
}

bool kitchen_door_f(int arg) { return false; }

bool kalamontee_platform_f(int arg) { return false; }
bool comm_room_f(int arg) { return false; }
