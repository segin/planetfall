#include "complexone_actions.h"
#include "complexone.h"
#include "feinstein.h"
#include "parser.h"
#include "planetfall.h"
#include <stdio.h>
#include <stdlib.h>

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

bool can_f(int verb) {
  if (verb == V_EXAMINE) {
    tellf("This is a rather normal tin can. It is large and is labelled \"Spam\n"
          "and Egz.\"\n");
    return true;
  }
  if (verb == V_OPEN) {
    tellf("You certainly can't open it with your hands, and you don't seem to have\n"
          "found a can opener yet.\n");
    return true;
  }
  return false;
}

bool ladder_f(int verb) {
  if (verb == V_TAKE) {
    if (game_state.ladder_extended) {
      tellf("You can't possibly carry the ladder while it's extended.\n");
      return true;
    }
  }
  if (verb == V_EXAMINE) {
    tellf("It is a heavy-duty ladder built of sturdy aluminum tubing. It is currently ");
    if (game_state.ladder_extended) {
      tellf("extended to its full length of about 8 meters, but could be collapsed to\n"
            "a shorter length for easier carrying.\n");
    } else {
      tellf("collapsed and is around two-and-a-half meters long, but if extended would\n"
            "obviously be much longer.\n");
    }
    return true;
  }
  if (verb == V_OPEN) {
    if (game_state.ladder_extended) {
      tellf("The ladder is already extended.\n");
    } else if (current_room == R_STORAGE_EAST || current_room == R_STORAGE_WEST ||
               current_room == R_BOOTH_2 || current_room == R_UPPER_ELEVATOR ||
               current_room == R_LOWER_ELEVATOR) {
      tellf("You can't extend the ladder in this tiny space!\n");
    } else if (obj_in(O_LADDER, player)) {
      tellf("You couldn't possibly extend the ladder while you're holding it.\n");
    } else {
      obj_set_flag(O_LADDER, F_TRYTAKEBIT);
      game_state.ladder_extended = true;
      game_state.c_elapsed = 36;
      tellf("The ladder extends to a length of around eight meters.\n");
    }
    return true;
  }
  if (verb == V_CLOSE) {
    if (game_state.ladder_extended) {
      game_state.c_elapsed = 21;
      if (game_state.ladder_flag) {
        game_state.ladder_flag = false;
        obj_remove(O_LADDER);
        tellf("As the ladder shortens it plunges into the rift.\n");
      } else {
        game_state.ladder_extended = false;
        obj_clear_flag(O_LADDER, F_TRYTAKEBIT);
        tellf("The ladder collapses to a length of around two-and-a-half meters.\n");
      }
    } else {
      tellf("The ladder is already in its collapsed state.\n");
    }
    return true;
  }
  if (verb == V_SPAN || verb == V_ATTRACT) {
    if (game_state.ladder_flag) {
      tellf("The ladder already spans the rift.\n");
    } else {
      if (game_state.ladder_extended) {
        game_state.ladder_flag = true;
        obj_set_flag(O_LADDER, F_NDESCBIT);
        tellf("The ladder swings out across the rift and comes to rest on the far edge,\n"
              "spanning the precipice.\n");
      } else {
        obj_remove(O_LADDER);
        tellf("The ladder, far too short to reach the other edge of the rift, plunges into\n"
              "the rift and is lost forever.\n");
      }
    }
    return true;
  }
  if (verb == V_CLIMB_UP || verb == V_CLIMB_FOO || verb == V_CLIMB_ON) {
    if (game_state.ladder_flag) {
      tellf("You can't climb a horizontal ladder!\n");
    } else if (obj_in(O_LADDER, player)) {
      tellf("That would be a neat trick, considering that you're holding it.\n");
    }
    return true;
  }
  return false;
}

bool walkway_pseudo_action(int verb) {
  if (verb == V_EXAMINE || verb == V_LAMP_ON) {
    tellf("The walkway, which hastened the trip down that\n"
          "long corridor, is no longer in service.\n");
    return true;
  }
  return false;
}

bool bench_pseudo_action(int verb) {
  if (verb == V_CLIMB_ON || verb == V_BOARD) {
    tellf("The benches look uncomfortable.\n");
    return true;
  }
  return false;
}

bool kitchen_door_f(int verb) {
  if (verb == V_OPEN) {
    tellf("A light flashes \"Pleez yuuz kitcin akses kard.\"\n");
    return true;
  }
  return false;
}

bool dispenser_f(int verb) {
  if (verb == V_EXAMINE) {
    tellf("This wall-mounted unit contains an octagonal niche beneath a spout. ");
    if (obj_in(O_CANTEEN, O_DISPENSER)) {
      tellf("A canteen is resting in the niche, its mouth lying just below the spout. ");
    }
    tellf("Above the spout is a button. The machine is labelled \"Hii Prooteen Likwid\n"
          "Dispensur.\"\n");
    return true;
  }
  if (verb == V_CLOSE) {
    tellf("You can't close that.\n");
    return true;
  }
  if (verb == V_PUT) {
    if (current_cmd.prso_list[0] == O_CANTEEN || current_cmd.prso_count == 0) {
      obj_move(O_CANTEEN, O_DISPENSER);
      tellf("The canteen fits snugly into the octagonal niche,\n"
            "its mouth resting just below the spout of the machine.\n");
      return true;
    } else {
      tellf("It doesn't fit in the niche.\n");
      return true;
    }
  }
  return false;
}

bool canteen_f(int verb) {
  return false;
}

bool high_protein_f(int verb) {
  if (verb == V_EAT) {
    if (!obj_in(O_CANTEEN, player)) {
      tellf("You're not holding the canteen!\n");
      return true;
    }
    if (game_state.hunger_level == 0) {
      tellf("You are not hungry.\n");
      return true;
    }
    obj_remove(O_HIGH_PROTEIN);
    game_state.c_elapsed = 15;
    game_state.hunger_level = 0;
    tellf("Mmmm....that was good. It certainly quenched your thirst and satisfied your\n"
          "hunger.\n");
    return true;
  }
  if (verb == V_POUR) {
    if (!obj_in(O_CANTEEN, player)) {
      tellf("Maybe if you were holding the canteen...\n");
      return true;
    }
    obj_remove(O_HIGH_PROTEIN);
    ZObjectID prsi = current_cmd.prsi;
    const char *target = (prsi != NOTHING) ? objects[prsi].description : "ground";
    tellf("The protein-rich fluid pours over the %s and then dries up.\n", target);
    return true;
  }
  return false;
}

bool kitchen_button_pseudo_action(int verb) {
  if (verb == V_PUSH) {
    if (obj_has_flag(O_DISPENSER, F_MUNGEDBIT)) {
      tellf("The dispenser sputters a few times.\n");
    } else if (obj_in(O_CANTEEN, O_DISPENSER)) {
      if (!obj_has_flag(O_CANTEEN, F_OPENBIT)) {
        tellf("A thick, brown liquid spills over the closed canteen, dribbles down the side\n"
              "of the machine, and forms a puddle on the floor which quickly dries up.\n");
      } else if (obj_in(O_HIGH_PROTEIN, O_CANTEEN)) {
        tellf("The brown liquid splashes over the mouth of the already-filled canteen,\n"
              "creating a mess%s.\n",
              obj_has_flag(O_PATROL_UNIFORM, F_WORNBIT) ? " and staining your uniform" : "");
      } else {
        obj_move(O_HIGH_PROTEIN, O_CANTEEN);
        tellf("The canteen fills almost to the brim with a brown liquid.\n");
      }
    } else {
      tellf("A thick, brownish liquid pours from the spout\n"
            "and splashes to the floor, where it quickly evaporates.\n");
    }
    return true;
  }
  return false;
}

