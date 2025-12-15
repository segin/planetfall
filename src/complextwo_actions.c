#include "complextwo_actions.h"
#include "actions.h"
#include "ids.h"
#include "output.h"
#include "parser.h"
#include "planetfall.h"
#include "syntax_gen.h"
#include <stdlib.h> // For rand if needed

// Forward declarations if needed

// --- Complex Two Actions ---

bool lawanda_platform_f(int arg) {
  if (arg == M_LOOK) {
    TELL("This is a wide, flat strip of concrete. ");
    // COND check for alfie/betty (shuttles)
    bool alfie_at_kal = false; // TODO: Check global
    bool betty_at_kal = false; // TODO: Check global

    if (!alfie_at_kal && !betty_at_kal) {
      TELL("Open shuttle cars lie to the north and south.");
    } else if (!alfie_at_kal || !betty_at_kal) {
      TELL("An open shuttle car lies to the ");
      if (alfie_at_kal)
        TELL("north.");
      else
        TELL("south.");
    }

    TELL(" A wide escalator, not currently operating, beckons upward at the "
         "east end of\n"
         "the platform. A faded sign reads \"Shutul Platform -- Lawanda "
         "Staashun.\"\n");
    return true;
  }
  // M_ENTER check for flags
  if (arg == M_ENTER) {
    // if (!lawanda_platform_flag) ... set flag, warn sickness
  }
  return false;
}

// Placeholder for now, will implement routines systematically
// --- INFIRMARY ---

bool infirmary_f(int arg) {
  if (arg == M_END) {
    // Check for Floyd finding Lazarus part
    // ZIL: <COND (<AND <EQUAL? .RARG ,M-END> <NOT ,LAZARUS-FLAG> <IN? ,FLOYD
    // ,HERE> <FSET? ,FLOYD ,RLANDBIT> <PROB 30>> ...>
    if (!game_state.lazarus_flag && obj_in(O_FLOYD, current_room) &&
        obj_has_flag(O_FLOYD, F_RLANDBIT)) { // TODO: Add probability check if
                                             // needed, or deterministic for now
      game_state.lazarus_flag = true;
      obj_move(O_LAZARUS_PART, current_room);
      obj_move(O_FLOYD, R_FORK); // Floyd leaves
      // game_state.floyd_follow = false; // TODO: Implement follow logic
      // game_state.floyd_spoke = true;
      TELL("Floyd, rummaging in a corner, finds something and carries it to "
           "the center of\n"
           "the room to examine it in the brighter light. It seems to be the "
           "breast plate\n"
           "of a robot, along with some connected inner circuitry. The entire "
           "piece is\n"
           "bent and rusting. Floyd stares at it in complete silence. A moment "
           "later, he\n"
           "begins sobbing quietly, awkwardly excuses himself, and runs out of "
           "the room.\n"
           "You look at the breast plate, and notice the name \"Lazarus\" "
           "engraved on\n"
           "it.\n");
      return true;
    }
  }
  return false;
}

bool red_spool_f(void) {
  if (current_cmd.verb == V_TAKE) {
    // Check if inside spool reader and reader on
    // ZIL: <AND <VERB? TAKE> <IN? ,RED-SPOOL ,SPOOL-READER> <FSET?
    // ,SPOOL-READER ,ONBIT>> Need to check where it is. If user types TAKE
    // SPOOL, parser handles basic take. But if it's in reader, we might want
    // special text? ZIL says: "The screen goes blank as you take the spool."
    /*
    if (obj_in(O_RED_SPOOL, O_SPOOL_READER) && obj_has_flag(O_SPOOL_READER,
    F_ONBIT)) {
         // Let standard Move happen or do it here?
         // Helper specific logic needed for "Taking from container" hook?
         // Since C parser is simple, likely we intercept before move.
         TELL("The screen goes blank as you take the spool.\n");
         // Return false to let normal take happen? Or do move and return true.
         obj_move(O_RED_SPOOL, player);
         obj_clear_flag(O_RED_SPOOL, F_TRYTAKEBIT);
         return true;
    }
    */
  }
  return false;
}

