#include <stdio.h>
#include <string.h>
#include "planetfall.h"
#include "events.h"
#include "output.h"

typedef struct {
    ZObjectID id;
    ZObjectID parent;
    ZObjectID child;
    ZObjectID sibling;
    uint64_t flags;
    int size;
    int capacity;
    int value;
    ZObjectID north;
    ZObjectID south;
    ZObjectID east;
    ZObjectID west;
    ZObjectID ne;
    ZObjectID nw;
    ZObjectID se;
    ZObjectID sw;
    ZObjectID up;
    ZObjectID down;
    ZObjectID in;
    ZObjectID out;
    ZObjectID globals[10];
} SavedObject;

bool save_game(const char* filename) {
    FILE* f = fopen(filename, "wb");
    if (!f) {
        tellf("Failed to open file for saving.\n");
        return false;
    }
    
    // Header
    const char* magic = "PLANETFALL";
    fwrite(magic, 1, 10, f);
    
    // Game State
    fwrite(&game_state, sizeof(ZGameState), 1, f);
    
    // Globals
    fwrite(&player, sizeof(ZObjectID), 1, f);
    fwrite(&current_room, sizeof(ZObjectID), 1, f);

    // Objects
    for (int i = 0; i < MAX_OBJECTS; i++) {
        ZObject* o = &objects[i];
        SavedObject so;
        so.id = o->id;
        so.parent = o->parent;
        so.child = o->child;
        so.sibling = o->sibling;
        so.flags = o->flags;
        so.size = o->size;
        so.capacity = o->capacity;
        so.value = o->value;
        so.north = o->north;
        so.south = o->south;
        so.east = o->east;
        so.west = o->west;
        so.ne = o->ne;
        so.nw = o->nw;
        so.se = o->se;
        so.sw = o->sw;
        so.up = o->up;
        so.down = o->down;
        so.in = o->in;
        so.out = o->out;
        memcpy(so.globals, o->globals, sizeof(o->globals));
        
        fwrite(&so, sizeof(SavedObject), 1, f);
    }
    
    // Events
    save_events(f);
    
    fclose(f);
    tellf("Game saved.\n");
    return true;
}

bool restore_game(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        tellf("Failed to open file for restoring.\n");
        return false;
    }
    
    char magic[11];
    fread(magic, 1, 10, f);
    magic[10] = '\0';
    if (strcmp(magic, "PLANETFALL") != 0) {
        tellf("Invalid save file.\n");
        fclose(f);
        return false;
    }
    
    fread(&game_state, sizeof(ZGameState), 1, f);
    fread(&player, sizeof(ZObjectID), 1, f);
    fread(&current_room, sizeof(ZObjectID), 1, f);

    for (int i = 0; i < MAX_OBJECTS; i++) {
        SavedObject so;
        if (fread(&so, sizeof(SavedObject), 1, f) != 1) break;
        
        ZObject* o = &objects[i];
        // Do NOT overwrite pointers (description, etc)
        // Overwrite mutable state
        o->id = so.id;
        o->parent = so.parent;
        o->child = so.child;
        o->sibling = so.sibling;
        o->flags = so.flags;
        o->size = so.size;
        o->capacity = so.capacity;
        o->value = so.value;
        o->north = so.north;
        o->south = so.south;
        o->east = so.east;
        o->west = so.west;
        o->ne = so.ne;
        o->nw = so.nw;
        o->se = so.se;
        o->sw = so.sw;
        o->up = so.up;
        o->down = so.down;
        o->in = so.in;
        o->out = so.out;
        memcpy(o->globals, so.globals, sizeof(o->globals));
    }
    
    restore_events(f);
    
    fclose(f);
    tellf("Game restored.\n");
    return true;
}