bool spout_pseudo_action(int verb) {
  if (verb == V_PUT || verb == V_PUT_UNDER) {
    if (current_cmd.prso_list[0] == O_CANTEEN || current_cmd.prso_count == 0) {
      obj_move(O_CANTEEN, O_DISPENSER);
      tellf("The canteen fits snugly into the octagonal niche,\n"
            "its mouth resting just below the spout of the machine.\n");
      return true;
    }
  }
  if (verb == V_LOOK_UNDER) {
    if (obj_in(O_CANTEEN, O_DISPENSER)) {
      tellf("The canteen is sitting under the spout.\n");
      return true;
    }
  }
  return false;
}

bool kitchen_f(int arg) {
  if (arg == M_ENTER) {
    score_obj(R_KITCHEN);
    return true;
  }
  return false;
}

ZObjectID long_hall_f(void) {
  tellf("You walk down the long, featureless hallway for a long time. Finally,\n"
        "you see ");
  game_state.c_elapsed = 160;
  if (current_room == R_CORRIDOR_JUNCTION) {
    tellf("some doorways ahead...\n\n");
    return R_DORM_CORRIDOR;
  } else {
    tellf("an intersection ahead...\n\n");
    return R_CORRIDOR_JUNCTION;
  }
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
    tellf("This room is probably some sort of machine shop filled with a variety\n"
          "of unusual machines. Doorways lead north, east, and west.\n\n"
          "Standing against the rear wall is a large dispensing machine with a\n"
          "spout. ");
    if (game_state.spout_placed != O_GROUND && game_state.spout_placed != NOTHING) {
      const char *art = obj_has_flag(game_state.spout_placed, F_VOWELBIT) ? "an " : "a ";
      tellf("Sitting under the spout is %s%s. ", art, objects[game_state.spout_placed].description);
    }
    tellf("The dispenser is lined with brightly-colored buttons. The first four\n"
          "buttons, labelled \"KUULINTS 1 - 4\", are colored red, blue, green, and\n"
          "yellow. The next three buttons, labelled \"KATALISTS 1 - 3\", are colored\n"
          "gray, brown, and black. The last two buttons are both white. One of\n"
          "these is square and says \"BAAS.\" The other white button is round and\n"
          "says \"ASID.\"\n");
    return true;
  }
  return false;
}

bool chem_spout_pseudo_action(int verb) {
  if (verb == V_LOOK_INSIDE || verb == V_EXAMINE) {
    tellf("The spout is about ten centimeters across. Right below it is a small\n"
          "stand on which you could place something.\n");
    return true;
  }
  return false;
}

bool chemical_dispenser_f(int verb) {
  if (verb == V_PUT_UNDER) {
    ZObjectID prso = current_cmd.prso_count > 0 ? current_cmd.prso_list[0] : NOTHING;
    if (game_state.spout_placed == O_GROUND || game_state.spout_placed == NOTHING) {
      game_state.spout_placed = prso;
      obj_move(prso, current_room);
      tellf("The %s is now sitting under the spout.\n", objects[prso].description);
    } else {
      tellf("The %s is already resting under the spout.\n", objects[game_state.spout_placed].description);
    }
    return true;
  }
  return false;
}

bool devices_pseudo_action(int verb) {
  if (verb == V_EXAMINE) {
    tellf("They are components of disassembled robots, beyond repair.\n");
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
      tellf("The machine coughs a few times, but nothing else happens.\n");
      return true;
    }

    ZObjectID button = current_cmd.prso_count > 0 ? current_cmd.prso_list[0] : NOTHING;
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
        tellf("Another dose of the chemical fluid pours out of the spout, splashes over\n"
              "the already-full flask, spills onto the floor, and dries up.\n");
      } else {
        obj_move(O_CHEMICAL_FLUID, O_FLASK);
        tellf("The flask fills with some %s chemical fluid. The fluid gradually turns milky white.\n",
              get_color_name(color_val));
        game_state.chemical_flag = color_val;
      }
    } else if (game_state.spout_placed == O_CANTEEN &&
               obj_has_flag(O_CANTEEN, F_OPENBIT)) {
      tellf("Chemical fluid gushes from the spout. Unfortunately, the mouth of the canteen\n"
            "is very narrow, and the fluid just splashes over it.\n");
    } else {
      if (game_state.spout_placed != O_GROUND && game_state.spout_placed != NOTHING) {
        tellf("Some sort of chemical fluid pours out of the spout, spills all over the %s, and dries up.\n",
              objects[game_state.spout_placed].description);
      } else {
        tellf("Some sort of chemical fluid pours out of the spout, spills all over the floor, and dries up.\n");
      }
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
    if ((rand() % 5) == 0) {
      tellf("You catch, out of the corner of your eye, a glint of light from the direction\n"
            "of the floor.\n");
      return true;
    }
  }
  return false;
}

bool crevice_f(int verb) {
  if (verb == V_REACH) {
    tellf("The crevice is too narrow to reach into.\n");
    return true;
  }
  if (verb == V_LOOK_INSIDE || verb == V_EXAMINE || verb == V_SEARCH) {
    if (obj_has_flag(O_KEY, F_TOUCHBIT)) {
      tellf("Nothing there but bunches of dust.\n");
    } else {
      obj_clear_flag(O_KEY, F_INVISIBLE);
      tellf("Lying at the bottom of the narrow crack, partly covered by layers of dust,\n"
            "is a shiny steel key!\n");
    }
    return true;
  }
  return false;
}

bool key_f(int verb) {
  if ((verb == V_TAKE || verb == V_MOVE || verb == V_ATTRACT) &&
      !obj_has_flag(O_KEY, F_TOUCHBIT)) {
    if (current_cmd.prsi == O_PLIERS) {
      tellf("These are heavy-duty pliers, too large to reach into this narrow crack.\n");
      return true;
    }
    if (current_cmd.prsi == O_MAGNET) {
      if (!obj_in(O_MAGNET, player)) {
        tellf("You're not holding the magnet!\n");
        return true;
      }
      obj_move(O_KEY, player);
      obj_clear_flag(O_KEY, F_INVISIBLE);
      obj_clear_flag(O_KEY, F_TRYTAKEBIT);
      obj_set_flag(O_KEY, F_TOUCHBIT);
      tellf("With a spray of dust and a loud clank, a piece of metal leaps from the\n"
            "crevice and affixes itself to the magnet. It is a steel key! With a tug,\n"
            "you remove the key from the magnet.\n");
      return true;
    }
    if (current_cmd.prsi != NOTHING) {
      tellf("Nice try.\n");
      return true;
    }
    tellf("Either the crevice is too narrow, or your fingers are too large.\n");
    return true;
  }
  if (verb == V_PUT && current_cmd.prsi == O_CREVICE) {
    tellf("And you wonder why you're still only an Ensign Seventh Class?\n");
    return true;
  }
  return false;
}

ZObjectID ladder_exit_f(void) {
  if (game_state.ladder_flag) {
    game_state.c_elapsed = 33;
    tellf("You slowly make your way across the swaying ladder. You can see sharp,\n"
          "pointy rocks at the bottom of the rift, far below...\n\n");
    if (current_room == R_ADMIN_CORRIDOR_N) {
      return R_ADMIN_CORRIDOR;
    } else {
      return R_ADMIN_CORRIDOR_N;
    }
  } else {
    tellf("The rift is too wide to jump across.\n");
    return NOTHING;
  }
}