bool medicine_f(void) {
  // Handling Medicine object (quantity) inside Bottle
  if (current_cmd.verb == V_EAT || current_cmd.verb == V_TASTE ||
      current_cmd.verb == V_POUR) {
    if (!obj_in(O_MEDICINE_BOTTLE, player)) {
      // ZIL: <SETG PRSO ,MEDICINE-BOTTLE> <NOT-HOLDING>
      TELL("You are not holding the medicine bottle.\n");
      return true;
    }
    // Check open bit of bottle?
    // ZIL logic seems to imply medicine IS the bottle contents?
    // Simulating liquid mechanics is tricky.
    if (current_cmd.verb == V_EAT) {
      obj_remove(O_MEDICINE); // Remove liquid
      game_state.c_elapsed = 15;
      game_state.sickness_level -= 2;
      // game_state.load_allowed += 20;
      TELL("The medicine tasted extremely bitter.\n");
      return true;
    }
  }
  return false;
}
// --- ROBOT HOLE ---

void floyd_through_hole(void) {
  if (game_state.hole_trip_flag) {
    TELL("\"Not again,\" whines Floyd.\n");
    return;
  }
  game_state.c_elapsed = 50;
  game_state.hole_trip_flag = true;
  game_state.board_reported = true;
  obj_clear_flag(O_GOOD_BOARD, F_INVISIBLE);
  TELL("Floyd squeezes through the opening and is gone for quite a while. You "
       "hear\n"
       "thudding noises and squeals of enjoyment. After a while the noise "
       "stops, and\n"
       "Floyd emerges, looking downcast. \"Floyd found a rubber ball inside. "
       "Lots of\n"
       "fun for a while, but must have been old, because it fell apart. "
       "Nothing else\n"
       "interesting inside. Just a shiny fromitz board.\"\n");
}

bool robot_hole_f(int arg) {
  if (current_cmd.verb == V_EXAMINE) {
    TELL("It's too small for you to get through. It was presumably intended "
         "for\n"
         "robots, such as the broken repair robot lying over there.\n");
    return true;
  }
  if (current_cmd.verb == V_LOOK_INSIDE) {
    TELL("You can make out a small supply room of some sort.\n");
    return true;
  }
  return false;
}

// Good Board (in hole)
bool good_board_f(void) {
  if (obj_has_flag(O_GOOD_BOARD, F_NDESCBIT)) {
    // If hidden/invisible logic
    // But we handle this via F_INVISIBLE in C engine usually?
  }
  if (current_cmd.verb == V_EXAMINE) {
    TELL("Like most fromitz boards, it is a twisted maze of silicon circuits. "
         "It is\n"
         "square, approximately seventeen centimeters on each side.\n");
    return true;
  }
  return false;
}
// --- PLANETARY DEFENSE ---

bool planetary_defense_f(int arg) {
  if (arg == M_LOOK) {
    TELL("This room is filled with a dazzling array of lights and controls. ");
    if (!game_state.defense_fixed) {
      TELL("One light, blinking quickly, catches your eye. It reads \"Surkit "
           "Boord\n"
           "Faalyur. WORNEENG: xis boord kuntroolz xe diskriminaashun "
           "surkits.\"");
    }
    TELL(" There is a small access panel on one wall which is ");
    if (obj_has_flag(O_ACCESS_PANEL, F_OPENBIT))
      TELL("open");
    else
      TELL("closed");
    TELL(".\n");
    return true;
  }
  return false;
}

