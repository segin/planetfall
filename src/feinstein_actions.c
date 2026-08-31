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

// CRETIN-F (globals.zil): the player as an object of their own commands.
bool cretin_f(int arg) {
  (void)arg;
  switch (current_cmd.verb) {
  case V_GIVE:
    // GIVE something to yourself is just taking it.
    return perform(V_TAKE, current_cmd.prso_list[0], NOTHING);
  case V_SCRUB:
    TELL("If only you'd done that before the last inspection, you wouldn't "
         "have\n"
         "gotten 300 demerits.\n");
    return true;
  case V_DROP:
    TELL("Huh?\n");
    return true;
  case V_SMELL:
    TELL("Phew!\n");
    return true;
  case V_FOLLOW:
    TELL("It would be hard not to.\n");
    return true;
  case V_EAT:
    TELL("Auto-cannibalism is not the answer.\n");
    return true;
  case V_ATTACK:
  case V_MUNG:
    if (current_cmd.prso_list[0] == player) {
      jigs_up("If you insist.... Poof, you're dead!");
    } else {
      TELL("What a silly idea!\n");
    }
    return true;
  case V_TAKE:
    TELL("How romantic!\n");
    return true;
  case V_DISEMBARK:
    TELL("You'll have to do that on your own.\n");
    return true;
  case V_EXAMINE:
    TELL("That's difficult unless your eyes are prehensile.\n");
    return true;
  default:
    return false;
  }
}

// HANDS-F (globals.zil).
bool hands_f(int arg) {
  (void)arg;
  if (current_cmd.verb != V_SHAKE)
    return false;

  if (obj_in(O_AMBASSADOR, current_room)) {
    TELL("A repulsive idea.\n");
  } else if (obj_in(O_BLATHER, current_room)) {
    TELL("Saluting might be a better idea.\n");
  } else if (obj_in(O_FLOYD, current_room) &&
             obj_has_flag(O_FLOYD, F_RLANDBIT)) {
    TELL("You shake one of Floyd's grasping extensions.\n");
  } else {
    TELL("There's no one to shake hands with.\n");
  }
  return true;
}

// GRAFFITI-PSEUDO (globals.zil). Reading the walls of the brig takes a while.
bool graffiti_f(int arg) {
  (void)arg;
  if (current_cmd.verb == V_READ || current_cmd.verb == V_EXAMINE) {
    game_state.c_elapsed = 28;
    TELL("All the graffiti seem to be about Blather. One of\n"
         "the least obscene items reads:\n\n"
         "There once was a krip, name of Blather\n"
         "Who told a young Ensign named Smather\n"
         "\"I'll make you inherit\n"
         "A trotting demerit\n"
         "And ship you off to those stinking fawg-infested tar-pools of "
         "Krather.\"\n\n"
         "It's not a very good limerick, is it?\n");
    return true;
  }
  return false;
}

// DOOR-PSEUDO (globals.zil): the brig's cell door.
bool brig_door_f(int arg) {
  (void)arg;
  if (current_cmd.verb == V_OPEN || current_cmd.verb == V_UNLOCK) {
    TELL("No way, Jose.\n");
    return true;
  }
  return false;
}

// WINDOW-F (globals.zil). Only the escape pod and the generic branches are
// ported so far; the Bio Lock, shuttle cabins and balcony come with their
// chapters.
bool window_f(int arg) {
  (void)arg;
  switch (current_cmd.verb) {
  case V_LOOK_INSIDE:
    if (current_room == R_ESCAPE_POD) {
      if (game_state.trip_counter < 2) {
        TELL("You can see debris from the exploding Feinstein.\n");
      } else if (game_state.trip_counter > 8) {
        TELL("You can see a planet, hopefully a hospitable one.\n");
      } else {
        TELL("The viewport is polarized into a featureless black rectangle.\n");
      }
      return true;
    } else if (current_room == R_HELICOPTER) {
      TELL("You see the helipad and the ocean beyond.\n");
      return true;
    }
    return false;
  case V_OPEN:
    TELL("This window doesn't open.\n");
    return true;
  case V_MUNG:
    TELL("It's made of tough Zynoid plastic.\n");
    return true;
  default:
    return false;
  }
}

// LIGHTS-F (globals.zil).
bool lights_f(int arg) {
  (void)arg;
  if (current_cmd.verb == V_EXAMINE && current_room == R_COMPUTER_ROOM) {
    TELL("The red light would seem to indicate a malfunction in the "
         "computer.\n");
    return true;
  }
  return false;
}

// DDESC (globals.zil): renders a door's state for inline use in descriptions.
const char *ddesc(ZObjectID door) {
  return obj_has_flag(door, F_OPENBIT) ? "open" : "closed";
}

// ESCAPE-POD-F (globals.zil).
bool escape_pod_f(int arg) {
  if (arg == M_LOOK) {
    tellf("This is one of the Feinstein's primary escape pods, for use in "
          "extreme\n"
          "emergencies. A mass of safety webbing, large enough to hold several "
          "dozen\n"
          "people, fills half the pod. The controls are entirely automated. "
          "The\n"
          "bulkhead leading out is %s.\n",
          ddesc(O_POD_DOOR));
    return true;
  }
  return false;
}