bool rift_f(int verb) {
  if (verb == V_LEAP) {
    jigs_up("You get a brief (but much closer) view of the sharp and nasty rocks at\n"
            "the bottom of the rift.");
    return true;
  }
  if (verb == V_PUT && current_cmd.prsi == O_RIFT) {
    if (current_cmd.prso_count > 0) {
      ZObjectID prso = current_cmd.prso_list[0];
      obj_remove(prso);
      if (prso == O_SCRUB_BRUSH) {
        tellf("You watch with tremendous satisfaction as the brush is lost forever.\n");
      } else {
        tellf("The %s sails gracefully into the rift.\n", objects[prso].description);
      }
    }
    return true;
  }
  if (verb == V_EXAMINE || verb == V_LOOK_INSIDE) {
    tellf("The rift is at least eight meters wide and more than thirty meters deep. The\n"
          "bottom is covered with sharp and nasty rocks.\n");
    return true;
  }
  return false;
}

// --- Elevator Actions ---

bool elevator_exit_f(void); // Forward decl

bool elevator_lobby_f(int arg) {
  if (arg == M_LOOK) {
    tellf("This is a wide, brightly lit lobby. A blue metal door to the north is %s",
          (obj_has_flag(O_UPPER_ELEVATOR_DOOR, F_OPENBIT) && !game_state.upper_elevator_up) ? "open" : "closed");
    tellf(" and a larger red metal door to the south is ");
    if (obj_has_flag(O_LOWER_ELEVATOR_DOOR, F_OPENBIT) && game_state.lower_elevator_up) {
      if (obj_has_flag(O_UPPER_ELEVATOR_DOOR, F_OPENBIT) && !game_state.upper_elevator_up) {
        tellf("also ");
      }
      tellf("open");
    } else {
      if (!obj_has_flag(O_UPPER_ELEVATOR_DOOR, F_OPENBIT) || game_state.upper_elevator_up) {
        tellf("also ");
      }
      tellf("closed");
    }
    tellf(". Beside the blue door is a blue button, and beside the red door is\n"
          "a red button. A corridor leads west. To the east is a small room\n"
          "about the size of a telephone booth.\n");
    return true;
  }
  return false;
}

bool upper_elevator_f(int arg) {
  if (arg == M_LOOK) {
    tellf("You have entered a tiny room with a sliding door to the south which is %s.\n"
          "A control panel contains an Up button, a Down button, and a narrow slot.\n",
          obj_has_flag(O_UPPER_ELEVATOR_DOOR, F_OPENBIT) ? "open" : "closed");
    return true;
  }
  return false;
}

bool lower_elevator_f(int arg) {
  if (arg == M_LOOK) {
    tellf("This is a medium-sized room with a door to the north which is %s.\n"
          "A control panel contains an Up button, a Down button, and a narrow slot.\n",
          obj_has_flag(O_LOWER_ELEVATOR_DOOR, F_OPENBIT) ? "open" : "closed");
    return true;
  }
  return false;
}

bool upper_elevator_door_f(int verb) {
  if (verb == V_OPEN) {
    if (obj_has_flag(O_UPPER_ELEVATOR_DOOR, F_OPENBIT)) {
      tellf("It is already open.\n");
    } else {
      tellf("It won't budge.\n");
    }
    return true;
  }
  if (verb == V_CLOSE) {
    if (obj_has_flag(O_UPPER_ELEVATOR_DOOR, F_OPENBIT)) {
      tellf("You can't close it yourself.\n");
    } else {
      tellf("It is already closed.\n");
    }
    return true;
  }
  return false;
}

bool lower_elevator_door_f(int verb) {
  if (verb == V_OPEN) {
    if (obj_has_flag(O_LOWER_ELEVATOR_DOOR, F_OPENBIT)) {
      tellf("It is already open.\n");
    } else {
      tellf("It won't budge.\n");
    }
    return true;
  }
  if (verb == V_CLOSE) {
    if (obj_has_flag(O_LOWER_ELEVATOR_DOOR, F_OPENBIT)) {
      tellf("You can't close it yourself.\n");
    } else {
      tellf("It is already closed.\n");
    }
    return true;
  }
  return false;
}

bool blue_elevator_button_f(int verb) {
  if (verb == V_PUSH) {
    if (game_state.upper_elevator_up) {
      if (is_event_enabled(EVT_UPPER_ELEVATOR_ARRIVE)) {
        tellf("Patience, patience...\n");
      } else {
        tellf("You hear a faint whirring noise from behind the blue door.\n");
        queue_event(EVT_UPPER_ELEVATOR_ARRIVE, (rand() % 20) + 40);
      }
      return true;
    }
  }
  return false;
}

bool red_elevator_button_f(int verb) {
  if (verb == V_PUSH) {
    if (!game_state.lower_elevator_up) {
      if (is_event_enabled(EVT_LOWER_ELEVATOR_ARRIVE)) {
        tellf("Patience, patience...\n");
      } else {
        tellf("The red door begins vibrating a bit.\n");
        queue_event(EVT_LOWER_ELEVATOR_ARRIVE, (rand() % 40) + 80);
      }
      return true;
    }
  }
  return false;
}

void i_upper_elevator_arrive(void) {
  obj_set_flag(O_UPPER_ELEVATOR_DOOR, F_OPENBIT);
  game_state.upper_elevator_up = false;
  dequeue_event(EVT_UPPER_ELEVATOR_ARRIVE);
  if (current_room == R_ELEVATOR_LOBBY) {
    tellf("\nThe door at the north end of the room slides open.\n");
  }
}

void i_lower_elevator_arrive(void) {
  obj_set_flag(O_LOWER_ELEVATOR_DOOR, F_OPENBIT);
  game_state.lower_elevator_up = true;
  dequeue_event(EVT_LOWER_ELEVATOR_ARRIVE);
  if (current_room == R_ELEVATOR_LOBBY) {
    tellf("\nThe door at the south end of the room slides open.\n");
  }
}

void i_upper_elevator_trip(void) {
  game_state.upper_elevator_up = !game_state.upper_elevator_up;
  game_state.elevator_in_transit = false;
  obj_set_flag(O_UPPER_ELEVATOR_DOOR, F_OPENBIT);
  dequeue_event(EVT_UPPER_ELEVATOR_TRIP);
  if (current_room == R_UPPER_ELEVATOR) {
    tellf("\nThe elevator door slides open.\n");
  }
}

void i_lower_elevator_trip(void) {
  game_state.lower_elevator_up = !game_state.lower_elevator_up;
  game_state.elevator_in_transit = false;
  obj_set_flag(O_LOWER_ELEVATOR_DOOR, F_OPENBIT);
  dequeue_event(EVT_LOWER_ELEVATOR_TRIP);
  if (current_room == R_LOWER_ELEVATOR) {
    tellf("\nThe elevator door slides open.\n");
  }
}

void i_turnoff_upper_elevator(void) {
  if (game_state.elevator_in_transit) {
    queue_event(EVT_TURNOFF_UPPER_ELEVATOR, 120);
  } else {
    game_state.upper_elevator_on = false;
    dequeue_event(EVT_TURNOFF_UPPER_ELEVATOR);
    if (current_room == R_UPPER_ELEVATOR) {
      tellf("\nA recording says \"Elevator no longer enabled.\"\n");
    }
  }
}