bool access_panel_f(int arg) {
  if (current_cmd.verb == V_OPEN) {
    if (obj_has_flag(O_ACCESS_PANEL, F_OPENBIT)) {
      tellf("It is already open.\n");
    } else {
      obj_set_flag(O_ACCESS_PANEL, F_OPENBIT);
      tellf("The panel swings open.\n");
      // Standard ZIL behavior: OPEN reveals contents automatically if
      // transparent/open But strict ZIL here says <PERFORM ,V?LOOK-INSIDE
      // ,ACCESS-PANEL> We can simulate that or just let standard examine handle
      // it if the user looks. For closer fidelity:
      perform_examine(O_ACCESS_PANEL);
    }
    return true;
  }
  if (current_cmd.verb == V_CLOSE) {
    if (obj_has_flag(O_ACCESS_PANEL, F_OPENBIT)) {
      obj_clear_flag(O_ACCESS_PANEL, F_OPENBIT);
      tellf("The panel swings closed.\n");
    } else {
      tellf("It is already closed.\n");
    }
    return true;
  }
  if (current_cmd.verb == V_PUT && current_cmd.prsi == O_ACCESS_PANEL) {
    if (!obj_has_flag(O_ACCESS_PANEL, F_OPENBIT)) {
      tellf("The panel is closed.\n");
      return true;
    }
    if (game_state.access_panel_full) {
      tellf("There's no room.\n");
      return true;
    }
    // Logic for putting boards
    if (current_cmd.prso_list[0] == O_GOOD_BOARD) {
      obj_remove(O_GOOD_BOARD);
      obj_move(O_SECOND_BOARD,
               O_ACCESS_PANEL); // Swap good board to second board?
      // ZIL: <MOVE ,SECOND-BOARD ,ACCESS-PANEL> <THIS-IS-IT ,SECOND-BOARD>
      game_state.defense_fixed = true;
      game_state.score += 6;
      game_state.access_panel_full = true;
      // <PUT-BOARD> logic
      tellf("The card clicks neatly into the socket. The warning lights stop "
            "flashing.\n");
      return true;
    }
    // Handle other boards (bad ones)
    if (current_cmd.prso_list[0] == O_CRACKED_BOARD ||
        current_cmd.prso_list[0] == O_FRIED_BOARD) {
      obj_remove(current_cmd.prso_list[0]);
      obj_move(O_SECOND_BOARD, O_ACCESS_PANEL);
      game_state.access_panel_full = true;
      // ZIL: <COND (<EQUAL? ,PRSO ,CRACKED-BOARD> <SETG ITS-CRACKED T>)>
      // (ITS-CRACKED is global) We don't have ITS-CRACKED in game_state yet,
      // assume irrelevant or add? It affects what you get back when you TAKE
      // it.
      tellf("The card clicks neatly into the socket.\n");
      return true;
    }

    tellf("The %s doesn't fit.\n",
          objects[current_cmd.prso_list[0]].description);
    return true;
  }
  return false;
}
// --- PLANETARY COURSE CONTROL ---

bool planetary_course_control_f(int arg) {
  if (arg == M_LOOK) {
    TELL("This is a long room whose walls are covered with complicated "
         "controls\n"
         "and colored lights. ");
    if (game_state.course_control_fixed) {
      TELL("One blinking light says \"Kors diivurjins minimiizeeng.\"\n");
    } else {
      TELL("Two of these lights are blinking. The first one reads \"Bedistur "
           "Faalyur!\"\n"
           "The other light reads \"Kritikul diivurjins frum pland kors.\"\n");
    }
    TELL(" In one corner is a large metal cube whose lid is ");
    if (obj_has_flag(O_CUBE, F_OPENBIT))
      TELL("open");
    else
      TELL("closed");
    TELL(".\n");
    return true;
  }
  return false;
}

bool cube_f(void) {
  if (current_cmd.verb == V_OPEN) {
    if (obj_has_flag(O_CUBE, F_OPENBIT)) {
      TELL("It is already open.\n");
    } else {
      obj_set_flag(O_CUBE, F_OPENBIT);
      TELL("The lid swings open.\n");
      // Could auto-look inside here
    }
    return true;
  }
  if (current_cmd.verb == V_CLOSE) {
    if (obj_has_flag(O_CUBE, F_OPENBIT)) {
      obj_clear_flag(O_CUBE, F_OPENBIT);
      TELL("The lid swings closed.\n");
    } else {
      TELL("It is already closed.\n");
    }
    return true;
  }
  if (current_cmd.verb == V_PUT && current_cmd.prsi == O_CUBE) {
    if (!obj_has_flag(O_CUBE, F_OPENBIT)) {
      TELL("The cube is closed.\n");
      return true;
    }
    if (obj_in(O_BAD_BEDISTOR, O_CUBE)) {
      TELL("There's a fused bedistor in the way.\n");
      return true;
    }
    // Logic for good bedistor
    if (current_cmd.prso_list[0] == O_GOOD_BEDISTOR) {
      obj_move(O_GOOD_BEDISTOR, O_CUBE);
      if (!obj_has_flag(O_CUBE, F_MUNGEDBIT)) { // Check if already munged
        game_state.course_control_fixed = true;
        obj_set_flag(O_GOOD_BEDISTOR, F_TRYTAKEBIT);
        // game_state.score += 6;
        TELL("Done. The warning lights go out and another light goes on.\n");
      } else {
        TELL("Done.\n");
      }
      return true;
    }
    if (current_cmd.prso_list[0] == O_BAD_BEDISTOR) {
      obj_move(O_BAD_BEDISTOR, O_CUBE);
      TELL("Done.\n");
      return true;
    }
    TELL("It doesn't fit.\n");
    return true;
  }
  return false;
}

