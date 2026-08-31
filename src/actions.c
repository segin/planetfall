#include "actions.h"
#include "complexone_actions.h"
#include "events.h"
#include "output.h"
#include "parser.h"
#include "savegame.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

// Helper: Weight
int get_weight(ZObjectID obj) {
  if (obj == NOTHING)
    return 0;
  int wt = objects[obj].size;
  ZObjectID child = objects[obj].child;
  while (child != NOTHING) {
    if (!obj_has_flag(child, F_WORNBIT)) {
      wt += get_weight(child);
    }
    child = objects[child].sibling;
  }
  return wt;
}

int count_contents(ZObjectID obj) {
  int cnt = 0;
  ZObjectID child = objects[obj].child;
  while (child != NOTHING) {
    cnt++;
    child = objects[child].sibling;
  }
  return cnt;
}



static const char *yuks[] = {
    "Fat chance.",
    "A valiant attempt.",
    "You can't be serious.",
    "Not bloody likely.",
    "An interesting idea...",
    "What a concept!"
};
#define NUM_YUKS (sizeof(yuks) / sizeof(yuks[0]))

static const char *pick_one_yuk() {
  return yuks[rand() % NUM_YUKS];
}

bool is_held(ZObjectID obj) {
  if (obj <= 0 || obj >= MAX_OBJECTS)
    return false;
  ZObjectID p = objects[obj].parent;
  while (p != NOTHING) {
    if (p == player)
      return true;
    p = objects[p].parent;
  }
  return false;
}

bool pre_take(ZObjectID obj, ZObjectID prsi) {
  if (obj_in(obj, player)) {
    tellf("You already have it.\n");
    return false;
  }
  if (obj == game_state.spout_placed && obj_has_flag(obj, F_NDESCBIT)) {
    return true;
  }
  ZObjectID loc = objects[obj].parent;
  if (loc != NOTHING && obj_has_flag(loc, F_CONTBIT) &&
      !obj_has_flag(loc, F_OPENBIT)) {
    tellf("You can't reach into a closed container.\n");
    return false;
  }
  if (prsi != NOTHING) {
    if (prsi != loc) {
      if (obj == O_CELERY && prsi == O_AMBASSADOR) {
        // Celery on ambassador special case
      } else {
        tellf("It's not in that!\n");
        return false;
      }
    }
  }
  if (obj == objects[player].parent) {
    tellf("You are in it, asteroid-brain!\n");
    return false;
  }
  return true;
}

#define FUMBLE_NUMBER 7
#define FUMBLE_PROB 8

bool itake(ZObjectID obj, bool verbose) {
  if (!obj_has_flag(obj, F_TAKEBIT)) {
    if (verbose)
      tellf("%s\n", pick_one_yuk());
    return false;
  }

  int player_load = get_weight(player);
  int obj_weight = get_weight(obj);

  if (!obj_in(objects[obj].parent, player)) {
    if (player_load + obj_weight > game_state.load_allowed) {
      if (verbose)
        tellf("Your load is too heavy.\n");
      return false;
    }
  }

  int cnt = count_contents(player);
  if (cnt > FUMBLE_NUMBER && (rand() % 100) < (cnt * FUMBLE_PROB)) {
    ZObjectID drop_obj = objects[player].child;
    while (drop_obj != NOTHING && obj_has_flag(drop_obj, F_WORNBIT)) {
      drop_obj = objects[drop_obj].sibling;
    }
    if (drop_obj != NOTHING) {
      tellf("Oh, no. The %s slips from your arms while taking the %s and both tumble to the ground.\n",
            objects[drop_obj].description, objects[obj].description);
      if ((drop_obj == O_FLASK || obj == O_FLASK) &&
          obj_in(O_CHEMICAL_FLUID, O_FLASK)) {
        obj_remove(O_CHEMICAL_FLUID);
        tellf("Unfortunately, the chemical spills out of the flask and evaporates.\n");
      }
      if ((drop_obj == O_CANTEEN || obj == O_CANTEEN) &&
          obj_in(O_HIGH_PROTEIN, O_CANTEEN) &&
          obj_has_flag(O_CANTEEN, F_OPENBIT)) {
        obj_remove(O_HIGH_PROTEIN);
        tellf("To make matters worse, the high-protein liquid spills all over the place and then evaporates.\n");
      }
      obj_move(drop_obj, current_room);
      obj_move(obj, current_room);
      return false;
    }
  }

  obj_move(obj, player);
  obj_clear_flag(obj, F_NDESCBIT);
  score_obj(obj);
  obj_set_flag(obj, F_TOUCHBIT);
  if (obj == game_state.spout_placed && game_state.spout_placed != NOTHING) {
    game_state.spout_placed = NOTHING;
  }
  return true;
}

