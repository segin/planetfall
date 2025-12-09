#ifndef PLANETFALL_H
#define PLANETFALL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "ids.h"

// Max number of objects in the game
#define MAX_OBJECTS 1000

// Object IDs
typedef int ZObjectID;

// Boolean Flags (Attributes)
typedef enum {
    F_INVISIBLE    = 1ULL << 0,
    F_TOUCHBIT     = 1ULL << 1,
    F_SURFACEBIT   = 1ULL << 2,
    F_TRYTAKEBIT   = 1ULL << 3,
    F_MUNGBIT      = 1ULL << 4,
    F_MUNGEDBIT    = 1ULL << 5,
    F_SCRAMBLEDBIT = 1ULL << 6,
    F_WORNBIT      = 1ULL << 7,
    F_OPENBIT      = 1ULL << 8,
    F_SEARCHBIT    = 1ULL << 9,
    F_TRANSBIT     = 1ULL << 10,
    F_WEARBIT      = 1ULL << 11,
    F_NDESCBIT     = 1ULL << 12,
    F_CLIMBBIT     = 1ULL << 13,
    F_VOWELBIT     = 1ULL << 14,
    F_ACTORBIT     = 1ULL << 15,
    F_VEHBIT       = 1ULL << 16,
    F_ACIDBIT      = 1ULL << 17,
    F_FOODBIT      = 1ULL << 18,
    F_READBIT      = 1ULL << 19,
    F_TAKEBIT      = 1ULL << 20,
    F_CONTBIT      = 1ULL << 21,
    F_LIGHTBIT     = 1ULL << 22,
    F_ONBIT        = 1ULL << 23,
    F_DOORBIT      = 1ULL << 24,
    F_TOOLBIT      = 1ULL << 25,
    F_RLANDBIT     = 1ULL << 26
} ZFlag;

typedef struct {
    ZObjectID id;
    ZObjectID parent;
    ZObjectID child;
    ZObjectID sibling;
    
    // Vocabulary for parser matching
    const char* synonyms[5];
    const char* adjectives[5];
    
    // Descriptions
    const char* description;      // Short description (e.g., "brass lantern")
    const char* long_description; // Room description or Initial presence description
    const char* text;             // For reading (books, signs)
    
    uint64_t flags;
    
    int size;
    int capacity;
    int value; // Score value
    
    // Action routine ID (to be mapped to function dispatch)
    int action_id;
    
    // Function pointer for object action (Returns true if handled)
    bool (*action)(void);

    // Room specific properties (Exits)
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
    
} ZObject;

// Game State
typedef struct {
    int internal_moves;
    int day;
    int score;
    int load_allowed;
    int hunger_level;
    int sickness_level;
    int sleepy_level;
    int blowup_counter;
    int trip_counter;
    int sink_counter;
    int brigs_up;
    int blather_leave_counter;
    int ambassador_leave_counter;
    bool verbose;
} ZGameState;

// Global Arrays
extern ZObject objects[MAX_OBJECTS];
extern ZGameState game_state;
extern ZObjectID player;
extern ZObjectID current_room;

// Core Engine API
void init_game();
void init_game_data();
void init_objects();

extern bool game_running;

// Object Manipulation
ZObject* get_obj(ZObjectID id);
void obj_move(ZObjectID obj, ZObjectID dest);
void obj_remove(ZObjectID obj);
bool obj_in(ZObjectID obj, ZObjectID parent);
bool obj_has_flag(ZObjectID obj, ZFlag flag);
void obj_set_flag(ZObjectID obj, ZFlag flag);
void obj_clear_flag(ZObjectID obj, ZFlag flag);

// Hierarchy Traversal
ZObjectID obj_first_child(ZObjectID parent);
ZObjectID obj_next_sibling(ZObjectID obj);
ZObjectID obj_parent(ZObjectID obj);
void obj_rob(ZObjectID victim, ZObjectID dest);

// Output
#include "output.h"

// Parser/Output
void perform_look();

// Events (Included from events.h generally, but we need forward decls if not including)
// Actually, circular include if events.h includes planetfall.h?
// events.h doesn't seem to include planetfall.h (it just includes stdbool).
// So I can include it.
#include "events.h"

// Globals
extern int brigs_up;

#endif