// SAFETY-WEB-F (globals.zil). Riding out the launch in the webbing is the whole
// point of the pod; standing up again once you have landed drops the pod off
// its ledge and starts it sinking.
bool safety_web_f(int arg) {
  (void)arg;
  switch (current_cmd.verb) {
  case V_EXAMINE:
    TELL("The safety webbing fills most of the pod. It could accomodate\n"
         "from one to, perhaps, twenty people.\n");
    return true;
  case V_TAKE:
    if (obj_in(player, O_SAFETY_WEB)) {
      TELL("You're in it!\n");
    } else {
      TELL("The safety web seems to be more intended for getting into than\n"
           "grabbing onto.\n");
    }
    return true;
  case V_BOARD:
  case V_CLIMB_ON:
    if (obj_in(player, O_SAFETY_WEB)) {
      TELL("You're in it!\n");
    } else {
      obj_move(player, O_SAFETY_WEB);
      TELL("You are now safely cushioned within the web.\n");
    }
    return true;
  case V_EXIT:
  case V_DISEMBARK:
  case V_STAND:
    if (!obj_in(player, O_SAFETY_WEB))
      return false;
    obj_move(player, current_room);
    if (game_state.trip_counter > 14 && !is_event_enabled(EVT_SINK_POD)) {
      queue_event(EVT_SINK_POD, -1);
      TELL("As you stand, the pod shifts slightly and you feel it falling.\n"
           "A moment later, the fall stops with a shock, and you see water\n"
           "rising past the viewport.\n");
    } else {
      TELL("You are standing again.\n");
    }
    return true;
  default:
    return false;
  }
}

// CONTROLS-F (globals.zil).
bool controls_f(int arg) {
  (void)arg;
  switch (current_cmd.verb) {
  case V_RUB:
  case V_MOVE:
  case V_TURN:
  case V_SET:
  case V_TAKE:
  case V_EXAMINE:
  case V_PUSH:
  case V_PULL:
    if (current_room == R_HELICOPTER) {
      TELL("The controls are covered and locked.\n");
    } else if (current_room == R_ESCAPE_POD) {
      TELL("The controls are entirely automated.\n");
    } else {
      TELL("The controls are incredibly complicated and you shouldn't even\n"
           "be thinking about touching them.\n");
    }
    return true;
  case V_OPEN:
  case V_UNLOCK:
    if (current_room == R_HELICOPTER) {
      TELL("You don't even have the orange key!\n");
      return true;
    }
    return false;
  default:
    return false;
  }
}

// FOOD-KIT-F (globals.zil).
bool food_kit_f(int arg) {
  (void)arg;
  if (current_cmd.verb == V_EMPTY) {
    if (!obj_has_flag(O_FOOD_KIT, F_OPENBIT)) {
      TELL("The kit is closed!\n");
    } else if (obj_first_child(O_FOOD_KIT) != NOTHING) {
      TELL("The goo, being gooey, sticks to the inside of the kit. You would "
           "probably\n"
           "have to shake the kit to get the goo out.\n");
    }
    return true;
  }
  return false;
}

// GOO-F (globals.zil). The goo is edible but not portable -- it has to be eaten
// straight out of the survival kit.
bool goo_f(int arg) {
  (void)arg;
  ZObjectID self = current_cmd.prso_list[0];

  switch (current_cmd.verb) {
  case V_EAT:
    if (game_state.hunger_level == 0) {
      TELL("Thanks, but you're not hungry.\n");
    } else if (!obj_in(O_FOOD_KIT, player)) {
      tellf("You're not holding the %s.\n", objects[O_FOOD_KIT].description);
    } else {
      obj_remove(self);
      game_state.c_elapsed = 15;
      game_state.hunger_level = 0;
      queue_event(EVT_HUNGER_WARNINGS, 1450);
      tellf("Mmmm...that tasted just like %s.\n",
            self == O_BROWN_GOO  ? "delicious Nebulan fungus pudding"
            : self == O_RED_GOO  ? "scrumptious cherry pie"
                                 : "yummy lima beans");
    }
    return true;
  case V_TAKE:
    TELL("It would ooze through your fingers. You'll have to eat it right from "
         "the survival kit.\n");
    return true;
  case V_DROP:
    TELL("The goo, being gooey, sticks where it is. You'll have to eat it right "
         "from the survival kit.\n");
    return true;
  default:
    return false;
  }
}

// TOWEL-F (globals.zil).
bool towel_f(int arg) {
  (void)arg;
  if (current_cmd.verb == V_EXAMINE) {
    TELL("A pretty ordinary towel. Something is written in its corner.\n");
    return true;
  }
  return false;
}

// GLOBAL-POD-F (globals.zil): the pod as seen from outside, so that ENTER POD
// and friends route through the bulkhead rather than teleporting you inside.
bool global_pod_f(int arg) {
  (void)arg;
  switch (current_cmd.verb) {
  case V_THROUGH:
  case V_BOARD:
  case V_WALK_TO:
    if (current_room == R_ESCAPE_POD) {
      TELL("You're already in it!\n");
    } else {
      perform_walk(objects[current_room].west);
    }
    return true;
  case V_EXIT:
  case V_DISEMBARK:
  case V_DROP:
    if (current_room == R_DECK_NINE) {
      TELL("You're not in it!\n");
      return true;
    }
    if (current_room == R_ESCAPE_POD) {
      perform_walk(objects[current_room].out);
      return true;
    }
    return false;
  case V_OPEN:
    return pod_door_f(arg);
  default:
    return false;
  }
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