bool trytake(ZObjectID obj) {
  if (obj_in(obj, player))
    return true;
  if (obj_has_flag(obj, F_TRYTAKEBIT) && objects[obj].action) {
    if (objects[obj].action(V_TAKE))
      return true;
  }
  return itake(obj, true);
}

void perform_take(ZObjectID obj) {
  if (!pre_take(obj, NOTHING)) {
    return;
  }
  if (objects[obj].action && objects[obj].action(V_TAKE)) {
    return;
  }
  if (itake(obj, true)) {
    tellf("Taken.\n");
  }
}

bool pre_put(ZObjectID prso, ZObjectID prsi) {
  if (prso == NOTHING)
    return true;
  if (obj_has_flag(prso, F_WORNBIT)) {
    tellf("You can't while you're wearing it.\n");
    return false;
  }
  if (obj_in(prso, OBJ_GLOBAL_OBJECTS) || !obj_has_flag(prso, F_TAKEBIT)) {
    tellf("Nice try.\n");
    return false;
  }
  return true;
}

void perform_put(ZObjectID prso, ZObjectID prsi) {
  if (!pre_put(prso, prsi))
    return;

  if (prsi == NOTHING) {
    tellf("You can't do that.\n");
    return;
  }

  if (objects[prsi].action && objects[prsi].action(V_PUT)) {
    return;
  }

  if (!obj_has_flag(prsi, F_OPENBIT) &&
      !obj_has_flag(prsi, F_DOORBIT) &&
      !obj_has_flag(prsi, F_CONTBIT) &&
      !obj_has_flag(prsi, F_VEHBIT)) {
    tellf("You can't do that.\n");
    return;
  }

  if (!obj_has_flag(prsi, F_OPENBIT)) {
    tellf("The %s isn't open.\n", objects[prsi].description);
    return;
  }

  if (prsi == prso) {
    tellf("How can you do that?\n");
    return;
  }

  if (obj_in(prso, prsi)) {
    tellf("The %s is already in the %s.\n", objects[prso].description,
          objects[prsi].description);
    return;
  }

  if (obj_in(prsi, prso)) {
    tellf("How can you put the %s in the %s when the %s is already in the %s?\n",
          objects[prso].description, objects[prsi].description,
          objects[prsi].description, objects[prso].description);
    return;
  }

  int prsi_weight = get_weight(prsi);
  int prso_weight = get_weight(prso);
  if (prsi_weight + prso_weight - objects[prsi].size > objects[prsi].capacity &&
      objects[prsi].capacity > 0) {
    tellf("There's no room.\n");
    return;
  }

  if (!obj_in(prso, player) && !trytake(prso)) {
    return;
  }

  score_obj(prso);
  obj_move(prso, prsi);
  obj_set_flag(prso, F_TOUCHBIT);
  tellf("Done.\n");
}

void perform_slide() {
  tellf("%s\n", pick_one_yuk());
}

bool pre_give(ZObjectID prso, ZObjectID prsi) {
  if (!is_held(prso)) {
    tellf("You're not holding the %s.\n", objects[prso].description);
    return false;
  }
  return true;
}

void perform_give(ZObjectID prso, ZObjectID prsi) {
  if (!pre_give(prso, prsi))
    return;

  if (prsi != NOTHING && objects[prsi].action && objects[prsi].action(V_GIVE))
    return;
  if (prso != NOTHING && objects[prso].action && objects[prso].action(V_GIVE))
    return;

  if (!obj_has_flag(prsi, F_ACTORBIT)) {
    const char *art1 = obj_has_flag(prso, F_VOWELBIT) ? "an" : "a";
    const char *art2 = obj_has_flag(prsi, F_VOWELBIT) ? "an" : "a";
    tellf("You can't give %s %s to %s %s!\n", art1, objects[prso].description,
          art2, objects[prsi].description);
    return;
  }
  tellf("The %s declines your offer.\n", objects[prsi].description);
}