void i_turnoff_lower_elevator(void) {
  if (game_state.elevator_in_transit) {
    queue_event(EVT_TURNOFF_LOWER_ELEVATOR, 120);
  } else {
    game_state.lower_elevator_on = false;
    dequeue_event(EVT_TURNOFF_LOWER_ELEVATOR);
    if (current_room == R_LOWER_ELEVATOR) {
      tellf("\nA recording says \"Elevator no longer enabled.\"\n");
    }
  }
}

bool elevator_button_f(int verb) {
  if (verb == V_PUSH_UP) {
    if (current_room == R_LOWER_ELEVATOR && !game_state.lower_elevator_up &&
        game_state.lower_elevator_on && !game_state.elevator_in_transit) {
      tellf("The elevator door slides shut. After a moment, you feel a sensation of\n"
            "vertical movement.\n");
      obj_clear_flag(O_LOWER_ELEVATOR_DOOR, F_OPENBIT);
      game_state.elevator_in_transit = true;
      queue_event(EVT_LOWER_ELEVATOR_TRIP, 100);
      return true;
    }
    if (current_room == R_UPPER_ELEVATOR && !game_state.upper_elevator_up &&
        game_state.upper_elevator_on && !game_state.elevator_in_transit) {
      tellf("The elevator door slides shut. After a moment, you feel a sensation of\n"
            "vertical movement.\n");
      obj_clear_flag(O_UPPER_ELEVATOR_DOOR, F_OPENBIT);
      game_state.elevator_in_transit = true;
      queue_event(EVT_UPPER_ELEVATOR_TRIP, 50);
      return true;
    }
    tellf("Nothing happens.\n");
    return true;
  }
  if (verb == V_PUSH_DOWN) {
    if (current_room == R_LOWER_ELEVATOR && game_state.lower_elevator_up &&
        game_state.lower_elevator_on && !game_state.elevator_in_transit) {
      tellf("The elevator door slides shut. After a moment, you feel a sensation of\n"
            "vertical movement.\n");
      obj_clear_flag(O_LOWER_ELEVATOR_DOOR, F_OPENBIT);
      game_state.elevator_in_transit = true;
      queue_event(EVT_LOWER_ELEVATOR_TRIP, 100);
      return true;
    }
    if (current_room == R_UPPER_ELEVATOR && game_state.upper_elevator_up &&
        game_state.upper_elevator_on && !game_state.elevator_in_transit) {
      tellf("The elevator door slides shut. After a moment, you feel a sensation of\n"
            "vertical movement.\n");
      obj_clear_flag(O_UPPER_ELEVATOR_DOOR, F_OPENBIT);
      game_state.elevator_in_transit = true;
      queue_event(EVT_UPPER_ELEVATOR_TRIP, 50);
      return true;
    }
    tellf("Nothing happens.\n");
    return true;
  }
  if (verb == V_PUSH) {
    tellf("You must specify whether you want to push the Up button or\nthe Down button.\n");
    return true;
  }
  return false;
}

bool slot_f(int verb) {
  if (verb == V_PUT && current_cmd.prsi == O_SLOT) {
    tellf("The slot is shallow, so you can't put anything in it. It may be possible to\n"
          "slide something through the slot, though.\n");
    return true;
  }
  if (verb == V_EXAMINE) {
    tellf("The slot is about ten centimeters wide, but only about two centimeters deep.\n"
          "It is surrounded on its long sides by parallel ridges of metal.\n");
    return true;
  }
  if (verb == V_SLIDE && current_cmd.prsi == O_SLOT) {
    ZObjectID card = current_cmd.prso_count > 0 ? current_cmd.prso_list[0] : NOTHING;
    obj_move(card, player);
    if (obj_has_flag(card, F_SCRAMBLEDBIT)) {
      tellf("A sign flashes \"Magnetik striip randumiizd...konsult Prajekt Handbuk abowt\n"
            "propur kaar uv awtharazaashun kardz.\"\n");
      return true;
    }
    if (card == O_KITCHEN_CARD) {
      if (current_room == R_MESS_HALL) {
        if (obj_has_flag(O_KITCHEN_DOOR, F_OPENBIT)) {
          tellf("Nothing happens.\n");
        } else {
          obj_set_flag(O_KITCHEN_DOOR, F_OPENBIT);
          tellf("The kitchen door quietly slides open.\n");
        }
        return true;
      }
    } else if (card == O_UPPER_ELEVATOR_CARD) {
      if (current_room == R_UPPER_ELEVATOR) {
        game_state.upper_elevator_on = true;
        queue_event(EVT_TURNOFF_UPPER_ELEVATOR, 180);
        tellf("A voice says \"Elivaatur inebuld.\"\n");
        return true;
      }
    } else if (card == O_LOWER_ELEVATOR_CARD) {
      if (current_room == R_LOWER_ELEVATOR) {
        game_state.lower_elevator_on = true;
        queue_event(EVT_TURNOFF_LOWER_ELEVATOR, 200);
        tellf("A voice says \"Elivaatur inebuld.\"\n");
        return true;
      }
    } else if (card == O_SHUTTLE_CARD) {
      tellf("A voice says \"Shutul inebuld.\"\n");
      return true;
    }
    tellf("A small red light flashes twice.\n");
    return true;
  }
  return false;
}

// --- Floyd ---

// FLOYDISMS (compone.zil): what Floyd gets up to when he has nothing better to
// do, which is most of the time.
static const char *floydisms[] = {
    "paces impatiently",
    "absent-mindedly recites the first six hundred digits of pi",
    "lowers his voice and tells you the latest rumors about Dr. Fizpick",
    "recalls the time he bruised his knee",
    "chants the death scene from \"Carmen\"",
    "cranes his neck to see what you are doing",
    "rubs his head affectionately against your shoulder",
    "asks if you want to play Hucka-Bucka-Beanstalk",
    "examines himself for signs of rust",
    "absent-mindedly oils one of his joints",
    "wanders restlessly around the room",
    "notices a mouse scurrying by and tries to hide behind you",
    "sings an ancient ballad, totally out of key",
    "frets about the possibility of his batteries failing",
    "reminisces about his friend Lazarus, a medical robot",
    "relates some fond memories about his robotic friend Lazarus",
    "whistles tunelessly",
    "tells you about the time he helped someone sharpen a pencil",
    "yawns and looks bored",
    "produces a crayon from one of his compartments and scrawls\n"
    "his name on the wall",
};
#define NUM_FLOYDISMS (sizeof(floydisms) / sizeof(floydisms[0]))

// FLOYD-COMES-ALIVE (compone.zil).
void floyd_comes_alive(void) {
  if (obj_in(O_FLOYD, current_room)) {
    if (game_state.floyd_reactivated) {
      game_state.floyd_spoke = true;
      TELL("Floyd jumps to his feet, hopping mad. \"Why you turn Floyd off?\" "
           "he\n"
           "asks accusingly.\n");
    } else {
      game_state.floyd_introduced = true;
      game_state.floyd_spoke = true;
      TELL("Suddenly, the robot comes to life and its head starts swivelling "
           "about.\n"
           "It notices you and bounds over. \"Hi! I'm B-19-7, but to "
           "everyperson I'm\n"
           "called Floyd. Are you a doctor-person or a planner-person? ");
      ZObjectID carried = obj_first_child(player);
      if (carried != NOTHING) {
        tellf("That's a nice %s you are having there. ",
              objects[carried].description);
      }
      TELL("Let's play Hider-and-Seeker you with me.\"\n");
    }
  }
  obj_set_flag(O_FLOYD, F_RLANDBIT);
  obj_set_flag(O_FLOYD, F_ACTORBIT);
  obj_set_flag(O_FLOYD, F_TOUCHBIT);
  game_state.floyd_reactivated = true;
}