bool bad_bedistor_f(int arg) {
  (void)arg;
  // TAKE while in CUBE - fused message
  if (current_cmd.verb == V_TAKE && obj_in(O_BAD_BEDISTOR, O_CUBE)) {
    TELL("It seems to be fused to its socket.\n");
    return true;
  }
  // ZATTRACT (pull with tool) - need PLIERS
  if (current_cmd.verb == V_ZATTRACT) {
    if (current_cmd.prsi == O_PLIERS) {
      obj_move(O_BAD_BEDISTOR, player);
      obj_clear_flag(O_BAD_BEDISTOR, F_TRYTAKEBIT);
      TELL("With a tug, you manage to remove the fused bedistor.\n");
      return true;
    } else {
      TELL("You can't get a grip on the bedistor with that.\n");
      return true;
    }
  }
  return false;
}

bool terminal_f(void) { return false; }
bool spool_reader_f(void) { return false; }

bool projcon_office_f(int arg) {
  if (arg == M_LOOK) {
    TELL("This office looks like a headquarters of some kind. Exits lead north "
         "and\n"
         "east. The west wall displays a logo. ");
    if (game_state.course_control_fixed) { // Assuming computed fixed logic
      TELL("The mural that previously adorned the south wall has slid away, "
           "revealing\n"
           "an open doorway to a large elevator!\n");
    } else {
      TELL("The south wall is completely covered by a garish mural which "
           "clashes\n"
           "with the other decor of the room.\n");
    }
    return true;
  }
  // Logic for Floyd noticing mural
  return false;
}

bool cryo_elevator_f(int arg) {
  if (arg == M_LOOK) {
    TELL("This is a large, plain elevator with one solitary button and a door\n"
         "to the north which is ");
    // DDESC O_CRYO_ELEVATOR_DOOR
    if (obj_has_flag(O_CRYO_ELEVATOR_DOOR, F_OPENBIT))
      TELL("open");
    else
      TELL("closed");
    TELL(".\n");
    return true;
  }
  return false;
}

bool cryo_anteroom_f(int arg) {
  if (arg == M_LOOK) {
    TELL("The elevator closes as you leave it, and you find yourself in a "
         "small,\n"
         "chilly room. To the north, through a wide arch, is an enormous "
         "chamber lined\n"
         "from floor to ceiling with thousands of cryo-units. You can see "
         "similar\n"
         "chambers beyond, and your mind staggers at the thought of the "
         "millions of\n"
         "individuals asleep for countless centuries.\n\n"
         "In the anteroom where you stand is a solitary cryo-unit, its cover "
         "frosted.\n"
         "Next to the cryo-unit is a complicated control panel.\n\n");
    return true;
  }
  if (arg == M_END) {
    // Cutscene logic here
  }
  return false;
}

bool bio_lock_east_f(int arg) { return false; }
bool bio_lab_f(int arg) { return false; }
bool radiation_lab_f(int arg) { return false; }
bool lab_office_f(int arg) { return false; }

// --- BIO LOCK DOORS ---

bool bio_door_east_f(void) {
  if (current_cmd.verb == V_OPEN) {
    if (obj_has_flag(O_BIO_DOOR_EAST, F_OPENBIT)) {
      TELL("It is already open.\n");
      return true;
    }
    if (obj_has_flag(O_BIO_DOOR_WEST, F_OPENBIT)) {
      TELL("A very bored-sounding recorded voice explains that, in order to "
           "prevent\n"
           "contamination, both lock doors cannot be open simultaneously.\n");
      return true;
    }
    // Special Floyd logic for foray
    if (game_state.floyd_waiting && obj_has_flag(O_FLOYD, F_RLANDBIT) &&
        game_state.foray_counter == 0) {
      // Start Floyd's foray into Bio Lab
      game_state.floyd_forayed = true;
      obj_set_flag(O_BIO_DOOR_EAST, F_OPENBIT);
      obj_remove(O_FLOYD);
      TELL("The door opens and Floyd, pausing only for the briefest moment, "
           "plunges into\n"
           "the Bio Lab. Immediately, he is set upon by hideous, mutated "
           "monsters! More\n"
           "are heading straight toward the open door! Floyd shrieks and yells "
           "to you to\n"
           "close the door.\n");
      // TODO: Enable I-FLOYD-FORAY event
      return true;
    }
    // If not Floyd scenario, dangerous
    if (!game_state.floyd_forayed) {
      TELL("Opening the door reveals a Bio-Lab full of horrible mutations. You "
           "stare at\n"
           "them, frozen with horror. Growling with hunger and delight, the "
           "mutations\n"
           "march into the bio-lock and devour you.\n");
      // TODO: JIGS-UP
      return true;
    }
    obj_set_flag(O_BIO_DOOR_EAST, F_OPENBIT);
    TELL("The door opens.\n");
    return true;
  }
  if (current_cmd.verb == V_CLOSE) {
    if (obj_has_flag(O_BIO_DOOR_EAST, F_OPENBIT)) {
      if (game_state.foray_counter == 4) {
        game_state.c_elapsed = 95; // Special timing
      }
      obj_clear_flag(O_BIO_DOOR_EAST, F_OPENBIT);
      TELL("The door closes.\n");
      return true;
    }
    TELL("It is already closed.\n");
    return true;
  }
  return false;
}