void perform_sgive(ZObjectID prso, ZObjectID prsi) {
  perform_give(prsi, prso);
}

bool idrop(ZObjectID obj) {
  if (!is_held(obj)) {
    tellf("You're not carrying the %s.\n", objects[obj].description);
    return false;
  }
  if (obj_has_flag(obj, F_WORNBIT)) {
    tellf("You'll have to take it off, first.\n");
    return false;
  }
  if (!obj_in(obj, player) && !obj_has_flag(objects[obj].parent, F_OPENBIT)) {
    tellf("The %s is closed.\n", objects[objects[obj].parent].description);
    return false;
  }

  obj_move(obj, current_room);
  return true;
}

void perform_drop(ZObjectID obj) {
  if (objects[obj].action && objects[obj].action(V_DROP))
    return;
  if (idrop(obj)) {
    tellf("Dropped.\n");
  }
}

void perform_throw(ZObjectID obj, ZObjectID prsi) {
  if (objects[obj].action && objects[obj].action(V_THROW))
    return;
  if (idrop(obj)) {
    tellf("Thrown.\n");
  }
}

bool see_inside(ZObjectID obj) {
  if (obj <= 0 || obj >= MAX_OBJECTS)
    return false;
  if (obj_has_flag(obj, F_INVISIBLE))
    return false;
  return obj_has_flag(obj, F_TRANSBIT) || obj_has_flag(obj, F_OPENBIT);
}

bool global_in(ZObjectID obj, ZObjectID room) {
  if (room <= 0 || room >= MAX_OBJECTS)
    return false;
  for (int i = 0; i < 10; i++) {
    if (objects[room].globals[i] == obj && obj != NOTHING)
      return true;
  }
  return false;
}

bool is_here(ZObjectID obj) {
  if (obj <= 0 || obj >= MAX_OBJECTS)
    return false;
  if (obj == current_room)
    return true;
  if (obj_in(obj, OBJ_GLOBAL_OBJECTS))
    return true;
  if (obj_in(obj, OBJ_LOCAL_GLOBALS) && global_in(obj, current_room))
    return true;
  if (global_in(obj, current_room))
    return true;

  ZObjectID p = objects[obj].parent;
  while (p != NOTHING) {
    if (p == current_room || p == player)
      return true;
    p = objects[p].parent;
  }
  return false;
}

bool pre_examine(ZObjectID obj) {
  if (!is_here(obj)) {
    tellf("You can't see any %s here!\n", objects[obj].description);
    return false;
  }
  return true;
}

void perform_look_inside(ZObjectID obj) {
  if (obj_has_flag(obj, F_ACTORBIT)) {
    tellf("There is nothing special to be seen.\n");
  } else if (obj_has_flag(obj, F_DOORBIT)) {
    if (obj_has_flag(obj, F_OPENBIT)) {
      tellf("The %s is open, but I can't tell what's beyond it.\n",
            objects[obj].description);
    } else {
      tellf("The %s is closed.\n", objects[obj].description);
    }
  } else if (obj_has_flag(obj, F_CONTBIT)) {
    if (!obj_has_flag(obj, F_OPENBIT)) {
      tellf("The %s is closed.\n", objects[obj].description);
    } else if (see_inside(obj)) {
      if (objects[obj].child != NOTHING && print_cont(obj, false, 0)) {
        return;
      } else if (obj_has_flag(obj, F_SURFACEBIT)) {
        tellf("There is nothing on the %s.\n", objects[obj].description);
      } else {
        tellf("The %s is empty.\n", objects[obj].description);
      }
    } else {
      tellf("The %s is closed.\n", objects[obj].description);
    }
  } else if (obj_has_flag(obj, F_TRANSBIT)) {
    tellf("You can see dimly through the %s.\n", objects[obj].description);
  } else {
    const char *art = obj_has_flag(obj, F_VOWELBIT) ? "an" : "a";
    tellf("You can't look inside %s %s.\n", art, objects[obj].description);
  }
}

