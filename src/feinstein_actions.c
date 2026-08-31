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

// POD-DOOR-F (globals.zil). The bulkhead is never the player's to operate: it is
// blown open by the first explosion and clangs shut again when the pod launches.
// Without this the generic V_OPEN handler would cheerfully unlock the escape pod
// on turn one.
bool pod_door_f(int arg) {
  (void)arg;
  switch (current_cmd.verb) {
  case V_OPEN:
    if (obj_has_flag(O_POD_DOOR, F_OPENBIT)) {
      TELL("It's already open!\n");
    } else if (game_state.trip_counter > 14) {
      // Down on the ocean floor, opening up is a way to drown.
      obj_set_flag(O_POD_DOOR, F_OPENBIT);
      TELL("The bulkhead opens and cold ocean water rushes in!\n");
    } else if (game_state.blowup_counter > 0) {
      if (current_room == R_DECK_NINE) {
        TELL("Too late. The pod's launching procedure has already begun.\n");
      } else {
        TELL("Opening the door now would be a phenomenally stupid idea.\n");
      }
    } else {
      TELL("Why open the door to the emergency escape pod if there's no "
           "emergency?\n");
    }
    return true;
  case V_CLOSE:
    if (!obj_has_flag(O_POD_DOOR, F_OPENBIT)) {
      TELL("It is closed!\n");
    } else {
      TELL("You can't close it yourself.\n");
    }
    return true;
  case V_THROUGH:
    if (current_room == R_DECK_NINE) {
      perform_walk(objects[R_DECK_NINE].west);
    } else {
      perform_walk(objects[current_room].out);
    }
    return true;
  default:
    return false;
  }
}

// GANGWAY-DOOR-F (globals.zil), shared by the narrow and wide emergency
// bulkheads. Both start open and invisible and are slammed shut by the second
// wave of explosions; the player has no way to work them.
bool gangway_door_f(int arg) {
  (void)arg;
  ZObjectID self = current_cmd.prso_list[0];
  if (self != O_GANGWAY_DOOR && self != O_CORRIDOR_DOOR)
    return false;

  switch (current_cmd.verb) {
  case V_OPEN:
    if (obj_has_flag(self, F_OPENBIT)) {
      TELL("It's already open!\n");
    } else {
      TELL("There doesn't seem to be any way to open it.\n");
    }
    return true;
  case V_CLOSE:
    if (obj_has_flag(self, F_OPENBIT)) {
      TELL("You can't close it yourself.\n");
    } else {
      TELL("It is closed!\n");
    }
    return true;
  default:
    return false;
  }
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
    // TELL-TIME (globals.zil): Galactic Standard Time prints as a bare count.
    tellf("According to the chronometer, the current time is %d.",
          obj_has_flag(O_CHRONOMETER, F_MUNGEDBIT) ? game_state.munged_time
                                                   : game_state.internal_moves);
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

// BLATHER-F (globals.zil).
bool blather_f(int arg) {
  (void)arg;
  switch (current_cmd.verb) {
  case V_TALK:
  case V_HELLO:
    TELL("Blather shouts \"Speak when you're spoken to, Ensign Seventh "
         "Class!\" He\n"
         "breaks three pencil points in a frenzied rush to give you more "
         "demerits.\n");
    return true;
  case V_ATTACK:
  case V_KICK:
    jigs_up("Blather removes several of your appendages and internal organs.");
    return true;
  case V_SALUTE:
    TELL("Blather's sneer softens a bit. \"First right thing you've done "
         "today. Only\n"
         "five demerits.\"\n");
    return true;
  case V_THROW:
    // Only when Blather is what you threw the thing AT.
    if (current_cmd.prsi == O_BLATHER) {
      ZObjectID missile = current_cmd.prso_list[0];
      obj_move(missile, current_room);
      tellf("The %s bounces off Blather's bulbous nose. He becomes livid, "
            "orders\n"
            "you to do five hundred push-ups, gives you ten thousand demerits, "
            "and assigns\n"
            "you five years of extra galley duty.\n",
            objects[missile].description);
      return true;
    }
    return false;
  case V_EXAMINE:
    TELL("Ensign Blather is a tall, beefy officer with a tremendous, "
         "misshapen nose.\n"
         "His uniform is perfect in every respect, and the crease in his "
         "trousers\n"
         "could probably slice diamonds in half.\n");
    return true;
  case V_TAKE:
    TELL("Blather brushes you away, muttering about suspended shore leave.\n");
    return true;
  default:
    return false;
  }
}

// CELERY-F (globals.zil). The ambassador's snack is lethal to humans.
bool celery_f(int arg) {
  (void)arg;
  switch (current_cmd.verb) {
  case V_EAT:
    jigs_up("Oops. Looks like Blow'k-Bibben-Gordoan metabolism is not\n"
            "compatible with our own. You die of all sorts of convulsions.");
    return true;
  case V_TAKE:
    TELL("The ambassador seems perturbed by your lack of normal protocol.\n");
    return true;
  default:
    return false;
  }
}

// LIKE-SLIME (globals.zil).
static void like_slime(const char *verbed) {
  tellf("It %s like slime. Aren't you glad you didn't step in it?\n", verbed);
}

// SLIME-PSEUDO (globals.zil). The trail only exists once the ambassador has
// come through; before that there is nothing on the deck to poke at.
bool slime_f(int arg) {
  (void)arg;
  if (!obj_in(O_AMBASSADOR, current_room) &&
      game_state.ambassador_leave_counter == 0) {
    return false;
  }

  switch (current_cmd.verb) {
  case V_EAT:
  case V_TASTE:
    like_slime("tastes");
    return true;
  case V_TAKE:
  case V_RUB:
    like_slime("feels");
    return true;
  case V_EXAMINE:
    like_slime("looks");
    return true;
  case V_SMELL:
    like_slime("smells");
    return true;
  default:
    return false;
  }
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