// CALL-ME-FLOYD (compone.zil): how he introduces himself when he catches you up
// somewhere other than the Robot Shop.
static void call_me_floyd(void) {
  game_state.floyd_introduced = true;
  TELL("The robot you were fiddling with in the Robot Shop bounds into the "
       "room.\n"
       "\"Hi!\" he says, with a wide and friendly smile. \"You turn Floyd on?\n"
       "Be Floyd's friend, yes?\"\n");
}

// COMPUTER-ACTION (compone.zil): Floyd works out that the Project's computer is
// broken, which is the first real hint of what is wrong with this place.
void computer_action(void) {
  game_state.computer_flag = true;
  game_state.floyd_spoke = true;
  tellf("Floyd examines the %s. With a concerned frown, he says, \"Uh oh. "
        "Computer is\n"
        "broken. A Doctor-person once told Floyd that Computer is the most "
        "important\n"
        "part of the Project.\"\n",
        current_room == R_COMPUTER_ROOM ? "glowing light"
                                        : "computer printout");
}

// KLUDGE (compone.zil): the two rooms Floyd has something to say about the
// first time he follows you into them.
static void kludge(void) {
  if (current_room == R_REPAIR_ROOM && !game_state.achilles_flag) {
    game_state.achilles_flag = true;
    game_state.floyd_spoke = true;
    TELL("Floyd points at the fallen robot. \"That's Achilles. He was in "
         "charge of\n"
         "repairing machinery. He repaired Floyd once. I never liked him much; "
         "he\n"
         "wasn't friendly like other robots. Looks like he fell down the "
         "stairs.\n"
         "He always had trouble with one of his feet working right. A "
         "Planner-person\n"
         "once told me that's why they named him Achilles.\"\n");
  } else if (current_room == R_COMPUTER_ROOM && !game_state.computer_flag) {
    computer_action();
  }
}

static bool prob(int percent) { return (rand() % 100) < percent; }

// I-FLOYD (compone.zil). Runs every turn once he is switched on: he follows you
// about, lets himself into places he should not, occasionally goes exploring on
// his own, and otherwise fills the silence.
void i_floyd(void) {
  queue_event(EVT_FLOYD, -1);

  if (!obj_has_flag(O_FLOYD, F_RLANDBIT)) {
    obj_set_flag(O_FLOYD, F_ACTORBIT);
    TELL("\n");
    floyd_comes_alive();
    game_state.floyd_spoke = false;
    return;
  }

  if (obj_in(O_FLOYD, current_room)) {
    if (!game_state.floyd_introduced) {
      game_state.floyd_introduced = true;
      TELL("\nThe robot, now apparently active, notices you enter. \"Hi,\"\n"
           "he says. \"I'm Floyd!\"\n");
    } else if (game_state.floyd_follow &&
               obj_has_flag(current_room, F_FLOYDBIT) && prob(6)) {
      obj_remove(O_FLOYD);
      game_state.floyd_follow = false;
      TELL("\nFloyd says \"Floyd going exploring. See you later.\"\n"
           "He glides out of the room.\n");
    } else {
      game_state.floyd_follow = true;
      if (prob(40) && !game_state.floyd_spoke) {
        tellf("Floyd %s.\n", floydisms[rand() % NUM_FLOYDISMS]);
        game_state.floyd_spoke = false;
        return;
      }
    }
    game_state.floyd_spoke = false;
    return;
  }

  // Floyd is active but somewhere else -- does he catch you up?
  if (game_state.floyd_follow && prob(80)) {
    if (obj_in(O_LAZARUS_PART, current_room)) {
      game_state.floyd_follow = false;
      TELL("\nFloyd starts to follow you but notices the Lazarus breast "
           "plate.\n"
           "He sniffs and leaves the room.\n");
      game_state.floyd_spoke = false;
      return;
    }
    obj_move(O_FLOYD, current_room);
    TELL("Floyd follows you.\n");
    kludge();
    game_state.floyd_spoke = false;
    return;
  }

  game_state.floyd_follow = false;

  if (current_room == R_BOOTH_1 || current_room == R_BOOTH_2 ||
      current_room == R_BOOTH_3) {
    obj_move(O_FLOYD, current_room);
    if (!game_state.floyd_introduced) {
      TELL("\n");
      call_me_floyd();
      return;
    }
    TELL("\nFloyd scampers into the booth. \"Oooo,\n"
         "this is a tiny room,\" he remarks.\n");
  } else if ((!game_state.floyd_gave_up &&
              (current_room == R_BIO_LOCK_EAST ||
               current_room == R_BIO_LOCK_WEST)) ||
             current_room == R_RADIATION_LOCK_EAST ||
             current_room == R_RADIATION_LOCK_WEST) {
    obj_move(O_FLOYD, current_room);
    if (!game_state.floyd_introduced) {
      TELL("\n");
      call_me_floyd();
      return;
    }
    TELL("\nFloyd glides after you. \"Is this...is this a squash court?\" he "
         "asks.\n");
    // ZIL also lets him into the Alfie and Betty shuttle control cabins here;
    // those rooms are not ported yet, so their clauses are missing.
  } else if (current_room == R_UPPER_ELEVATOR ||
             current_room == R_LOWER_ELEVATOR ||
             current_room == R_REACTOR_ELEVATOR ||
             (current_room == R_MESS_HALL && obj_in(O_FLOYD, R_KITCHEN))) {
    obj_move(O_FLOYD, current_room);
    if (!game_state.floyd_introduced) {
      TELL("\n");
      call_me_floyd();
      return;
    }
    TELL("\nFloyd bounces into the ");
    if (current_room == R_UPPER_ELEVATOR || current_room == R_LOWER_ELEVATOR ||
        current_room == R_REACTOR_ELEVATOR) {
      TELL("elevator");
    } else if (current_room == R_MESS_HALL) {
      TELL("room");
    } else {
      TELL("cabin");
    }
    TELL(". \"Hey, wait for Floyd!\" he yells, smiling broadly.\n");
  } else if (current_room == R_MINI_BOOTH) {
    obj_move(O_FLOYD, current_room);
    if (!game_state.floyd_introduced) {
      TELL("\n");
      call_me_floyd();
      return;
    }
    TELL("\n\"Hi,\" whispers Floyd, tiptoeing in. \"Are we going to teleport "
         "into\n"
         "the computer like Achilles always used to do?\"\n");
  } else if (prob(30)) {
    if (current_room == R_INFIRMARY && game_state.lazarus_flag) {
      game_state.floyd_spoke = false;
      return;
    }
    obj_move(O_FLOYD, current_room);
    if (game_state.floyd_introduced) {
      if (prob(15) && !obj_in(player, O_BED)) {
        TELL("\nFloyd rushes into the room and barrels into you. \"Oops, "
             "sorry,\" he says.\n"
             "\"Floyd not looking at where he was going to.\"\n");
      } else {
        TELL("\nFloyd bounds into the room. \"Floyd here now!\" he cries.\n");
      }
      kludge();
    } else {
      TELL("\n");
      call_me_floyd();
    }
  }

  game_state.floyd_spoke = false;
}