void perform_examine(ZObjectID obj) {
  game_state.c_elapsed = 32;
  if (!pre_examine(obj))
    return;

  if (objects[obj].action && objects[obj].action(V_EXAMINE)) {
    return;
  }

  if (objects[obj].text) {
    tellf("%s\n", objects[obj].text);
  } else if (obj_has_flag(obj, F_DOORBIT)) {
    perform_look_inside(obj);
  } else if (obj_has_flag(obj, F_CONTBIT)) {
    if (obj_has_flag(obj, F_OPENBIT)) {
      perform_look_inside(obj);
    } else {
      tellf("The %s is closed.\n", objects[obj].description);
    }
  } else {
    tellf("I see nothing special about the %s.\n", objects[obj].description);
  }
}

bool pre_read(ZObjectID prso) {
  if (!is_lit(current_room)) {
    tellf("It is impossible to read in the dark.\n");
    return false;
  }
  return true;
}

void perform_read(ZObjectID obj) {
  if (!pre_read(obj))
    return;
  if (obj != NOTHING && objects[obj].action && objects[obj].action(V_READ))
    return;
  if (!obj_has_flag(obj, F_READBIT) || objects[obj].text == NULL) {
    const char *art = obj_has_flag(obj, F_VOWELBIT) ? "an" : "a";
    tellf("How can I read %s %s?\n", art, objects[obj].description);
    return;
  }
  game_state.c_elapsed = 18;
  tellf("%s\n", objects[obj].text);
}

bool is_lit(ZObjectID room) {
  if (obj_has_flag(room, F_ONBIT) || obj_has_flag(room, F_LIGHTBIT))
    return true;

  ZObjectID child = objects[player].child;
  while (child != NOTHING) {
    if (obj_has_flag(child, F_ONBIT) || obj_has_flag(child, F_LIGHTBIT))
      return true;
    child = objects[child].sibling;
  }

  child = objects[room].child;
  while (child != NOTHING) {
    if (obj_has_flag(child, F_ONBIT) || obj_has_flag(child, F_LIGHTBIT))
      return true;
    child = objects[child].sibling;
  }

  return false;
}

bool describe_room(bool look) {
  bool v = look || game_state.verbose;

  if (!is_lit(current_room)) {
    tellf("It is pitch black. You might be eaten by a grue.\n");
    if (current_room == R_TRANSPORTATION_SUPPLY) {
      tellf("There is light to the south.\n");
    }
    return false;
  }

  if (!obj_has_flag(current_room, F_TOUCHBIT)) {
    obj_set_flag(current_room, F_TOUCHBIT);
    v = true;
  }

  tellf("%s", objects[current_room].description);
  ZObjectID av = objects[player].parent;
  if (av != current_room && av != NOTHING && obj_has_flag(av, F_VEHBIT)) {
    tellf(", in the %s", objects[av].description);
  }
  tellf("\n");

  if (look || !game_state.super_brief) {
    if (v) {
      if (objects[current_room].action && objects[current_room].action(M_LOOK)) {
        // Handled by room action
      } else if (objects[current_room].long_description) {
        tellf("%s\n", objects[current_room].long_description);
      }
    } else {
      if (objects[current_room].action) {
        objects[current_room].action(M_FLASH);
      }
    }

    if (av != current_room && av != NOTHING && obj_has_flag(av, F_VEHBIT)) {
      if (objects[av].action) {
        objects[av].action(M_LOOK);
      }
    }
  }

  return true;
}

bool firster(ZObjectID obj, int level) {
  if (obj == player) {
    tellf("You are carrying:\n");
    return true;
  }
  if (obj != current_room && !obj_in(obj, OBJ_ROOMS)) {
    if (level > 0) {
      tellf("%s", indents[level < 6 ? level : 5]);
    }
    if (obj_has_flag(obj, F_SURFACEBIT)) {
      tellf("Sitting on the %s is:\n", objects[obj].description);
    } else if (obj_has_flag(obj, F_ACTORBIT)) {
      tellf("The %s is holding:\n", objects[obj].description);
    } else {
      tellf("The %s contains:\n", objects[obj].description);
    }
    return true;
  }
  return false;
}

