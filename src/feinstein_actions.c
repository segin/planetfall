#include "actions.h"
#include "feinstein.h"
#include "ids.h"
#include "output.h"
#include "parser.h"
#include "syntax_gen.h"
#include <stdlib.h>

// Action routines for Feinstein Act rooms and objects

bool deck_nine_f(int arg) {
  if (arg == M_LOOK) {
    TELL("This is a featureless corridor similar to every other corridor on "
         "the ship.\n"
         "It curves away to starboard, and a gangway leads up");
    if (obj_has_flag(O_GANGWAY_DOOR, F_OPENBIT)) {
      TELL(".");
    } else {
      TELL(", but both of these are blocked by closed bulkheads.");
    }
    TELL(" To port is the entrance to one of the ship's primary escape pods. "
         "The\n"
         "pod bulkhead is ");
    if (obj_has_flag(O_POD_DOOR, F_OPENBIT))
      TELL("open");
    else
      TELL("closed");
    TELL(".\n");
    return true;
  }
  return false;
}

bool gangway_f(int arg) {
  if (arg == M_END) {
    // Random message about Ensign in danger
    if ((rand() % 100) < 15 && game_state.blowup_counter == 0) {
      TELL("\nYou hear a distant bellowing ... something about an Ensign "
           "Seventh Class\n"
           "whose life is in danger.\n");
    }
  }
  return false;
}

bool ground_f(void) {
  if (current_cmd.verb == V_PUT && current_cmd.prsi == O_GROUND) {
    // Redirect PUT ON GROUND to DROP
    TELL("Dropped.\n");
    return true;
  }
  if (current_cmd.verb == V_EXAMINE && current_room == R_ADMIN_CORRIDOR_S) {
    TELL("A narrow, jagged crevice runs across the floor.\n");
    return true;
  }
  return false;
}

bool chronometer_f(int arg) {
  (void)arg;
  if (current_cmd.verb == V_EXAMINE || current_cmd.verb == V_READ) {
    TELL("It is a standard wrist chronometer with a digital display. ");
    // Display time based on game_state.internal_moves
    int hour = 8 + (game_state.internal_moves / 60);
    int minute = (game_state.internal_moves % 60);
    tellf("According to the chronometer, the current time is %d:%02d.\n", hour, minute);
    TELL(" The back is engraved with\n"
         "the message \"Good luck in the Patrol! Love, Mom and Dad.\"\n");
    return true;
  }
  return false;
}

bool patrol_uniform_f(void) {
  if (current_cmd.verb == V_EXAMINE) {
    TELL("It is a standard-issue one-pocket Stellar Patrol uniform, a miracle "
         "of modern\n"
         "technology. It will keep its owner warm in cold climates and cool in "
         "warm\n"
         "locales. It provides protection against mild radiation, repels all "
         "insects,\n"
         "absorbs sweat, promotes healthy skin tone, and on top of everything "
         "else,\n"
         "it is super-comfy.\n");
    return true;
  }
  if (current_cmd.verb == V_TAKE_OFF &&
      obj_has_flag(O_PATROL_UNIFORM, F_WORNBIT)) {
    obj_clear_flag(O_PATROL_UNIFORM, F_WORNBIT);
    TELL("You have removed your Patrol uniform.");
    if (obj_in(O_BLATHER, current_room)) {
      TELL(" \"Removing your uniform while on duty? Five hundred demerits!\"");
    } else if (obj_in(O_FLOYD, current_room)) {
      TELL(" Floyd giggles. \"You look funny without any clothes on.\"");
    }
    TELL("\n");
    return true;
  }
  return false;
}

bool ambassador_f(int arg) {
  (void)arg; // Unused for now
  // TALK, HELLO, or interaction
  if (current_cmd.verb == V_TALK || current_cmd.verb == V_HELLO) {
    TELL("The ambassador taps his translator, and then touches his center knee "
         "to his\n"
         "left ear (the Blow'k-bibben-Gordoan equivalent of shrugging).\n");
    return true;
  }

  // ASK-FOR (Ask Ambassador For Celery)
  if (current_cmd.verb == V_ASK_FOR && current_cmd.prsi == O_CELERY) {
    TELL("The ambassador seems willing to let you eat some of it, but I doubt "
         "he wants\n"
         "to part with the entire stalk.\n");
    return true;
  }

  // ATTACK or KICK
  if (current_cmd.verb == V_ATTACK || current_cmd.verb == V_KICK) {
    TELL("The ambassador is startled, and emits an amazing quantity of slime "
         "which\n"
         "spreads across the section of the deck you just polished.\n");
    return true;
  }

  // EXAMINE
  if (current_cmd.verb == V_EXAMINE) {
    TELL("The ambassador has around twenty eyes, seven of which are currently\n"
         "open. Half of his six legs are retracted. Green slime oozes from\n"
         "multiple orifices in his scaly skin. He speaks through a mechanical\n"
         "translator slung around his neck.\n");
    return true;
  }

  // LISTEN
  if (current_cmd.verb == V_LISTEN) {
    TELL("The alien makes a wheezing noise as he breathes.\n");
    return true;
  }

  return false;
}