// FLOYD-NOT-HAVE (compone.zil).
static void floyd_not_have(void) {
  TELL("\"Floyd does not one of those have!\"\n");
}

// FLOYD-INTO-LAB and FLOYDS-FAMOUS-DOOR-ROUTINE belong to the half of FLOYD-F
// that answers commands addressed to him, which needs a WINNER the parser does
// not have yet. They arrive with the Bio Lab sequence.

// FLOYD-F (compone.zil), the half that handles things done *to* Floyd. The
// other half -- commands addressed to him, "FLOYD, TAKE THE BOARD" -- needs the
// parser to support a WINNER other than the player, which it does not yet.
bool floyd_f(int arg) {
  (void)arg;
  int verb = current_cmd.verb;
  ZObjectID prso = current_cmd.prso_list[0];

  if (verb == V_CLOSE) {
    TELL("Huh?\n");
    return true;
  }
  if (verb == V_LOOK_INSIDE || verb == V_REACH) {
    return perform(V_OPEN, O_FLOYD, NOTHING);
  }

  if (obj_has_flag(O_FLOYD, F_RLANDBIT)) {
    // --- Floyd is switched on -------------------------------------------
    game_state.floyd_spoke = true;

    switch (verb) {
    case V_LAMP_ON:
      TELL("He's already been activated.\n");
      return true;
    case V_LAMP_OFF:
      obj_clear_flag(O_FLOYD, F_RLANDBIT);
      obj_clear_flag(O_FLOYD, F_ACTORBIT);
      dequeue_event(EVT_FLOYD);
      TELL("Floyd, shocked by this betrayal from his new-found friend, "
           "whimpers\n"
           "and keels over");
      if (obj_first_child(O_FLOYD) != NOTHING) {
        TELL(", dropping what he was carrying.\n");
      } else {
        TELL(".\n");
      }
      obj_rob(O_FLOYD, current_room);
      return true;
    case V_EXAMINE:
      TELL("From its design, the robot seems to be of the multi-purpose sort. "
           "It is\n"
           "slightly cross-eyed, and its mechanical mouth forms a lopsided "
           "grin.\n");
      return true;
    case V_KISS:
      TELL("You receive a painful electric shock.\n");
      return true;
    case V_SCOLD:
      TELL("Floyd looks defensive. \"What did Floyd do wrong?\"\n");
      return true;
    case V_PLAY_WITH:
      game_state.c_elapsed = 30;
      queue_event(EVT_FLOYD, 1);
      TELL("You play with Floyd for several centichrons until you drop to the "
           "floor,\n"
           "exhausted. Floyd pokes at you gleefully. \"C'mon! Let's play some "
           "more!\"\n");
      return true;
    case V_LISTEN:
      TELL("Floyd is babbling about this and that.\n");
      return true;
    case V_TAKE:
      TELL("You manage to lift Floyd a few inches off the ground, but he is "
           "too heavy\n"
           "and you drop him suddenly. Floyd gives a surprised squeal and "
           "moves\n"
           "a respectable distance away.\n");
      return true;
    case V_ATTACK:
    case V_MUNG:
      TELL("Floyd starts dashing around the room. \"Oh boy oh boy oh boy! I "
           "haven't played\n"
           "Chase and Tag for years! You be It! Nah, nah!\"\n");
      return true;
    case V_KICK:
    case V_SHAKE:
      TELL("\"Why you do that?\" Floyd whines. \"I think a wire now shaken "
           "loose.\"\n"
           "He goes off into a corner and sulks.\n");
      return true;
    case V_HELLO:
    case V_TALK:
      TELL("\"Hi!\" Floyd grins and bounces up and down.\n");
      return true;
    case V_SEARCH:
    case V_SCRUB:
    case V_OPEN:
      TELL("Floyd giggles and pushes you away. \"You're tickling Floyd!\" He "
           "clutches at\n"
           "his side panels, laughing hysterically. Oil drops stream from his "
           "eyes.\n");
      return true;
    case V_RUB:
      TELL("Floyd gives a contented sigh.\n");
      return true;
    case V_SMELL:
      TELL("Floyd smells faintly of ozone and light machine oil.\n");
      return true;
    case V_ASK_FOR:
      if (obj_in(current_cmd.prsi, O_FLOYD)) {
        obj_move(current_cmd.prsi, player);
        tellf("\"Okay,\" says Floyd, handing you the %s, \"but only\n"
              "because you're Floyd's best friend.\"\n",
              objects[current_cmd.prsi].description);
      } else {
        floyd_not_have();
      }
      return true;
    case V_GIVE:
    case V_PUT:
      if (current_cmd.prsi != O_FLOYD)
        return false;
      if (prso == O_LAZARUS_PART) {
        obj_remove(O_FLOYD);
        game_state.floyd_follow = false;
        obj_move(O_LAZARUS_PART, current_room);
        queue_event(EVT_FLOYD, 40);
        TELL("At first, Floyd is all grins because of your gift. Then, he "
             "realizes what\n"
             "it is, begins weeping, drops the breastplate, and rushes out of "
             "the room.\n");
      } else if (prso == O_RED_GOO || prso == O_GREEN_GOO ||
                 prso == O_BROWN_GOO) {
        TELL("Floyd looks at the goo. \"Yech! Got any Number Seven Heavy "
             "Grease?\"\n");
      } else if (obj_first_child(O_FLOYD) != NOTHING || prob(25)) {
        obj_move(prso, current_room);
        tellf("Floyd examines the %s, shrugs, and drops %s\n",
              objects[prso].description, prso == O_PLIERS ? "them." : "it.");
      } else {
        obj_move(prso, O_FLOYD);
        TELL("\"Neat!\" exclaims Floyd. He thanks you profusely.\n");
      }
      return true;
    case V_SHOW:
      if (current_cmd.prsi != O_FLOYD)
        return false;
      if (prso == O_PRINT_OUT && !game_state.computer_flag) {
        computer_action();
      } else if (current_room == R_REC_AREA && prso == O_PSEUDO_OBJECT) {
        TELL("\"Too intellectual for Floyd. Any paddleball sets around?\"\n");
      } else if (prso == O_ID_CARD || prso == O_SHUTTLE_CARD ||
                 prso == O_KITCHEN_CARD || prso == O_UPPER_ELEVATOR_CARD) {
        TELL("Floyd scratches his head. \"Aren't those things usually "
             "blue?\"\n");
      } else if (prso == O_LOWER_ELEVATOR_CARD && !game_state.card_revealed) {
        game_state.card_revealed = true;
        TELL("\"I've got one just like that!\" says Floyd. He looks through "
             "several\n"
             "of his compartments, then glances at you suspiciously.\n");
      } else {
        tellf("Floyd looks over the %s. \"Can you play any games with it?\"\n"
              "he asks.\n",
              objects[prso].description);
      }
      return true;
    default:
      return false;
    }
  }

  // --- Floyd is switched off ---------------------------------------------
  switch (verb) {
  case V_LAMP_ON:
    if (game_state.floyd_introduced) {
      queue_event(EVT_FLOYD, -1);
    } else {
      // The joke: nothing seems to happen, and a few turns later he comes
      // bounding over. I-FLOYD's first branch is what fires when this expires.
      queue_event(EVT_FLOYD, 25);
      TELL("Nothing happens.\n");
      if (!game_state.floyd_score_flag) {
        game_state.floyd_score_flag = true;
        game_state.score += 2;
      }
    }
    return true;
  case V_LAMP_OFF:
    TELL("The robot doesn't seem to be on.\n");
    return true;
  case V_EXAMINE:
    TELL("The de-activated robot is leaning against the wall, its head lolling "
         "to the\n"
         "side. It is short, and seems to be equipped for general-purpose "
         "work. It has\n"
         "apparently been turned off.\n");
    return true;
  case V_SEARCH:
  case V_OPEN:
    if (!game_state.card_revealed && !game_state.card_stolen) {
      obj_clear_flag(O_LOWER_ELEVATOR_CARD, F_INVISIBLE);
      obj_move(O_LOWER_ELEVATOR_CARD, player);
      score_obj(O_LOWER_ELEVATOR_CARD);
      game_state.card_stolen = true;
      TELL("In one of the robot's compartments you find and take a "
           "magnetic-striped card\n"
           "embossed \"Loowur Elavaatur Akses Kard.\"\n");
    } else {
      TELL("Your search discovers nothing in the robot's compartments except "
           "a\n"
           "single crayon which you leave where you found it.\n");
    }
    return true;
  default:
    return false;
  }
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

bool monitors_pseudo_action(int verb) {
  if (verb == V_EXAMINE || verb == V_READ) {
    describe_monitors();
    return true;
  }
  return false;
}

bool equipment_pseudo_action(int verb) {
  if (verb == V_EXAMINE || verb == V_RUB || verb == V_LAMP_ON || verb == V_LAMP_OFF) {
    tellf("The equipment here is so complicated that you couldn't even begin to\n"
          "figure out how to operate it.\n");
    return true;
  }
  return false;
}

bool cubbyhole_pseudo_action(int verb) {
  if (verb == V_EXAMINE || verb == V_LOOK_INSIDE) {
    tellf("The cubbyholes look like the kind that are used to hold maps or\n"
          "blueprints. They are all empty now.\n");
    return true;
  }
  return false;
}

bool maps_pseudo_action(int verb) {
  if (verb == V_EXAMINE) {
    tellf("Examining the maps reveals no new information.\n");
    return true;
  }
  return false;
}

bool desk_f(int verb) {
  if (verb == V_SEARCH || verb == V_EXAMINE) {
    ZObjectID prso = current_cmd.prso_count > 0 ? current_cmd.prso_list[0] : NOTHING;
    tellf("The desk has a drawer which is currently %s.\n",
          obj_has_flag(prso, F_OPENBIT) ? "open" : "closed");
    return true;
  }
  return false;
}

bool oil_can_f(int verb) {
  if (verb == V_POUR) {
    ZObjectID prsi = current_cmd.prsi != NOTHING ? current_cmd.prsi : O_GROUND;
    tellf("A few drops of oil pour out onto the %s.\n", objects[prsi].description);
    return true;
  }
  if (verb == V_EMPTY) {
    tellf("Pretty much impossible -- you could only do that one drop at a time.\n");
    return true;
  }
  return false;
}

bool carton_f(int verb) {
  if (verb == V_CLOSE) {
    tellf("You can't close that.\n");
    return true;
  }
  return false;
}

void examine_board(void) {
  tellf("Like most fromitz boards, it is a twisted maze of silicon circuits. It is\n"
        "square, approximately seventeen centimeters on each side.");
}

bool cracked_board_f(int verb) {
  if (verb == V_EXAMINE) {
    examine_board();
    tellf(" This one looks as though it's been dropped.\n");
    return true;
  }
  return false;
}

bool good_bedistor_f(int verb) {
  if (verb == V_TAKE && game_state.course_control_fixed) {
    jigs_up("Kerzap!! You should know better than to touch an active bedistor!");
    return true;
  }
  return false;
}

bool catwalk_pseudo_action(int verb) {
  if (verb == V_CLIMB_ON || verb == V_CLIMB_UP || verb == V_CLIMB_FOO) {
    tellf("The catwalks are too high for you to access.\n");
    return true;
  }
  return false;
}

bool reactor_elevator_door_f(int verb) {
  if (verb == V_OPEN || verb == V_CLOSE) {
    tellf("It won't budge.\n");
    return true;
  }
  return false;
}

void i_reactor_door_close(void) {
  if (current_room != R_REACTOR_ELEVATOR) {
    obj_clear_flag(O_REACTOR_ELEVATOR_DOOR, F_OPENBIT);
    if (current_room == R_REACTOR_CONTROL) {
      tellf("\nThe elevator door slides shut.\n");
    }
    dequeue_event(EVT_REACTOR_DOOR_CLOSE);
  } else {
    queue_event(EVT_REACTOR_DOOR_CLOSE, -1);
  }
}

bool reactor_button_pseudo_action(int verb) {
  if (verb == V_PUSH) {
    obj_set_flag(O_REACTOR_ELEVATOR_DOOR, F_OPENBIT);
    tellf("The metal doors slide open, revealing a small room to the east.\n");
    queue_event(EVT_REACTOR_DOOR_CLOSE, 30);
    return true;
  }
  return false;
}

bool diagram_pseudo_action(int verb) {
  if (verb == V_EXAMINE || verb == V_READ) {
    tellf("The diagram shows a massive planetary power reactor far below this very complex.\n");
    return true;
  }
  return false;
}

bool flask_f(int verb) {
  if (verb == V_EXAMINE) {
    tellf("The flask has a wide mouth and looks large enough to hold one or two liters.\n"
          "It is made of glass, or perhaps some tough plastic");
    if (obj_in(O_CHEMICAL_FLUID, O_FLASK)) {
      tellf(", and is filled with a milky white fluid");
    }
    tellf(".\n");
    return true;
  }
  if (verb == V_CLOSE) {
    tellf("You can't close that.\n");
    return true;
  }
  return false;
}

void i_magnet(void) {
  if (obj_in(O_MAGNET, player)) {
    if (obj_in(O_KITCHEN_CARD, player)) obj_set_flag(O_KITCHEN_CARD, F_SCRAMBLEDBIT);
    if (obj_in(O_SHUTTLE_CARD, player)) obj_set_flag(O_SHUTTLE_CARD, F_SCRAMBLEDBIT);
    if (obj_in(O_UPPER_ELEVATOR_CARD, player)) obj_set_flag(O_UPPER_ELEVATOR_CARD, F_SCRAMBLEDBIT);
    if (obj_in(O_LOWER_ELEVATOR_CARD, player)) obj_set_flag(O_LOWER_ELEVATOR_CARD, F_SCRAMBLEDBIT);
  } else {
    dequeue_event(EVT_MAGNET);
  }
}

bool magnet_f(int verb) {
  if (verb == V_TAKE) {
    queue_event(EVT_MAGNET, -1);
    return false; // let normal take proceed
  }
  if (verb == V_ATTRACT || verb == V_PUT_ON) {
    if (current_cmd.prso_count > 0 && current_cmd.prso_list[0] == O_MAGNET && !obj_in(O_MAGNET, player)) {
      tellf("You're not holding that!\n");
      return true;
    }
    ZObjectID prsi = current_cmd.prsi;
    if (prsi == O_KEY || prsi == O_CREVICE) {
      if (obj_has_flag(O_KEY, F_TOUCHBIT)) {
        obj_move(O_KEY, player);
        tellf("The key jumps against the ends of the magnet and sticks there. Proud of your\n"
              "feat, you remove the key from the magnet.\n");
      } else {
        obj_move(O_KEY, player);
        obj_clear_flag(O_KEY, F_INVISIBLE);
        obj_clear_flag(O_KEY, F_TRYTAKEBIT);
        obj_set_flag(O_KEY, F_TOUCHBIT);
        tellf("With a spray of dust and a loud clank, a piece of metal leaps from the\n"
              "crevice and affixes itself to the magnet. It is a steel key! With a tug,\n"
              "you remove the key from the magnet.\n");
      }
      return true;
    }
  }
  return false;
}

bool systems_monitors_f(int arg) {
  if (arg == M_LOOK) {
    TELL("This is a large room filled with tables full of strange equipment. ");
    describe_monitors();
    return true;
  }
  return false;
}

bool mess_hall_f(int arg) {
  if (arg == M_LOOK) {
    tellf("This is a large hall lined with tables and benches. An opening to the north\n"
          "leads back to the corridor. A door to the south is %s. Next to the door\n"
          "is a small slot.\n",
          obj_has_flag(O_KITCHEN_DOOR, F_OPENBIT) ? "open" : "closed");
    return true;
  }
  return false;
}

bool helicopter_object_f(int verb) {
  if (verb == V_THROUGH || verb == V_BOARD || verb == V_WALK_TO) {
    if (current_room == R_HELIPAD) {
      obj_move(player, R_HELICOPTER);
      current_room = R_HELICOPTER;
      perform_first_look();
      return true;
    } else {
      tellf("You're in it!\n");
      return true;
    }
  }
  if (verb == V_EXIT || verb == V_DROP || verb == V_DISEMBARK) {
    if (current_room == R_HELICOPTER) {
      obj_move(player, R_HELIPAD);
      current_room = R_HELIPAD;
      perform_first_look();
      return true;
    } else {
      tellf("You're not in it!\n");
      return true;
    }
  }
  if (verb == V_FLY) {
    if (current_room == R_HELICOPTER) {
      tellf("The controls seem to be locked.\n");
    } else {
      tellf("You're not even in it!\n");
    }
    return true;
  }
  return false;
}

bool fence_pseudo_action(int verb) {
  if (verb == V_EXAMINE) {
    tellf("The fence is about chest-high and completely surrounds the helipad.\n");
    return true;
  }
  if (verb == V_CLIMB_UP || verb == V_CLIMB_ON || verb == V_CLIMB_FOO) {
    tellf("The wind is far too strong to attempt that.\n");
    return true;
  }
  return false;
}

bool lock_pseudo_action(int verb) {
  if (verb == V_EXAMINE) {
    tellf("The lock is built into the control panel.\n");
    return true;
  }
  return false;
}

bool cables_pseudo_action(int verb) {
  if (verb == V_EXAMINE) {
    tellf("Thick cables snake up from the consoles into the ceiling above.\n");
    return true;
  }
  return false;
}

bool enunciator_pseudo_action(int verb) {
  if (verb == V_EXAMINE) {
    if (game_state.comm_fixed || game_state.comm_shutdown) {
      tellf("All the lights on the enunciator panel are dark.\n");
    } else {
      const char *color = get_color_name(game_state.chemical_required);
      tellf("A %s light is flashing on the enunciator panel.\n", color);
    }
    return true;
  }
  return false;
}

bool playback_button_f(int verb) {
  if (verb == V_PUSH) {
    tellf("A voice fills the room ... the voice of the Feinstein's communications\n"
          "officer! \"Stellar Patrol Ship Feinstein to planetside ... Please respond\n"
          "on frequency 48.5 ... SPS Feinstein to planetside ... Please come in ...\"\n"
          "After a pause you hear the officer, in a quieter voice, say \"Admiral, no\n"
          "response on any of the standard frequen...\" The sentence is cut short by the\n"
          "sound of an explosion and a loud burst of static, followed by silence.\n");
    return true;
  }
  return false;
}

bool chemical_fluid_f(int verb) {
  if (verb == V_EAT) {
    jigs_up("Mmmmm....that tasted just like delicious poisonous chemicals!");
    return true;
  }
  if (verb == V_PUT || verb == V_POUR) {
    if (!obj_in(O_FLASK, player)) {
      tellf("You're not holding the flask.\n");
      return true;
    }
    ZObjectID prsi = current_cmd.prsi;
    obj_remove(O_CHEMICAL_FLUID);
    if (prsi == O_FUNNEL_HOLE) {
      if (game_state.chemical_flag == game_state.chemical_required) {
        game_state.comm_fixed = true;
        game_state.score += 6;
        game_state.chemical_required = 10;
        tellf("The liquid disappears into the hole. The lights on the enunciator\n"
              "panel blink rapidly and then go dark. The coolant system warning light goes off, and another\n"
              "flashes, indicating that the help message is now being sent.\n");
      } else {
        game_state.comm_shutdown = true;
        tellf("An alarm sounds briefly, and a sign flashes \"Kuulint Sistum Imbalins Kritikul -- Shuteeng Down Awl Sistumz.\"\n"
              "A moment later, the lights in the room dim and the send console shuts down.\n");
      }
      return true;
    } else {
      tellf("The chemical pours all over the %s, making quite a mess.\n",
            (prsi != NOTHING) ? objects[prsi].description : "floor");
      return true;
    }
  }
  return false;
}

void i_unenter(void) {
  if (current_room != R_COMM_ROOM) {
    game_state.just_entered = true;
    dequeue_event(EVT_UNENTER);
  }
}

bool comm_room_f(int arg) {
  if (arg == M_LOOK) {
    tellf("This is a small room with no windows. The sole exit is southwest. Two wide\n"
          "consoles fill either end of the room; thick cables lead up into the ceiling.\n\n"
          "The console on the left side of the room is labelled \"Reeseev Staashun.\" A\n"
          "bright red light, labelled \"Tranzmishun Reeseevd\", is blinking rapidly.\n"
          "Next to the light is a glowing button marked \"Mesij Plaabak.\"\n\n"
          "The console on the right side of the room is labelled \"Send Staashun.\" A\n"
          "screen on the console displays a message. Next to the screen is a flashing\n"
          "sign which says ");
    if (game_state.comm_shutdown) {
      tellf("\"Kuulint Sistum Imbalins Kritikul -- Shuteeng Down Awl Sistumz.\"");
    } else if (game_state.comm_fixed) {
      tellf("\"Tranzmishun in pragres.\"");
    } else {
      tellf("\"Malfunkshun in Sendeeng Kuulint Sistum.\"");
    }
    tellf(" Next to this console is an enunciator");
    if (game_state.comm_fixed || game_state.comm_shutdown) {
      tellf(" whose lights are all dark");
    }
    tellf(".\nOn the console next to the enunciator panel is a funnel-shaped hole\n"
          "labelled \"Kuulint Sistum Manyuuwul Oovuriid.\"\n");
    return true;
  }
  if (arg == M_END) {
    if (!game_state.comm_fixed && !game_state.comm_shutdown && game_state.just_entered) {
      queue_event(EVT_UNENTER, -1);
      game_state.just_entered = false;
      const char *color = get_color_name(game_state.chemical_required);
      tellf("A %s colored light is flashing on the enunciator panel.\n", color);
      return true;
    }
  }
  return false;
}

bool kalamontee_platform_f(int arg) {
  if (arg == M_LOOK) {
    tellf("This is a wide, flat strip of concrete which continues westward. "
          "Open shuttle cars lie on the north and south sides of the platform. "
          "A faded sign on the wall reads \"Shutul Platform -- Kalamontee Staashun.\"\n");
    return true;
  }
  return false;
}