bool describe_object(ZObjectID obj, bool v, int level) {
  if (level == 0 && objects[obj].action && objects[obj].action(M_OBJDESC)) {
    return true;
  }
  if (obj == game_state.spout_placed && game_state.spout_placed != NOTHING) {
    return true;
  }

  if (level == 0) {
    if (!obj_has_flag(obj, F_TOUCHBIT) && objects[obj].long_description) {
      tellf("%s", objects[obj].long_description);
    } else {
      const char *art = obj_has_flag(obj, F_VOWELBIT) ? "an" : "a";
      tellf("There is %s %s here.", art, objects[obj].description);
    }
  } else {
    tellf("%s", indents[level < 6 ? level : 5]);
    const char *art = obj_has_flag(obj, F_VOWELBIT) ? "An" : "A";
    tellf("%s %s", art, objects[obj].description);
    if (obj_has_flag(obj, F_WORNBIT)) {
      tellf(" (being worn)");
    }
  }

  ZObjectID av = objects[player].parent;
  if (level == 0 && av != current_room && av != NOTHING &&
      obj_has_flag(av, F_VEHBIT)) {
    tellf(" (outside the %s)", objects[av].description);
  }
  tellf("\n");

  if (see_inside(obj) && objects[obj].child != NOTHING) {
    print_cont(obj, v, level);
  }
  return true;
}

bool print_cont(ZObjectID obj, bool v, int level) {
  ZObjectID y = objects[obj].child;
  if (y == NOTHING)
    return true;

  ZObjectID av = objects[player].parent;
  if (av != NOTHING && !obj_has_flag(av, F_VEHBIT)) {
    av = NOTHING;
  }

  bool first = true;
  bool pv = false;
  bool inv = (obj == player || objects[obj].parent == player);

  if (!inv) {
    // Pass 1: Objects with initial/first descriptions (FDESC)
    y = objects[obj].child;
    while (y != NOTHING) {
      if (y == av) {
        pv = true;
      } else if (y != player) {
        if (!obj_has_flag(y, F_INVISIBLE) &&
            !obj_has_flag(y, F_TOUCHBIT) &&
            objects[y].long_description) {
          if (!obj_has_flag(y, F_NDESCBIT)) {
            tellf("%s\n", objects[y].long_description);
          }
          if (see_inside(y) && (!objects[obj].action) &&
              objects[y].child != NOTHING) {
            print_cont(y, v, 0);
          }
        }
      }
      y = objects[y].sibling;
    }
  }

  // Pass 2: Remaining objects
  y = objects[obj].child;
  while (y != NOTHING) {
    if (y != av && y != player) {
      if (!obj_has_flag(y, F_INVISIBLE) &&
          (inv || obj_has_flag(y, F_TOUCHBIT) ||
           !objects[y].long_description)) {
        if (!obj_has_flag(y, F_NDESCBIT)) {
          if (first) {
            if (firster(obj, level)) {
              if (level < 0)
                level = 0;
            }
            level = level + 1;
            first = false;
          }
          describe_object(y, v, level);
        } else if (objects[y].child != NOTHING && see_inside(y)) {
          print_cont(y, v, level);
        }
      }
    }
    y = objects[y].sibling;
  }

  if (pv && av != NOTHING && objects[av].child != NOTHING) {
    print_cont(av, v, level);
  }

  return !first;
}

bool describe_objects(bool look) {
  if (!is_lit(current_room)) {
    tellf("You can't see anything in the dark.\n");
    return false;
  }

  bool v = look || game_state.verbose;
  if (objects[current_room].child != NOTHING) {
    print_cont(current_room, v, -1);
  }
  return true;
}

void perform_look() {
  game_state.c_elapsed = 9;
  if (describe_room(true)) {
    describe_objects(true);
  }
}

void perform_first_look() {
  if (describe_room(false)) {
    if (!game_state.super_brief) {
      describe_objects(false);
    }
  }
}

void perform_verbose() {
  game_state.verbose = true;
  game_state.super_brief = false;
  tellf("Maximum verbosity.\n\n");
  perform_look();
}

void perform_brief() {
  game_state.verbose = false;
  game_state.super_brief = false;
  tellf("Brief descriptions.\n");
}

void perform_super_brief() {
  game_state.super_brief = true;
  tellf("Super-brief descriptions.\n");
}