bool bio_door_west_f(void) {
  if (current_cmd.verb == V_OPEN) {
    if (obj_has_flag(O_BIO_DOOR_WEST, F_OPENBIT)) {
      TELL("It is already open.\n");
      return true;
    }
    if (obj_has_flag(O_BIO_DOOR_EAST, F_OPENBIT)) {
      TELL("A very bored-sounding recorded voice explains that, in order to "
           "prevent\n"
           "contamination, both lock doors cannot be open simultaneously.\n");
      return true;
    }
    obj_set_flag(O_BIO_DOOR_WEST, F_OPENBIT);
    TELL("The door opens.\n");
    return true;
  }
  if (current_cmd.verb == V_CLOSE) {
    if (obj_has_flag(O_BIO_DOOR_WEST, F_OPENBIT)) {
      obj_clear_flag(O_BIO_DOOR_WEST, F_OPENBIT);
      TELL("The door closes.\n");
    } else {
      TELL("It is already closed.\n");
    }
    return true;
  }
  return false;
}

bool rad_door_east_f(void) {
  if (current_cmd.verb == V_OPEN) {
    if (obj_has_flag(O_RAD_DOOR_EAST, F_OPENBIT)) {
      TELL("It is already open.\n");
    } else if (obj_has_flag(O_RAD_DOOR_WEST, F_OPENBIT)) {
      TELL("A very bored-sounding recorded voice explains that, in order to "
           "prevent\n"
           "contamination, both lock doors cannot be open simultaneously.\n");
    } else {
      obj_set_flag(O_RAD_DOOR_EAST, F_OPENBIT);
      TELL("The door opens.\n");
    }
    return true;
  }
  if (current_cmd.verb == V_CLOSE) {
    if (obj_has_flag(O_RAD_DOOR_EAST, F_OPENBIT)) {
      obj_clear_flag(O_RAD_DOOR_EAST, F_OPENBIT);
      TELL("The door closes.\n");
    } else {
      TELL("It is already closed.\n");
    }
    return true;
  }
  return false;
}

bool rad_door_west_f(void) {
  if (current_cmd.verb == V_OPEN) {
    if (obj_has_flag(O_RAD_DOOR_WEST, F_OPENBIT)) {
      TELL("It is already open.\n");
    } else if (obj_has_flag(O_RAD_DOOR_EAST, F_OPENBIT)) {
      TELL("A very bored-sounding recorded voice explains that, in order to "
           "prevent\n"
           "contamination, both lock doors cannot be open simultaneously.\n");
    } else {
      obj_set_flag(O_RAD_DOOR_WEST, F_OPENBIT);
      TELL("The door opens.\n");
    }
    return true;
  }
  if (current_cmd.verb == V_CLOSE) {
    if (obj_has_flag(O_RAD_DOOR_WEST, F_OPENBIT)) {
      obj_clear_flag(O_RAD_DOOR_WEST, F_OPENBIT);
      TELL("The door closes.\n");
    } else {
      TELL("It is already closed.\n");
    }
    return true;
  }
  return false;
}

// Stubs needed for linker only if not defined elsewhere
bool lab_uniform_f(void) { return false; }
bool combination_paper_f(void) { return false; }
bool lab_desk_f(void) { return false; }
bool light_button_f(void) { return false; }
bool dark_button_f(void) { return false; }
bool fungicide_button_f(void) { return false; }
bool lamp_f(void) { return false; }
bool cryo_exit_f(void) { return false; }

// Void stubs
void i_cryo_elevator_arrive(void) {}
void i_clear_floyd_peer(void) {}
void i_bio_east_closes(void) {}
void i_bio_west_closes(void) {}
void i_floyd_foray(void) {}
void monster_death(void) {}
void i_chase_scene(void) {}
void i_nuked_blue(void) {}
void i_unflood(void) {}
void i_turnoff_mini(void) {}
