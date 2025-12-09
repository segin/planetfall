#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "actions.h"
#include "parser.h"
#include "events.h"
#include "output.h"
#include "savegame.h"

// Helper: Weight
int get_weight(ZObjectID obj) {
    if (obj == NOTHING) return 0;
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

void score_obj(ZObjectID obj) {
    if (objects[obj].value > 0) {
        obj_set_flag(obj, F_TOUCHBIT);
        game_state.score += objects[obj].value;
        objects[obj].value = 0;
    }
}

bool itake(ZObjectID obj, bool verbose) {
    if (!obj_has_flag(obj, F_TAKEBIT)) {
        if (verbose) tellf("You can't take that.\n"); 
        return false;
    }
    
    int player_load = get_weight(player);
    int obj_weight = get_weight(obj);
    
    if (!obj_in(obj_parent(obj), player)) {
        if (player_load + obj_weight > game_state.load_allowed) {
            if (verbose) tellf("Your load is too heavy.\n");
            return false;
        }
    }
    
    int cnt = count_contents(player);
    if (cnt > 7 && (rand() % 100) < (cnt * 8)) {
        tellf("Oh, no. You fumble and drop it.\n");
        return false;
    }
    
    obj_move(obj, player);
    obj_clear_flag(obj, F_NDESCBIT);
    score_obj(obj);
    obj_set_flag(obj, F_TOUCHBIT);
    return true;
}

void perform_take(ZObjectID obj) {
    if (obj == player) {
        tellf("How romantic.\n");
        return;
    }
    if (obj_in(obj, player)) {
        tellf("You already have it.\n");
        return;
    }
    
    if (itake(obj, true)) {
        tellf("Taken.\n");
    }
}

bool idrop(ZObjectID obj) {
    if (!obj_in(obj, player)) {
        tellf("You're not carrying the %s.\n", objects[obj].description);
        return false;
    }
    if (obj_has_flag(obj, F_WORNBIT)) {
        tellf("You'll have to take it off, first.\n");
        return false;
    }
    
    obj_move(obj, current_room);
    return true;
}

void perform_drop(ZObjectID obj) {
    if (idrop(obj)) {
        tellf("Dropped.\n");
    }
}

void perform_examine(ZObjectID obj) {
    if (obj == NOTHING) return;
    
    if (objects[obj].text && obj_has_flag(obj, F_READBIT)) {
         tellf("%s\n", objects[obj].text);
    } else if (obj_has_flag(obj, F_CONTBIT) || obj_has_flag(obj, F_DOORBIT)) {
         if (obj_has_flag(obj, F_OPENBIT)) {
             tellf("The %s is open.\n", objects[obj].description);
             ZObjectID child = objects[obj].child;
             if (child) {
                 tellf("It contains:\n");
                 while (child != NOTHING) {
                     tellf("  %s\n", objects[child].description);
                     child = objects[child].sibling;
                 }
             } else {
                 tellf("It is empty.\n");
             }
         } else {
             tellf("The %s is closed.\n", objects[obj].description);
         }
    } else {
         if (objects[obj].long_description && !obj_has_flag(obj, F_TOUCHBIT)) {
             tellf("%s\n", objects[obj].long_description);
         } else {
             tellf("I see nothing special about the %s.\n", objects[obj].description);
         }
    }
}

void perform_look() {
    ZObject* r = &objects[current_room];
    tellf("%s\n", r->description);
    tellf("%s\n", r->long_description);
    
    ZObjectID child = r->child;
    while (child != NOTHING) {
        if (child != player && !obj_has_flag(child, F_NDESCBIT) && !obj_has_flag(child, F_INVISIBLE)) {
            tellf("There is a %s here.\n", objects[child].description);
        }
        child = objects[child].sibling;
    }
}

void perform_inventory() {
    tellf("You are carrying:\n");
    ZObjectID child = objects[player].child;
    if (child == NOTHING) {
        tellf("  Nothing.\n");
        return;
    }
    while (child != NOTHING) {
        tellf("  %s", objects[child].description);
        if (obj_has_flag(child, F_WORNBIT)) tellf(" (being worn)");
        if (obj_has_flag(child, F_OPENBIT) && obj_has_flag(child, F_CONTBIT)) {
             ZObjectID inner = objects[child].child;
             if (inner) {
                 tellf("\n    containing:");
                 while (inner != NOTHING) {
                     tellf("\n      %s", objects[inner].description);
                     inner = objects[inner].sibling;
                 }
             }
        }
        tellf("\n");
        child = objects[child].sibling;
    }
}

// Meta Actions
void perform_save() {
    save_game("planetfall.sav");
}

void perform_restore() {
    if (restore_game("planetfall.sav")) {
        perform_look();
    }
}

void perform_restart() {
    tellf("Restarting.\n");
    init_game_data();
    perform_look();
}

void perform_script() {
    set_scripting(true);
}

void perform_unscript() {
    set_scripting(false);
}

void jigs_up(const char* msg) {
    tellf("%s\n", msg);
    tellf("**** You have died ****\n");
    
    // Prompt loop
    while (1) {
        tellf("\nWould you like to RESTART, RESTORE, or QUIT?\n> ");
        char buf[64];
        if (!fgets(buf, 64, stdin)) exit(0);
        
        // Simple parsing
        if (strncasecmp(buf, "restart", 7) == 0) {
            perform_restart();
            return; // Continue game
        }
        if (strncasecmp(buf, "restore", 7) == 0) {
            perform_restore();
            return;
        }
        if (strncasecmp(buf, "quit", 4) == 0) {
            game_running = false;
            return;
        }
    }
}

bool dispatch_action(int verb, ZObjectID prso, ZObjectID prsi) {
    if (prso != NOTHING && objects[prso].action) {
        // Handle object action?
    }

    switch (verb) {
        case V_LOOK: perform_look(); return true;
        case V_INVENTORY: perform_inventory(); return true;
        case V_TAKE: perform_take(prso); return true;
        case V_DROP: perform_drop(prso); return true;
        case V_EXAMINE: perform_examine(prso); return true;
        case V_SAVE: perform_save(); return true;
        case V_RESTORE: perform_restore(); return true;
        case V_RESTART: perform_restart(); return true;
        case V_SCRIPT: perform_script(); return true;
        case V_UNSCRIPT: perform_unscript(); return true;
        case V_QUIT: game_running = false; return true;
        default: return false;
    }
}