void perform_look_cretin() {
  tellf("This isn't a primitive two-word-parser adventure game. If you want\n"
        "to look AT that object, please say so.\n");
}

void perform_inventory() {
  game_state.c_elapsed = 18;
  if (objects[player].child != NOTHING) {
    print_cont(player, false, 0);
  } else {
    tellf("You are empty-handed.\n");
  }
}

const char *indents[6] = {"", "  ", "    ", "      ", "        ", "          "};

// Meta Actions
void perform_save() {
  if (obj_in(O_FLOYD, current_room) && obj_has_flag(O_FLOYD, F_RLANDBIT)) {
    tellf("Floyd's eyes light up. \"Oh boy! Are we gonna try something\n"
          "dangerous now?\"\n\n");
  }
  if (save_game("planetfall.sav")) {
    tellf("Ok.\n");
  } else {
    tellf("Failed.\n");
  }
}

void perform_restore() {
  if (obj_in(O_FLOYD, current_room) && obj_has_flag(O_FLOYD, F_RLANDBIT)) {
    tellf("Floyd looks disappointed, but understanding. \"That part of the game was more\n"
          "fun than this part,\" he admits.\n\n");
  }
  if (restore_game("planetfall.sav")) {
    tellf("Ok.\n");
    perform_first_look();
  } else {
    tellf("Failed.\n");
  }
}

void perform_restart() {
  perform_score(true);
  if (obj_in(O_FLOYD, current_room) && obj_has_flag(O_FLOYD, F_RLANDBIT)) {
    tellf("Floyd looks sad. \"Going away?\" he asks.\n");
  }
  tellf("\nDo you wish to restart? (Y is affirmative): ");
  if (ask_yes()) {
    tellf("Restarting.\n");
    init_game_data();
    perform_first_look();
  }
}

void perform_script() { set_scripting(true); }

void perform_unscript() { set_scripting(false); }

int perform_score(bool ask) {
  tellf("Your score %s%d (out of 80 points). It is Day %d of your adventure. Current Galactic Standard Time ",
        ask ? "would be " : "is ", game_state.score, game_state.day);
  if (obj_in(O_CHRONOMETER, player)) {
    tellf("(adjusted to your local day-cycle) is ");
    if (obj_has_flag(O_CHRONOMETER, F_MUNGEDBIT)) {
      tellf("%d", game_state.munged_time);
    } else {
      int hour = 8 + (game_state.internal_moves / 60);
      int minute = (game_state.internal_moves % 60);
      tellf("%d:%02d", hour, minute);
    }
  } else {
    tellf("is impossible to determine, since you're not wearing your chronometer");
  }
  tellf(".\n");

  tellf("This score gives you the rank of ");
  if (game_state.score == 80)
    tellf("Galactic Overlord");
  else if (game_state.score > 72)
    tellf("Cluster Admiral");
  else if (game_state.score > 64)
    tellf("System Captain");
  else if (game_state.score > 48)
    tellf("Planetary Commodore");
  else if (game_state.score > 36)
    tellf("Lieutenant");
  else if (game_state.score > 24)
    tellf("Ensign First Class");
  else if (game_state.score > 12)
    tellf("Space Cadet");
  else
    tellf("Beginner");
  tellf(".\n");

  return game_state.score;
}

bool ask_yes() {
  printf("> ");
  fflush(stdout);
  char buf[64];
  if (!fgets(buf, sizeof(buf), stdin)) {
    return false;
  }
  char *p = buf;
  while (*p == ' ' || *p == '\t')
    p++;
  if (strncasecmp(p, "yes", 3) == 0 || strncasecmp(p, "y", 1) == 0) {
    return true;
  }
  return false;
}

void perform_quit() {
  perform_score(true);
  if (obj_in(O_FLOYD, current_room) && obj_has_flag(O_FLOYD, F_RLANDBIT)) {
    tellf("\nFloyd grins impishly. \"Giving up, huh?\"\n");
  }
  tellf("\nDo you wish to leave the game? (Y is affirmative): ");
  if (ask_yes()) {
    game_running = false;
  } else {
    tellf("Ok.\n");
  }
}

