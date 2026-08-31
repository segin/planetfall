#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "planetfall.h"

// Global Storage
ZObject objects[MAX_OBJECTS];
ZGameState game_state;
ZObjectID player = OBJ_PLAYER;
ZObjectID current_room = NOTHING;
bool game_running = true;

void init_game() {
    memset(objects, 0, sizeof(objects));
    memset(&game_state, 0, sizeof(game_state));
    
    // Default Game State
    game_state.day = 1;
    // <SETG INTERNAL-MOVES <+ 4450 <RANDOM 180>>> -- ZIL RANDOM is 1..N, so the
    // game opens somewhere in 4451..4630 Galactic Standard Time.
    game_state.internal_moves = 4450 + (rand() % 180) + 1;
    game_state.moves = game_state.internal_moves;
    game_state.c_elapsed = C_ELAPSED_DEFAULT;
    game_state.load_allowed = 100;
    game_state.number_needed = (rand() % 1000) + 1;
    game_state.upper_elevator_up = true;
    game_state.lower_elevator_up = false;
    game_state.just_entered = true;
    game_state.chemical_required = (rand() % 7) + 1;
}

ZObject* get_obj(ZObjectID id) {
    if (id <= 0 || id >= MAX_OBJECTS) return NULL;
    return &objects[id];
}

void obj_set_flag(ZObjectID id, ZFlag flag) {
    ZObject* obj = get_obj(id);
    if (obj) obj->flags |= flag;
}

void obj_clear_flag(ZObjectID id, ZFlag flag) {
    ZObject* obj = get_obj(id);
    if (obj) obj->flags &= ~flag;
}

bool obj_has_flag(ZObjectID id, ZFlag flag) {
    ZObject* obj = get_obj(id);
    if (!obj) return false;
    return (obj->flags & flag) != 0;
}

void score_obj(ZObjectID obj) {
    if (obj <= 0 || obj >= MAX_OBJECTS) return;
    if (objects[obj].value > 0) {
        obj_set_flag(obj, F_TOUCHBIT);
        game_state.score += objects[obj].value;
        objects[obj].value = 0;
    }
}

// ZILCH lays the object table out in declaration order, so objects the compiler
// placed are traversed in the order they were written, while <MOVE> at runtime
// links the object in as the new first child. Both matter: the first decides
// what order your inventory and every room's contents print in, the second is
// what the game logic depends on. Reproduce both by appending while the world is
// still being built and prepending once play starts.
bool world_building = true;

// Moves obj to dest (re-linking parent/child/sibling)
void obj_move(ZObjectID id, ZObjectID dest_id) {
    if (id == NOTHING) return;

    // 1. Remove from current location
    obj_remove(id);

    if (dest_id == NOTHING) return;

    ZObject* obj = get_obj(id);
    ZObject* dest = get_obj(dest_id);

    if (!obj || !dest) return;

    obj->parent = dest_id;

    if (world_building) {
        // 2a. Append, so initial contents list in declaration order.
        obj->sibling = NOTHING;
        if (dest->child == NOTHING) {
            dest->child = id;
        } else {
            ZObjectID last = dest->child;
            while (objects[last].sibling != NOTHING)
                last = objects[last].sibling;
            objects[last].sibling = id;
        }
    } else {
        // 2b. Prepend, as the Z-machine's insert_obj does.
        obj->sibling = dest->child;
        dest->child = id;
    }
}

// Removes obj from its parent (orphans it)
void obj_remove(ZObjectID id) {
    ZObject* obj = get_obj(id);
    if (!obj || obj->parent == NOTHING) return;
    
    ZObject* parent = get_obj(obj->parent);
    
    // Check if it's the first child
    if (parent->child == id) {
        parent->child = obj->sibling;
    } else {
        // Find previous sibling
        ZObjectID curr_id = parent->child;
        while (curr_id != NOTHING) {
            ZObject* curr = get_obj(curr_id);
            if (curr->sibling == id) {
                curr->sibling = obj->sibling;
                break;
            }
            curr_id = curr->sibling;
        }
    }
    
    obj->parent = NOTHING;
    obj->sibling = NOTHING;
}

bool obj_in(ZObjectID id, ZObjectID parent_id) {
    ZObject* obj = get_obj(id);
    return obj && obj->parent == parent_id;
}

ZObjectID obj_first_child(ZObjectID parent) {
    ZObject* obj = get_obj(parent);
    return obj ? obj->child : NOTHING;
}

ZObjectID obj_next_sibling(ZObjectID obj) {
    ZObject* o = get_obj(obj);
    return o ? o->sibling : NOTHING;
}

ZObjectID obj_parent(ZObjectID obj) {
    ZObject* o = get_obj(obj);
    return o ? o->parent : NOTHING;
}

// Moves all children of victim to dest
void obj_rob(ZObjectID victim, ZObjectID dest) {
    ZObject* v = get_obj(victim);
    if (!v) return;
    
    ZObjectID curr = v->child;
    while (curr != NOTHING) {
        ZObjectID next = objects[curr].sibling;
        obj_move(curr, dest);
        curr = next;
    }
}

