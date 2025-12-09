#ifndef PARSER_H
#define PARSER_H

#include "planetfall.h"

typedef struct {
    int verb;
    ZObjectID direct_object;
    ZObjectID indirect_object;
    int preposition;
} Command;

// Verb IDs (Shared with main)
enum {
    V_LOOK = 1,
    V_INVENTORY,
    V_EXAMINE,
    V_TAKE,
    V_DROP,
    V_QUIT,
    V_PUT,
    V_WAIT,
    V_BOARD,
    V_DISEMBARK,
    V_OPEN,
    V_CLOSE,
    V_SCRUB,
    V_ATTACK,
    V_KICK,
    V_TALK,
    V_EAT,
    V_SMELL,
    V_READ,
    V_REMOVE,
    V_WEAR,
    V_WALK_NORTH,
    V_WALK_SOUTH,
    V_WALK_EAST,
    V_WALK_WEST,
    V_WALK_NE,
    V_WALK_NW,
    V_WALK_SE,
    V_WALK_SW,
    V_WALK_UP,
    V_WALK_DOWN,
    V_WALK_IN,
    V_WALK_OUT
};

bool parse_command(char* input, Command* cmd);

#endif