void finish(bool died, bool repeating) {
  tellf("\n");
  if (!repeating) {
    perform_score(true);
    if (died) {
      tellf("\nOh, well. According to the Treaty of Gishen IV, signed in 8747 GY, all\n"
            "adventure game players must be given another chance after dying. In the\n"
            "interests of interstellar peace...\n");
    }
  }

  while (1) {
    tellf("\nWould you like to restart the game from the beginning, restore a saved game\n"
          "position, or end this session of the game? (Type RESTART, RESTORE, or QUIT.)\n\n> ");
    fflush(stdout);
    char buf[64];
    if (!fgets(buf, sizeof(buf), stdin)) {
      game_running = false;
      return;
    }
    char *p = buf;
    while (*p == ' ' || *p == '\t')
      p++;
    if (strncasecmp(p, "restart", 7) == 0) {
      perform_restart();
      return;
    } else if (strncasecmp(p, "restore", 7) == 0) {
      if (restore_game("planetfall.sav")) {
        tellf("Ok.\n");
        return;
      } else {
        tellf("Failed.\n");
        repeating = true;
      }
    } else if (strncasecmp(p, "quit", 4) == 0 || strncasecmp(p, "q", 1) == 0) {
      game_running = false;
      return;
    } else {
      repeating = true;
    }
  }
}

void jigs_up(const char *msg) {
  tellf("%s\n\n    ****  You have died  ****\n", msg);
  finish(true, false);
}

void perform_version() {
  tellf("PLANETFALL\n"
        "Infocom interactive fiction - a science fiction story\n"
        "Copyright (c) 1983 by Infocom, Inc. All rights reserved.\n"
        "PLANETFALL is a registered trademark of Infocom, Inc.\n"
        "Release 37 / Serial number 851003\n");
  if (obj_in(O_FLOYD, current_room) && obj_has_flag(O_FLOYD, F_RLANDBIT)) {
    tellf("\n\"Last version was better,\" says Floyd. \"More bugs. Bugs make\ngame fun.\"\n");
  }
}

void use_directions() {
  tellf("Use compass directions for movement.\n");
}

void perform_walk_around() {
  use_directions();
}

void perform_walk_to(ZObjectID obj) {
  if (obj != NOTHING && (obj_in(obj, current_room) || global_in(obj, current_room))) {
    tellf("It's here!\n");
  } else {
    use_directions();
  }
}

void perform_walk(ZObjectID dest) {
  if (current_room == R_WEST_WING && dest == R_CERTAIN_DEATH_MSG) {
    tellf("Certain death.\n");
    return;
  }
  if (current_room == R_BALCONY && dest == R_CRAG) {
    dest = water_level_f();
  } else if (current_room == R_WINDING_STAIR && dest == R_BALCONY) {
    dest = water_level_f();
  } else if (current_room == R_COURTYARD && dest == R_WINDING_STAIR) {
    if (game_state.day >= 6) {
      dest = water_level_f();
    }
  }
  if ((current_room == R_DORM_CORRIDOR && dest == R_CORRIDOR_JUNCTION) ||
      (current_room == R_CORRIDOR_JUNCTION && dest == R_DORM_CORRIDOR)) {
    dest = long_hall_f();
  }
  if ((current_room == R_ADMIN_CORRIDOR && dest == R_ADMIN_CORRIDOR_N) ||
      (current_room == R_ADMIN_CORRIDOR_N && dest == R_ADMIN_CORRIDOR)) {
    dest = ladder_exit_f();
    if (dest == NOTHING) return;
  }
  if ((current_room == R_REC_AREA && dest == R_CONFERENCE_ROOM) ||
      (current_room == R_CONFERENCE_ROOM && dest == R_REC_AREA)) {
    if (!obj_has_flag(O_CONFERENCE_DOOR, F_OPENBIT)) {
      tellf("The door is closed.\n");
      return;
    }
  }
  if ((current_room == R_MESS_CORRIDOR && dest == R_STORAGE_WEST) ||
      (current_room == R_STORAGE_WEST && dest == R_MESS_CORRIDOR)) {
    if (!obj_has_flag(O_STORAGE_WEST_DOOR, F_OPENBIT)) {
      tellf("The door is closed.\n");
      return;
    }
  }
  if ((current_room == R_MESS_HALL && dest == R_KITCHEN) ||
      (current_room == R_KITCHEN && dest == R_MESS_HALL)) {
    if (!obj_has_flag(O_KITCHEN_DOOR, F_OPENBIT)) {
      tellf("The door is closed.\n");
      return;
    }
  }

  if (dest == NOTHING || dest <= 0) {
    if (!is_lit(current_room) && (rand() % 100) < 75) {
      jigs_up("Oh, no! You have walked into the slavering fangs of a lurking grue!");
      return;
    }
    tellf("You can't go that way.\n");
    return;
  }
  if (objects[dest].action) {
    objects[dest].action(M_ENTER);
  }
  obj_move(player, dest);
  current_room = dest;
  perform_first_look();
}

