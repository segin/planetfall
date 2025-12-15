#include "global_objects.h"
#include "ids.h"
#include "planetfall.h"
#include <stdio.h>

void init_global_objects() {
  ZObject *o;

  // CLIFF
  o = &objects[O_CLIFF];
  o->id = O_CLIFF;
  o->description = "cliff";
  o->synonyms[0] = "cliff";
  o->synonyms[1] = "wall";
  o->flags = F_NDESCBIT;
  obj_move(O_CLIFF, OBJ_GLOBAL_OBJECTS);

  // OCEAN
  o = &objects[O_OCEAN];
  o->id = O_OCEAN;
  o->description = "ocean";
  o->synonyms[0] = "ocean";
  o->synonyms[1] = "sea";
  o->synonyms[2] = "water";
  o->flags = F_NDESCBIT | F_VOWELBIT;
  obj_move(O_OCEAN, OBJ_GLOBAL_OBJECTS);

  // STAIRS
  o = &objects[O_STAIRS];
  o->id = O_STAIRS;
  o->description = "stairway";
  o->synonyms[0] = "stairs";
  o->synonyms[1] = "stairway";
  o->synonyms[2] = "steps";
  o->synonyms[3] = "gangway";
  o->flags = F_NDESCBIT | F_CLIMBBIT;
  obj_move(O_STAIRS, OBJ_LOCAL_GLOBALS);

  // WINDOW
  o = &objects[O_WINDOW];
  o->id = O_WINDOW;
  o->description = "window";
  o->synonyms[0] = "window";
  o->synonyms[1] = "port";
  o->synonyms[2] = "viewport";
  o->adjectives[0] = "view";
  o->flags = F_NDESCBIT;
  obj_move(O_WINDOW, OBJ_LOCAL_GLOBALS);

  // GROUND
  o = &objects[O_GROUND];
  o->id = O_GROUND;
  o->description = "floor";
  o->synonyms[0] = "ground";
  o->synonyms[1] = "earth";
  o->synonyms[2] = "floor";
  o->synonyms[3] = "deck";
  o->flags = F_NDESCBIT;
  obj_move(O_GROUND, OBJ_GLOBAL_OBJECTS);

  // HANDS
  o = &objects[O_HANDS];
  o->id = O_HANDS;
  o->description = "pair of hands";
  o->synonyms[0] = "hands";
  o->synonyms[1] = "pair";
  o->adjectives[0] = "bare";
  o->flags = F_NDESCBIT;
  obj_move(O_HANDS, OBJ_GLOBAL_OBJECTS);

  // CONTROLS
  o = &objects[O_CONTROLS];
  o->id = O_CONTROLS;
  o->description = "set of controls";
  o->synonyms[0] = "controls";
  o->synonyms[1] = "panel";
  o->synonyms[2] = "dials";
  o->synonyms[3] = "gauges";
  o->adjectives[0] = "control";
  o->adjectives[1] = "complex";
  o->flags = F_NDESCBIT;
  obj_move(O_CONTROLS, OBJ_LOCAL_GLOBALS);

  // LIGHTS
  o = &objects[O_LIGHTS];
  o->id = O_LIGHTS;
  o->description = "light";
  o->synonyms[0] = "light";
  o->synonyms[1] = "lights";
  o->adjectives[0] = "red";
  o->adjectives[1] = "dazzling";
  o->adjectives[2] = "glowing";
  o->flags = F_NDESCBIT;
  obj_move(O_LIGHTS, OBJ_LOCAL_GLOBALS);

  // TABLES
  o = &objects[O_TABLES];
  o->id = O_TABLES;
  o->description = "table";
  o->synonyms[0] = "table";
  o->synonyms[1] = "tables";
  o->adjectives[0] = "round";
  o->adjectives[1] = "conference";
  o->adjectives[2] = "small";
  o->flags = F_NDESCBIT;
  obj_move(O_TABLES, OBJ_LOCAL_GLOBALS);

  // SHELVES
  o = &objects[O_SHELVES];
  o->id = O_SHELVES;
  o->description = "shelf";
  o->synonyms[0] = "shelf";
  o->synonyms[1] = "shelves";
  o->flags = F_NDESCBIT;
  obj_move(O_SHELVES, OBJ_LOCAL_GLOBALS);

  // BUTTONS (Teleportation)
  // BUTTON 1
  o = &objects[O_BUTTON_1];
  o->id = O_BUTTON_1;
  o->description = "brown button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "brown";
  o->text = "1";
  o->flags = F_NDESCBIT;
  obj_move(O_BUTTON_1, OBJ_LOCAL_GLOBALS);

  // BUTTON 2
  o = &objects[O_BUTTON_2];
  o->id = O_BUTTON_2;
  o->description = "beige button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "beige";
  o->text = "2";
  o->flags = F_NDESCBIT;
  obj_move(O_BUTTON_2, OBJ_LOCAL_GLOBALS);

  // BUTTON 3
  o = &objects[O_BUTTON_3];
  o->id = O_BUTTON_3;
  o->description = "tan button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "tan";
  o->text = "3";
  o->flags = F_NDESCBIT;
  obj_move(O_BUTTON_3, OBJ_LOCAL_GLOBALS);

  // SLOT
  o = &objects[O_SLOT];
  o->id = O_SLOT;
  o->description = "slot";
  o->synonyms[0] = "slot";
  o->adjectives[0] = "small";
  o->flags = F_NDESCBIT;
  obj_move(O_SLOT, OBJ_LOCAL_GLOBALS);

  // INTNUM
  o = &objects[O_INTNUM];
  o->id = O_INTNUM;
  o->description = "number";
  o->synonyms[0] = "intnum";
  obj_move(O_INTNUM, OBJ_GLOBAL_OBJECTS);

  // NOT_HERE_OBJECT
  o = &objects[O_NOT_HERE_OBJECT];
  o->id = O_NOT_HERE_OBJECT;
  o->description = "such thing";
  o->action_id = O_NOT_HERE_OBJECT; // Needs function

  // PSEUDO_OBJECT
  o = &objects[O_PSEUDO_OBJECT];
  o->id = O_PSEUDO_OBJECT;
  o->description = "pseudo";
  o->action_id = O_PSEUDO_OBJECT; // Needs function GO

  // DIRECTIONS
  o = &objects[O_NORTH];
  o->id = O_NORTH;
  o->description = "north";
  o->synonyms[0] = "north";
  o->synonyms[1] = "n";
  o->synonyms[2] = "fore";
  o = &objects[O_SOUTH];
  o->id = O_SOUTH;
  o->description = "south";
  o->synonyms[0] = "south";
  o->synonyms[1] = "s";
  o->synonyms[2] = "aft";
  o = &objects[O_EAST];
  o->id = O_EAST;
  o->description = "east";
  o->synonyms[0] = "east";
  o->synonyms[1] = "e";
  o->synonyms[2] = "starboard";
  o->synonyms[3] = "sb";
  o = &objects[O_WEST];
  o->id = O_WEST;
  o->description = "west";
  o->synonyms[0] = "west";
  o->synonyms[1] = "w";
  o->synonyms[2] = "port";
  o->synonyms[3] = "p";
  o = &objects[O_NE];
  o->id = O_NE;
  o->description = "northeast";
  o->synonyms[0] = "northeast";
  o->synonyms[1] = "ne";
  o = &objects[O_NW];
  o->id = O_NW;
  o->description = "northwest";
  o->synonyms[0] = "northwest";
  o->synonyms[1] = "nw";
  o = &objects[O_SE];
  o->id = O_SE;
  o->description = "southeast";
  o->synonyms[0] = "southeast";
  o->synonyms[1] = "se";
  o = &objects[O_SW];
  o->id = O_SW;
  o->description = "southwest";
  o->synonyms[0] = "southwest";
  o->synonyms[1] = "sw";
  o = &objects[O_UP];
  o->id = O_UP;
  o->description = "up";
  o->synonyms[0] = "up";
  o->synonyms[1] = "u";
  o->synonyms[2] = "ceiling";
  o = &objects[O_DOWN];
  o->id = O_DOWN;
  o->description = "down";
  o->synonyms[0] = "down";
  o->synonyms[1] = "d";
  o->synonyms[2] = "floor";
  o = &objects[O_IN];
  o->id = O_IN;
  o->description = "in";
  o->synonyms[0] = "in";
  o->synonyms[1] = "inside";
  o->synonyms[2] = "into";
  o = &objects[O_OUT];
  o->id = O_OUT;
  o->description = "out";
  o->synonyms[0] = "out";
  o->synonyms[1] = "outside";

  // ADVENTURER (OBJ_PLAYER)
  o = &objects[OBJ_PLAYER];
  o->id = OBJ_PLAYER;
  o->description = "cretin"; // Internal name often, or "adventurer"
  o->synonyms[0] = "me";
  o->synonyms[1] = "myself";
  o->synonyms[2] = "self";
  o->synonyms[3] = "cretin";
  o->adjectives[0] = "my";
  o->flags = F_NDESCBIT | F_ACTORBIT; // Removed F_INVISIBLE to allow EXAMINE ME
  obj_move(OBJ_PLAYER, R_DECK_NINE);

  // IT
  o = &objects[O_IT];
  o->id = O_IT;
  o->description = "random object";
  o->synonyms[0] = "it";
  o->synonyms[1] = "that";
  o->synonyms[2] = "this";
  o->synonyms[3] = "him";
  o->flags = F_NDESCBIT;
  obj_move(O_IT, OBJ_GLOBAL_OBJECTS);
}
