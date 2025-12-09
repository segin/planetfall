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
    game_state.load_allowed = 100;
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

// Moves obj to dest (re-linking parent/child/sibling)
void obj_move(ZObjectID id, ZObjectID dest_id) {
    if (id == NOTHING) return;

    // 1. Remove from current location
    obj_remove(id);
    
    if (dest_id == NOTHING) return;
    
    ZObject* obj = get_obj(id);
    ZObject* dest = get_obj(dest_id);
    
    if (!obj || !dest) return;
    
    // 2. Add to new location (as first child)
    obj->parent = dest_id;
    obj->sibling = dest->child;
    dest->child = id;
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