bool dispatch_action(int verb, ZObjectID prso, ZObjectID prsi) {
  // Try object specific action first (PRSO)
  if (prso != NOTHING && objects[prso].action) {
    if (objects[prso].action(verb))
      return true;
  }

  // Try indirect object action (PRSI)
  if (prsi != NOTHING && objects[prsi].action) {
    if (objects[prsi].action(verb))
      return true;
  }

  switch (verb) {
  case V_VERBOSE:
    perform_verbose();
    return true;
  case V_BRIEF:
    perform_brief();
    return true;
  case V_SUPER_BRIEF:
    perform_super_brief();
    return true;
  case V_LOOK:
    perform_look();
    return true;
  case V_LOOK_CRETIN:
    perform_look_cretin();
    return true;
  case V_QUIT:
    perform_quit();
    return true;
  case V_RESTART:
    perform_restart();
    return true;
  case V_RESTORE:
    perform_restore();
    return true;
  case V_SAVE:
    perform_save();
    return true;
  case V_SCORE:
    perform_score(false);
    return true;
  case V_SCRIPT:
    perform_script();
    return true;
  case V_UNSCRIPT:
    perform_unscript();
    return true;
  case V_VERSION:
    perform_version();
    return true;
  case V_WALK_AROUND:
    perform_walk_around();
    return true;
  case V_WALK_TO:
    perform_walk_to(prso);
    return true;
  case V_TIME:
    // TODO: partial implementation
    tellf("Day %d, %02d:%02d\n", game_state.day, game_state.internal_moves / 60,
          game_state.internal_moves % 60);
    return true;
  case V_TELL:
    // Needs proper implementation
    return false;
  case V_TELEPORT:
    perform_teleport(prso);
    return true;
  case V_NORTH:
    perform_walk(objects[current_room].north);
    return true;
  case V_SOUTH:
    perform_walk(objects[current_room].south);
    return true;
  case V_EAST:
    perform_walk(objects[current_room].east);
    return true;
  case V_WEST:
    perform_walk(objects[current_room].west);
    return true;
  case V_TAKE:
    perform_take(prso);
    return true;
  case V_DROP:
    perform_drop(prso);
    return true;
  case V_THROW:
    perform_throw(prso, prsi);
    return true;
  case V_PUT:
    perform_put(prso, prsi);
    return true;
  case V_SLIDE:
    perform_slide();
    return true;
  case V_GIVE:
    perform_give(prso, prsi);
    return true;
  case V_SGIVE:
    perform_sgive(prso, prsi);
    return true;
  case V_EXAMINE:
    perform_examine(prso);
    return true;
  case V_READ:
    perform_read(prso);
    return true;
  default:
    break;
  }
  return false;
}
void perform_teleport(ZObjectID dest) {
  if (dest == NOTHING) {
    tellf("Teleport where?\n");
    return;
  }
  // Simple teleport: Move player to destination
  // If destination is an object, move to its location? Default behavior usually
  // allows 'teleport [room]'

  // Check if dest is a room. In ZIL, Rooms are valid objects.
  // Assuming room ID range or flag. For now, just move player.
  if (dest < MAX_OBJECTS) {
    obj_move(player, dest);
    current_room = dest; // IMPORTANT: Engine presumably trusts obj_move but
                         // current_room global needs sync?
    // Wait, standard ZIL engine usually derives current_room from player
    // parent, but 'current_room' global exists in planetfall.h Let's assume we
    // need to update it.
    perform_look();
    return;
  }
  tellf("That's not a valid destination.\n");
}
