#include "complexone.h"
#include "complexone_actions.h"
#include "feinstein.h"
#include "planetfall.h"
#include <stdio.h>

void init_complexone() {
  ZObject *o;
  ZObject *r;

  // === GLOBALS ===

  // === ROOMS ===

  // R_UNDERWATER
  r = &objects[R_UNDERWATER];
  r->id = R_UNDERWATER;
  r->description = "Underwater";
  r->synonyms[0] = "underwater";
  r->long_description =
      "You are momentarily disoriented as you enter the turbulent waters.\n"
      "Currents buffet you against the sharp rocks of an underwater\n"
      "cliff. A dim light filters down from above.";
  r->flags = F_ONBIT | F_RWATERBIT;
  r->up = R_CRAG;
  // Down/West/North/South -> Underwater (Loop)
  r->down = R_UNDERWATER;
  r->west = R_UNDERWATER;
  r->north = R_UNDERWATER;
  r->south = R_UNDERWATER;
  r->action = underwater_f;

  // R_CRAG (Update)
  r = &objects[R_CRAG];
  r->id = R_CRAG;
  r->description = "Crag";
  r->synonyms[0] = "crag";
  r->flags = F_ONBIT | F_RLANDBIT;
  r->long_description = "You have reached a cleft in the cliff wall where the "
                        "island rises from the\n"
                        "water. The edge of the cleft displays recently "
                        "exposed rock where it collapsed\n"
                        "under the weight of the escape pod. About two meters "
                        "below, turbulent waters\n"
                        "swirl against sharp rocks. A small structure clings "
                        "to the face of the cliff\n"
                        "about eight meters above you. Even an out-of-shape "
                        "Ensign Seventh Class could\n"
                        "probably climb up to it.";
  r->down = R_UNDERWATER;
  r->up = R_BALCONY;
  r->action = crag_f;
  // In is Escape Pod (already set)

  // Pseudo Objects for Crag
  o = &objects[O_STRUCTURE_PSEUDO];
  o->id = O_STRUCTURE_PSEUDO;
  o->description = "structure";
  o->synonyms[0] = "structure";
  o->flags = F_NDESCBIT;
  o->action = structure_pseudo_action;
  obj_move(O_STRUCTURE_PSEUDO, R_CRAG);

  o = &objects[O_CLEFT_PSEUDO];
  o->id = O_CLEFT_PSEUDO;
  o->description = "cleft";
  o->synonyms[0] = "cleft";
  o->flags = F_NDESCBIT;
  o->action = cleft_pseudo_action;
  obj_move(O_CLEFT_PSEUDO, R_CRAG);

  // R_BALCONY (Update)
  r = &objects[R_BALCONY];
  r->id = R_BALCONY;
  r->description = "Balcony";
  r->synonyms[0] = "balcony";
  r->flags = F_ONBIT | F_RLANDBIT;
  r->long_description =
      "This is an octagonal room, half carved into and half built out from the "
      "cliff\n"
      "wall. Through the shattered windows which ring the outer wall you can "
      "see\n"
      "ocean to the horizon. A weathered metal plaque with barely readable "
      "lettering\n"
      "rests below the windows. The language seems to be a corrupt form of\n"
      "Galalingua. A steep stairway, roughly cut into the face of the cliff, "
      "leads\n"
      "upward.";
  r->down = R_CRAG;
  r->up = R_WINDING_STAIR;
  r->action = balcony_f;

  // Pseudo Objects for Balcony
  o = &objects[O_PLAQUE_PSEUDO];
  o->id = O_PLAQUE_PSEUDO;
  o->description = "metal plaque";
  o->synonyms[0] = "plaque";
  o->flags = F_NDESCBIT | F_READBIT;
  o->action = plaque_pseudo_action;
  o->text = "\nSEENIK VISTA\n\n"
            "Xis stuneeng vuu uf xee Kalamontee Valee kuvurz oovur fortee "
            "skwaar miilz\n"
            "uf xat faamus tuurist spot. Xee larj bildeeng at xee bend in xee "
            "Gulmaan Rivur\n"
            "iz xee formur pravincul kapitul bildeeng.";
  obj_move(O_PLAQUE_PSEUDO, R_BALCONY);

  // R_WINDING_STAIR (Update)
  r = &objects[R_WINDING_STAIR];
  r->id = R_WINDING_STAIR;
  r->description = "Winding Stair";
  r->synonyms[0] = "stair";
  r->synonyms[1] = "stairway";
  r->flags = F_ONBIT | F_RLANDBIT;
  r->long_description =
      "The middle of a long, steep stairway carved into the face of a cliff.";
  r->down = R_BALCONY;
  r->up = R_COURTYARD;
  r->action = winding_stair_f;

  // R_COURTYARD (Update)
  r = &objects[R_COURTYARD];
  r->id = R_COURTYARD;
  r->description = "Courtyard";
  r->synonyms[0] = "courtyard";
  r->flags = F_ONBIT | F_RLANDBIT | F_FLOYDBIT;
  r->long_description =
      "You are in the courtyard of an ancient stone edifice, vaguely "
      "reminiscent of\n"
      "the castles you saw during your leave on Ramos Two. It has decayed to\n"
      "the point where it can probably be termed a ruin. Openings lead north "
      "and west,\n"
      "and a stairway downward is visible to the south.";
  r->south = R_WINDING_STAIR; // Also Down
  r->down = R_WINDING_STAIR;
  r->west = R_WEST_WING;
  r->north = R_PLAIN_HALL;
  r->action = courtyard_f;

  // Pseudo Objects for Courtyard
  o = &objects[O_CASTLE_PSEUDO];
  o->id = O_CASTLE_PSEUDO;
  o->description = "castle";
  o->long_description = "The castle is ancient and crumbling.";
  o->synonyms[0] = "castle";
  o->synonyms[1] = "edifice";
  o->synonyms[2] = "ruin";
  o->flags = F_NDESCBIT;
  o->action = castle_pseudo_action;
  obj_move(O_CASTLE_PSEUDO, R_COURTYARD);

  o = &objects[O_RUBBLE_PSEUDO];
  o->id = O_RUBBLE_PSEUDO;
  o->description = "rubble";
  o->long_description = "Don't be silly.";
  o->synonyms[0] = "rubble";
  o->flags = F_NDESCBIT;
  o->action = rubble_pseudo_action;
  obj_move(O_RUBBLE_PSEUDO, R_COURTYARD);

  // R_WEST_WING
  r = &objects[R_WEST_WING];
  r->id = R_WEST_WING;
  r->description = "West Wing";
  r->synonyms[0] = "wing";
  r->adjectives[0] = "west";
  r->long_description = "This was once the west wing of the castle, but the "
                        "walls are now mostly\n"
                        "rubble, allowing a view of the cliff and ocean below. "
                        "Rubble blocks all exits\n"
                        "save one, eastward to the courtyard.";
  r->flags = F_RLANDBIT | F_ONBIT | F_FLOYDBIT;
  r->east = R_COURTYARD;
  // Down leads to refusal message "Certain death." handled in main.c
  r->down = R_CERTAIN_DEATH_MSG;

  // Add Pseudos to West Wing (Castle, Rubble) - Need duplicate objects or
  // reuse? Objects can only be in one place. I need separate objects or a
  // "Global" list for room. My engine supports `globals[10]` on Room. I can put
  // `O_CASTLE_PSEUDO` in `OBJ_LOCAL_GLOBALS` (or just leave it) and add to
  // `r->globals`. But `O_CASTLE_PSEUDO` is defined as "Pseudo", usually local.
  // ZIL defines separate pseudos for each room sometimes or reuses.
  // Here `COURTYARD` and `WEST-WING` share "CASTLE" and "RUBBLE".
  // I'll make O_CASTLE_PSEUDO a global object visible in these rooms via
  // `globals` list? Or I'll just move it to `OBJ_LOCAL_GLOBALS` and add it to
  // `r->globals` for both rooms. Let's do that.

  obj_move(O_CASTLE_PSEUDO, OBJ_LOCAL_GLOBALS);
  objects[R_COURTYARD].globals[0] = O_CASTLE_PSEUDO;
  objects[R_WEST_WING].globals[0] = O_CASTLE_PSEUDO;

  obj_move(O_RUBBLE_PSEUDO, OBJ_LOCAL_GLOBALS);
  objects[R_COURTYARD].globals[1] = O_RUBBLE_PSEUDO;
  objects[R_WEST_WING].globals[1] = O_RUBBLE_PSEUDO;

  // R_PLAIN_HALL
  r = &objects[R_PLAIN_HALL];
  r->id = R_PLAIN_HALL;
  r->description = "Plain Hall";
  r->synonyms[0] = "hall";
  r->adjectives[0] = "plain";
  r->long_description =
      "This is a featureless hall leading north and south. Although the "
      "hallway is\n"
      "old and dusty, the construction is of a much more modern style than "
      "the\n"
      "castle to the south. A similar hall branches off to the northeast.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->south = R_COURTYARD;
  // R_PLAIN_HALL (Update Exits)
  // Note: r is already PLAIN_HALL from above
  r->north = R_REC_AREA;
  r->ne = R_REC_CORRIDOR;

  // R_REC_AREA
  r = &objects[R_REC_AREA];
  r->id = R_REC_AREA;
  r->description = "Rec Area";
  r->synonyms[0] = "area";
  r->adjectives[0] = "rec";
  r->long_description = "This is a recreational facility of some sort. Games "
                        "and tapes are scattered\n"
                        "about the room. Hallways head off to the east and "
                        "south, and to the north is\n"
                        "a door which is closed and locked. A dial on the door "
                        "is currently set to 0.";
  r->flags = F_ONBIT | F_RLANDBIT | F_FLOYDBIT;
  r->south = R_PLAIN_HALL;
  r->east = R_REC_CORRIDOR;
  r->north = R_CONFERENCE_ROOM;
  r->action = rec_area_f;
  r->globals[0] = O_CONFERENCE_DOOR;

  // Pseudo Objects for Rec Area
  o = &objects[O_GAMES_PSEUDO];
  o->id = O_GAMES_PSEUDO;
  o->description = "games";
  o->synonyms[0] = "games";
  o->synonyms[1] = "game";
  o->flags = F_NDESCBIT;
  o->action = games_pseudo_action;
  obj_move(O_GAMES_PSEUDO, R_REC_AREA);

  o = &objects[O_TAPES_PSEUDO];
  o->id = O_TAPES_PSEUDO;
  o->description = "tapes";
  o->synonyms[0] = "tapes";
  o->synonyms[1] = "tape";
  o->flags = F_NDESCBIT;
  o->action = tapes_pseudo_action;
  obj_move(O_TAPES_PSEUDO, R_REC_AREA);

  // R_CONFERENCE_ROOM
  r = &objects[R_CONFERENCE_ROOM];
  r->id = R_CONFERENCE_ROOM;
  r->description = "Conference Room";
  r->synonyms[0] = "room";
  r->adjectives[0] = "conference";
  r->long_description = "This is a fairly square room, almost filled by a "
                        "round conference table.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->south = R_REC_AREA;
  r->out = R_REC_AREA;
  r->north = R_BOOTH_1;
  r->in = R_BOOTH_1;
  r->action = conference_room_f;
  r->globals[0] = O_CONFERENCE_DOOR;
  r->globals[1] = O_TABLES;

  // Pseudo Object for Conference Room
  o = &objects[O_NEAR_BOOTH_PSEUDO];
  o->id = O_NEAR_BOOTH_PSEUDO;
  o->description = "booth";
  o->synonyms[0] = "booth";
  o->flags = F_NDESCBIT;
  o->action = near_booth_pseudo_action;
  obj_move(O_NEAR_BOOTH_PSEUDO, R_CONFERENCE_ROOM);

  // O_CONFERENCE_DOOR
  o = &objects[O_CONFERENCE_DOOR];
  o->id = O_CONFERENCE_DOOR;
  o->description = "door";
  o->synonyms[0] = "door";
  o->flags = F_DOORBIT | F_NDESCBIT;
  o->action = conference_door_f;
  obj_move(O_CONFERENCE_DOOR, OBJ_LOCAL_GLOBALS);

  // O_COMBINATION_DIAL
  o = &objects[O_COMBINATION_DIAL];
  o->id = O_COMBINATION_DIAL;
  o->description = "combination dial";
  o->synonyms[0] = "dial";
  o->adjectives[0] = "combination";
  o->flags = F_MUNGBIT | F_NDESCBIT;
  o->action = combination_dial_f;
  obj_move(O_COMBINATION_DIAL, R_REC_AREA);

  // R_BOOTH_1
  r = &objects[R_BOOTH_1];
  r->id = R_BOOTH_1;
  r->description = "Booth 1";
  r->synonyms[0] = "booth";
  r->adjectives[0] = "1";
  r->long_description = "This is a tiny room with a large \"1\" painted on the "
                        "wall. A panel contains\n"
                        "a slot about ten centimeters wide, a beige button "
                        "labelled \"2\" and a tan\n"
                        "button labelled \"3.\"";
  r->flags = F_ONBIT | F_RLANDBIT;
  r->south = R_CONFERENCE_ROOM;
  r->out = R_CONFERENCE_ROOM;

  o = &objects[O_IN_BOOTH_PSEUDO];
  o->id = O_IN_BOOTH_PSEUDO;
  o->description = "booth";
  o->synonyms[0] = "booth";
  o->flags = F_NDESCBIT;
  o->action = in_booth_pseudo_action;
  obj_move(O_IN_BOOTH_PSEUDO, R_BOOTH_1);

  // Common Pseudos for Dorms & SanFacs
  o = &objects[O_PARTITION_PSEUDO];
  o->id = O_PARTITION_PSEUDO;
  o->description = "partition";
  o->synonyms[0] = "partition";
  o->synonyms[1] = "partitions";
  o->flags = F_NDESCBIT;
  o->action = partition_pseudo_action;
  obj_move(O_PARTITION_PSEUDO, OBJ_LOCAL_GLOBALS);

  o = &objects[O_TOILET_PSEUDO];
  o->id = O_TOILET_PSEUDO;
  o->description = "toilet";
  o->synonyms[0] = "toilet";
  o->synonyms[1] = "fixtures";
  o->synonyms[2] = "fixture";
  o->synonyms[3] = "bowl";
  o->flags = F_NDESCBIT;
  o->action = toilet_pseudo_action;
  obj_move(O_TOILET_PSEUDO, OBJ_LOCAL_GLOBALS);

  // R_REC_CORRIDOR
  r = &objects[R_REC_CORRIDOR];
  r->id = R_REC_CORRIDOR;
  r->description = "Rec Corridor";
  r->synonyms[0] = "corridor";
  r->adjectives[0] = "rec";
  r->long_description = "This is a wide, east-west hallway. Portals lead north "
                        "and south, and another\n"
                        "corridor branches southwest.";
  r->flags = F_ONBIT | F_RLANDBIT;
  r->west = R_REC_AREA;
  r->north = R_DORM_B;
  r->south = R_DORM_A;
  r->east = R_MESS_CORRIDOR;
  r->sw = R_PLAIN_HALL;

  // R_DORM_A
  r = &objects[R_DORM_A];
  r->id = R_DORM_A;
  r->description = "Dorm A";
  r->synonyms[0] = "dorm";
  r->adjectives[0] = "a";
  r->long_description = "This is a very long room lined with multi-tiered "
                        "bunks. Flimsy partitions\n"
                        "between the tiers may have provided a modicum of "
                        "privacy. These spartan\n"
                        "living quarters could have once housed many hundreds, "
                        "but it seems quite\n"
                        "deserted now. There are openings at the north and "
                        "south ends of the room.";
  r->flags = F_FLOYDBIT | F_ONBIT | F_RLANDBIT;
  r->south = R_SANFAC_A;
  r->north = R_REC_CORRIDOR;
  r->globals[0] = O_BED;
  r->globals[1] = O_PARTITION_PSEUDO;

  // R_SANFAC_A
  r = &objects[R_SANFAC_A];
  r->id = R_SANFAC_A;
  r->description = "SanFac A";
  r->synonyms[0] = "sanfac";
  r->adjectives[0] = "a";
  r->long_description = "This must be the sanitary facility for the adjacent "
                        "dormitory. The fixtures\n"
                        "are dry and dusty, the room dead and deserted. You "
                        "marvel at how little the\n"
                        "millenia and cultural gulfs have changed toilet bowl "
                        "design. The only exit is\n"
                        "north.";
  r->flags = F_ONBIT | F_RLANDBIT | F_FLOYDBIT;
  r->north = R_DORM_A;
  r->globals[0] = O_TOILET_PSEUDO;

  // R_DORM_B
  r = &objects[R_DORM_B];
  r->id = R_DORM_B;
  r->description = "Dorm B";
  r->synonyms[0] = "dorm";
  r->adjectives[0] = "b";
  r->long_description = "This is a very long room lined with multi-tiered "
                        "bunks. Flimsy partitions\n"
                        "between the tiers may have provided a modicum of "
                        "privacy. These spartan\n"
                        "living quarters could have once housed many hundreds, "
                        "but it seems quite\n"
                        "deserted now. There are openings at the north and "
                        "south ends of the room.";
  r->flags = F_FLOYDBIT | F_ONBIT | F_RLANDBIT;
  r->south = R_REC_CORRIDOR;
  r->north = R_SANFAC_B;
  r->globals[0] = O_BED;
  r->globals[1] = O_PARTITION_PSEUDO;

  // R_SANFAC_B
  r = &objects[R_SANFAC_B];
  r->id = R_SANFAC_B;
  r->description = "SanFac B";
  r->synonyms[0] = "sanfac";
  r->adjectives[0] = "b";
  r->long_description = "This must be the sanitary facility for the adjacent "
                        "dormitory. The fixtures\n"
                        "are dry and dusty, the room dead and deserted. You "
                        "marvel at how little the\n"
                        "millenia and cultural gulfs have changed toilet bowl "
                        "design. The only exit is\n"
                        "south.";
  r->flags = F_FLOYDBIT | F_ONBIT | F_RLANDBIT;
  r->south = R_DORM_B;
  r->globals[0] = O_TOILET_PSEUDO;

  // R_DORM_C
  r = &objects[R_DORM_C];
  r->id = R_DORM_C;
  r->description = "Dorm C";
  r->synonyms[0] = "dorm";
  r->adjectives[0] = "c";
  r->long_description = "This is a very long room lined with multi-tiered "
                        "bunks. Flimsy partitions\n"
                        "between the tiers may have provided a modicum of "
                        "privacy. These spartan\n"
                        "living quarters could have once housed many hundreds, "
                        "but it seems quite\n"
                        "deserted now. There are openings at the north and "
                        "south ends of the room.";
  r->flags = F_ONBIT | F_FLOYDBIT | F_RLANDBIT;
  r->north = R_DORM_CORRIDOR;
  r->south = R_SANFAC_C;
  r->globals[0] = O_BED;
  r->globals[1] = O_PARTITION_PSEUDO;

  // R_SANFAC_C
  r = &objects[R_SANFAC_C];
  r->id = R_SANFAC_C;
  r->description = "SanFac C";
  r->synonyms[0] = "sanfac";
  r->adjectives[0] = "c";
  r->long_description = "This must be the sanitary facility for the adjacent "
                        "dormitory. The fixtures\n"
                        "are dry and dusty, the room dead and deserted. You "
                        "marvel at how little the\n"
                        "millenia and cultural gulfs have changed toilet bowl "
                        "design. The only exit is\n"
                        "north.";
  r->flags = F_FLOYDBIT | F_ONBIT | F_RLANDBIT;
  r->north = R_DORM_C;
  r->globals[0] = O_TOILET_PSEUDO;

  // R_DORM_D
  r = &objects[R_DORM_D];
  r->id = R_DORM_D;
  r->description = "Dorm D";
  r->synonyms[0] = "dorm";
  r->adjectives[0] = "d";
  r->long_description = "This is a very long room lined with multi-tiered "
                        "bunks. Flimsy partitions\n"
                        "between the tiers may have provided a modicum of "
                        "privacy. These spartan\n"
                        "living quarters could have once housed many hundreds, "
                        "but it seems quite\n"
                        "deserted now. There are openings at the north and "
                        "south ends of the room.";
  r->flags = F_FLOYDBIT | F_ONBIT | F_RLANDBIT;
  r->south = R_DORM_CORRIDOR;
  r->north = R_SANFAC_D;
  r->globals[0] = O_BED;
  r->globals[1] = O_PARTITION_PSEUDO;

  // R_SANFAC_D
  r = &objects[R_SANFAC_D];
  r->id = R_SANFAC_D;
  r->description = "SanFac D";
  r->synonyms[0] = "sanfac";
  r->adjectives[0] = "d";
  r->long_description = "This must be the sanitary facility for the adjacent "
                        "dormitory. The fixtures\n"
                        "are dry and dusty, the room dead and deserted. You "
                        "marvel at how little the\n"
                        "millenia and cultural gulfs have changed toilet bowl "
                        "design. The only exit is\n"
                        "south.";
  r->flags = F_ONBIT | F_FLOYDBIT | F_RLANDBIT;
  r->south = R_DORM_D;
  r->globals[0] = O_TOILET_PSEUDO;

  // R_MESS_CORRIDOR
  r = &objects[R_MESS_CORRIDOR];
  r->id = R_MESS_CORRIDOR;
  r->description = "Mess Corridor";
  r->synonyms[0] = "corridor";
  r->adjectives[0] = "mess";
  r->long_description =
      "This is a wide, east-west hallway with a large portal to the south. A\n"
      "small door to the north is closed and hooked with a simple steel "
      "padlock.";
  r->flags = F_ONBIT | F_RLANDBIT;
  r->south = R_MESS_HALL;
  r->north = R_STORAGE_WEST;
  r->east = R_DORM_CORRIDOR;
  r->west = R_REC_CORRIDOR;
  r->action = mess_corridor_f;
  r->globals[0] = O_STORAGE_WEST_DOOR;

  // O_STORAGE_WEST_DOOR
  o = &objects[O_STORAGE_WEST_DOOR];
  o->id = O_STORAGE_WEST_DOOR;
  o->description = "door";
  o->synonyms[0] = "door";
  o->flags = F_DOORBIT | F_NDESCBIT;
  o->action = storage_west_door_f;
  obj_move(O_STORAGE_WEST_DOOR, OBJ_LOCAL_GLOBALS);

  // O_PADLOCK
  o = &objects[O_PADLOCK];
  o->id = O_PADLOCK;
  o->description = "padlock";
  o->synonyms[0] = "padlock";
  o->synonyms[1] = "lock";
  o->adjectives[0] = "simple";
  o->adjectives[1] = "steel";
  o->size = 10;
  o->flags = F_MUNGBIT | F_NDESCBIT | F_TAKEBIT | F_TRYTAKEBIT;
  o->action = padlock_f;
  obj_move(O_PADLOCK, R_MESS_CORRIDOR);
  // R_MESS_HALL
  r = &objects[R_MESS_HALL];
  r->id = R_MESS_HALL;
  r->description = "Mess Hall";
  r->synonyms[0] = "hall";
  r->adjectives[0] = "mess";
  r->long_description = "This is a large hall lined with tables and benches. "
                        "An opening to the north\n"
                        "leads back to the corridor. A door to the south is "
                        "closed. Next to the door\n"
                        "is a small slot.";
  r->flags = F_ONBIT | F_RLANDBIT | F_FLOYDBIT;
  r->north = R_MESS_CORRIDOR;
  r->out = R_MESS_CORRIDOR;
  r->south = R_KITCHEN;
  r->in = R_KITCHEN;
  r->action = mess_hall_f;
  r->globals[0] = O_KITCHEN_DOOR;
  r->globals[1] = O_TABLES;
  r->globals[2] = O_BENCH_PSEUDO;

  // Pseudo Object for Mess Hall
  o = &objects[O_BENCH_PSEUDO];
  o->id = O_BENCH_PSEUDO;
  o->description = "bench";
  o->synonyms[0] = "bench";
  o->synonyms[1] = "benches";
  o->flags = F_NDESCBIT;
  o->action = bench_pseudo_action;
  obj_move(O_BENCH_PSEUDO, OBJ_LOCAL_GLOBALS);

  // R_KITCHEN
  r = &objects[R_KITCHEN];
  r->id = R_KITCHEN;
  r->description = "Kitchen";
  r->synonyms[0] = "kitchen";
  r->long_description = "This is the food production and dispensary area for "
                        "the dining hall to the\n"
                        "north. Of particular interest is a machine near the "
                        "door. You should probably\n"
                        "examine it more closely.";
  r->flags = F_ONBIT | F_RLANDBIT;
  r->north = R_MESS_HALL;
  r->out = R_MESS_HALL;
  r->action = kitchen_f;
  r->globals[0] = O_KITCHEN_DOOR;
  r->globals[1] = O_SPOUT_PSEUDO;
  r->globals[2] = O_KITCHEN_BUTTON_PSEUDO;

  // Pseudo Objects for Kitchen
  o = &objects[O_SPOUT_PSEUDO];
  o->id = O_SPOUT_PSEUDO;
  o->description = "spout";
  o->synonyms[0] = "spout";
  o->flags = F_NDESCBIT;
  o->action = spout_pseudo_action;
  obj_move(O_SPOUT_PSEUDO, OBJ_LOCAL_GLOBALS);

  o = &objects[O_KITCHEN_BUTTON_PSEUDO];
  o->id = O_KITCHEN_BUTTON_PSEUDO;
  o->description = "button";
  o->synonyms[0] = "button";
  o->flags = F_NDESCBIT;
  o->action = kitchen_button_pseudo_action;
  obj_move(O_KITCHEN_BUTTON_PSEUDO, OBJ_LOCAL_GLOBALS);

  // O_KITCHEN_DOOR
  o = &objects[O_KITCHEN_DOOR];
  o->id = O_KITCHEN_DOOR;
  o->description = "door";
  o->synonyms[0] = "door";
  o->adjectives[0] = "kitchen";
  o->flags = F_DOORBIT | F_NDESCBIT;
  o->action = kitchen_door_f;
  obj_move(O_KITCHEN_DOOR, OBJ_LOCAL_GLOBALS);

  // O_DISPENSER
  o = &objects[O_DISPENSER];
  o->id = O_DISPENSER;
  o->description = "dispenser unit";
  o->synonyms[0] = "unit";
  o->synonyms[1] = "niche";
  o->synonyms[2] = "machine";
  o->synonyms[3] = "dispenser";
  o->adjectives[0] = "dispenser";
  o->flags =
      F_MUNGBIT | F_CONTBIT | F_SEARCHBIT | F_OPENBIT | F_TRANSBIT | F_NDESCBIT;
  o->action = dispenser_f;
  obj_move(O_DISPENSER, R_KITCHEN);

  // O_CANTEEN
  o = &objects[O_CANTEEN];
  o->id = O_CANTEEN;
  o->description = "canteen";
  o->synonyms[0] = "canteen";
  o->adjectives[0] = "octagonal";
  o->flags = F_TAKEBIT | F_CONTBIT | F_SEARCHBIT;
  o->capacity = 5;
  o->action = canteen_f;
  obj_move(O_CANTEEN, R_MESS_HALL);

  // O_HIGH_PROTEIN
  o = &objects[O_HIGH_PROTEIN];
  o->id = O_HIGH_PROTEIN;
  o->description = "quantity of protein-rich liquid";
  o->synonyms[0] = "liquid";
  o->synonyms[1] = "fluid";
  o->synonyms[2] = "food";
  o->adjectives[0] = "brown";
  o->adjectives[1] = "protein-rich";
  o->flags = F_FOODBIT;
  o->action = high_protein_f;
  obj_move(O_HIGH_PROTEIN, OBJ_GLOBAL_OBJECTS);

  // R_STORAGE_WEST
  r = &objects[R_STORAGE_WEST];
  r->id = R_STORAGE_WEST;
  r->description = "Storage West";
  r->synonyms[0] = "storage";
  r->adjectives[0] = "west";
  r->long_description =
      "This is a small room obviously intended as a storage area.";
  r->flags = F_ONBIT | F_RLANDBIT;
  r->south = R_MESS_CORRIDOR;
  r->out = R_MESS_CORRIDOR;
  r->globals[0] = O_STORAGE_WEST_DOOR;

  // O_CAN
  o = &objects[O_CAN];
  o->id = O_CAN;
  o->description = "tin can";
  o->synonyms[0] = "can";
  o->adjectives[0] = "large";
  o->adjectives[1] = "tin";
  o->adjectives[2] = "unopened";
  o->flags = F_TAKEBIT;
  o->size = 15;
  o->action = can_f;
  obj_move(O_CAN, R_STORAGE_WEST);

  // O_LADDER
  o = &objects[O_LADDER];
  o->id = O_LADDER;
  o->description = "ladder";
  o->synonyms[0] = "ladder";
  o->adjectives[0] = "heavy";
  o->adjectives[1] = "duty";
  o->adjectives[2] = "aluminum";
  o->flags = F_TAKEBIT;
  o->size = 80;
  o->action = ladder_f;
  obj_move(O_LADDER, R_STORAGE_WEST);

  // Pseudo Object for Walkway
  o = &objects[O_WALKWAY_PSEUDO];
  o->id = O_WALKWAY_PSEUDO;
  o->description = "walkway";
  o->synonyms[0] = "walkway";
  o->flags = F_NDESCBIT;
  o->action = walkway_pseudo_action;
  obj_move(O_WALKWAY_PSEUDO, OBJ_LOCAL_GLOBALS);

  // R_DORM_CORRIDOR
  r = &objects[R_DORM_CORRIDOR];
  r->id = R_DORM_CORRIDOR;
  r->description = "Dorm Corridor";
  r->synonyms[0] = "corridor";
  r->adjectives[0] = "dorm";
  r->long_description =
      "This is a wide, east-west hallway with openings to the north and "
      "south.\n"
      "To the east, the corridor stretches off into the distance. That section "
      "of\n"
      "the hallway is lined with a motorized walkway (no longer running) that\n"
      "was probably intended to transport people or cargo down that "
      "tremendously\n"
      "long hall.";
  r->flags = F_ONBIT | F_RLANDBIT;
  r->north = R_DORM_D;
  r->south = R_DORM_C;
  r->west = R_MESS_CORRIDOR;
  r->east = R_CORRIDOR_JUNCTION;
  r->globals[0] = O_WALKWAY_PSEUDO;

  // R_CORRIDOR_JUNCTION
  r = &objects[R_CORRIDOR_JUNCTION];
  r->id = R_CORRIDOR_JUNCTION;
  r->description = "Corridor Junction";
  r->synonyms[0] = "junction";
  r->adjectives[0] = "corridor";
  r->long_description = "A north-south corridor intersects the main corridor "
                        "here. To the west, the\n"
                        "main corridor extends as far as you can see; a "
                        "non-working walkway from\n"
                        "that direction ends here. To the east, the corridor "
                        "widens into a well-lit\n"
                        "area.";
  r->flags = F_RLANDBIT | F_ONBIT | F_FLOYDBIT;
  r->north = R_ADMIN_CORRIDOR_S;
  r->south = R_MECH_CORRIDOR_N;
  r->east = R_ELEVATOR_LOBBY;
  r->west = R_DORM_CORRIDOR;
  r->globals[0] = O_WALKWAY_PSEUDO;

  // R_ADMIN_CORRIDOR_S
  r = &objects[R_ADMIN_CORRIDOR_S];
  r->id = R_ADMIN_CORRIDOR_S;
  r->description = "Admin Corridor South";
  r->synonyms[0] = "corridor";
  r->adjectives[0] = "admin";
  r->adjectives[1] = "south";
  r->long_description =
      "This section of hallway seems to have suffered some minor structural\n"
      "damage. The walls are cracked, and a jagged crevice crosses the floor.\n"
      "An opening leads east and the corridor heads north and south.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->south = R_CORRIDOR_JUNCTION;
  r->north = R_ADMIN_CORRIDOR;
  r->east = R_SANFAC_E;
  r->action = admin_corridor_s_f;

  // O_CREVICE
  o = &objects[O_CREVICE];
  o->id = O_CREVICE;
  o->description = "crevice";
  o->synonyms[0] = "crevice";
  o->synonyms[1] = "crack";
  o->synonyms[2] = "hole";
  o->adjectives[0] = "narrow";
  o->adjectives[1] = "jagged";
  o->flags = F_NDESCBIT;
  o->action = crevice_f;
  obj_move(O_CREVICE, R_ADMIN_CORRIDOR_S);

  // O_KEY
  o = &objects[O_KEY];
  o->id = O_KEY;
  o->description = "key";
  o->synonyms[0] = "key";
  o->synonyms[1] = "metal";
  o->synonyms[2] = "object";
  o->adjectives[0] = "steel";
  o->adjectives[1] = "shiny";
  o->adjectives[2] = "iron";
  o->flags =
      F_TAKEBIT | F_TOOLBIT | F_INVISIBLE | F_TRYTAKEBIT | F_ACIDBIT;
  o->size = 3;
  o->action = key_f;
  obj_move(O_KEY, R_ADMIN_CORRIDOR_S);

  // R_SANFAC_E
  r = &objects[R_SANFAC_E];
  r->id = R_SANFAC_E;
  r->description = "SanFac E";
  r->synonyms[0] = "sanfac";
  r->adjectives[0] = "e";
  r->long_description =
      "Here is another sanitary facility. Like the others, it is dusty and\n"
      "deserted.";
  r->flags = F_ONBIT | F_FLOYDBIT | F_RLANDBIT;
  r->west = R_ADMIN_CORRIDOR_S;
  r->out = R_ADMIN_CORRIDOR_S;
  r->globals[0] = O_TOILET_PSEUDO;

  // R_ADMIN_CORRIDOR
  r = &objects[R_ADMIN_CORRIDOR];
  r->id = R_ADMIN_CORRIDOR;
  r->description = "Admin Corridor";
  r->synonyms[0] = "corridor";
  r->adjectives[0] = "admin";
  r->long_description =
      "The hallway, in fact the entire building, has been rent apart here, presumably\n"
      "by seismic upheaval. You can see the sky through the severed roof above, and\n"
      "the ground is thick with rubble. To the north is a gaping rift, at least eight\n"
      "meters across and thirty meters deep. A wide doorway, labelled \"Sistumz Moniturz,\" leads west.";
  r->flags = F_ONBIT | F_RLANDBIT;
  r->south = R_ADMIN_CORRIDOR_S;
  r->north = R_ADMIN_CORRIDOR_N;
  r->west = R_SYSTEMS_MONITORS;
  r->action = admin_corridor_f;
  r->globals[0] = O_RIFT;
  r->globals[1] = O_RUBBLE_PSEUDO;

  // R_ADMIN_CORRIDOR_N
  r = &objects[R_ADMIN_CORRIDOR_N];
  r->id = R_ADMIN_CORRIDOR_N;
  r->description = "Admin Corridor North";
  r->synonyms[0] = "corridor";
  r->adjectives[0] = "admin";
  r->adjectives[1] = "north";
  r->long_description =
      "The corridor ends here. Portals lead west, north, and east. Signs above these\n"
      "portals read, respectively, \"Administraativ Awfisiz,\" \"Tranzportaashun\n"
      "Suplii,\" and \"Plan Ruum.\" To the south is a wide rift separating this area from the rest of the building.";
  r->flags = F_ONBIT | F_RLANDBIT;
  r->south = R_ADMIN_CORRIDOR;
  r->north = R_TRANSPORTATION_SUPPLY;
  r->east = R_PLAN_ROOM;
  r->west = R_SMALL_OFFICE;
  r->action = admin_corridor_n_f;
  r->globals[0] = O_RIFT;

  // O_RIFT
  o = &objects[O_RIFT];
  o->id = O_RIFT;
  o->description = "rift";
  o->synonyms[0] = "rift";
  o->synonyms[1] = "gulf";
  o->synonyms[2] = "precipice";
  o->synonyms[3] = "hole";
  o->adjectives[0] = "wide";
  o->adjectives[1] = "deep";
  o->adjectives[2] = "gaping";
  o->flags = F_NDESCBIT;
  o->action = rift_f;
  obj_move(O_RIFT, OBJ_LOCAL_GLOBALS);

  // R_SYSTEMS_MONITORS
  r = &objects[R_SYSTEMS_MONITORS];
  r->id = R_SYSTEMS_MONITORS;
  r->description = "Systems Monitors";
  r->synonyms[0] = "monitors";
  r->synonyms[1] = "room";
  r->adjectives[0] = "systems";
  r->long_description = "This is a large room filled with tables full of strange equipment.";
  r->flags = F_FLOYDBIT | F_RLANDBIT | F_ONBIT;
  r->east = R_ADMIN_CORRIDOR;
  r->out = R_ADMIN_CORRIDOR;
  r->action = systems_monitors_f;
  r->globals[0] = O_TABLES;
  r->globals[1] = O_MONITORS_PSEUDO;
  r->globals[2] = O_EQUIPMENT_PSEUDO;

  // Pseudo Objects for Systems Monitors
  o = &objects[O_MONITORS_PSEUDO];
  o->id = O_MONITORS_PSEUDO;
  o->description = "monitors";
  o->synonyms[0] = "monitors";
  o->synonyms[1] = "monitor";
  o->flags = F_NDESCBIT;
  o->action = monitors_pseudo_action;
  obj_move(O_MONITORS_PSEUDO, OBJ_LOCAL_GLOBALS);

  o = &objects[O_EQUIPMENT_PSEUDO];
  o->id = O_EQUIPMENT_PSEUDO;
  o->description = "equipment";
  o->synonyms[0] = "equipment";
  o->synonyms[1] = "machines";
  o->flags = F_NDESCBIT;
  o->action = equipment_pseudo_action;
  obj_move(O_EQUIPMENT_PSEUDO, OBJ_LOCAL_GLOBALS);

  // R_PLAN_ROOM
  r = &objects[R_PLAN_ROOM];
  r->id = R_PLAN_ROOM;
  r->description = "Plan Room";
  r->synonyms[0] = "room";
  r->adjectives[0] = "plan";
  r->long_description =
      "This is a small room whose far wall is covered with many small cubbyholes,\n"
      "all empty. The left wall is covered with an enormous map, labelled\n"
      "\"Kalamontee Kompleks\", showing two installations connected by a long\n"
      "hallway. Near the upper part of this map is a red arrow saying \"Yuu ar\n"
      "heer.\" The right wall is covered with a similar map, labelled \"Lawanda\n"
      "Kompleks\", showing two installations, one apparently buried deep\n"
      "underground.";
  r->flags = F_RLANDBIT | F_ONBIT | F_FLOYDBIT;
  r->west = R_ADMIN_CORRIDOR_N;
  r->out = R_ADMIN_CORRIDOR_N;
  r->globals[0] = O_CUBBYHOLE_PSEUDO;
  r->globals[1] = O_MAPS_PSEUDO;

  // Pseudo Objects for Plan Room
  o = &objects[O_CUBBYHOLE_PSEUDO];
  o->id = O_CUBBYHOLE_PSEUDO;
  o->description = "cubbyhole";
  o->synonyms[0] = "cubbyhole";
  o->synonyms[1] = "cubbyholes";
  o->flags = F_NDESCBIT;
  o->action = cubbyhole_pseudo_action;
  obj_move(O_CUBBYHOLE_PSEUDO, OBJ_LOCAL_GLOBALS);

  o = &objects[O_MAPS_PSEUDO];
  o->id = O_MAPS_PSEUDO;
  o->description = "map";
  o->synonyms[0] = "map";
  o->synonyms[1] = "maps";
  o->flags = F_NDESCBIT;
  o->action = maps_pseudo_action;
  obj_move(O_MAPS_PSEUDO, OBJ_LOCAL_GLOBALS);

  // R_TRANSPORTATION_SUPPLY
  r = &objects[R_TRANSPORTATION_SUPPLY];
  r->id = R_TRANSPORTATION_SUPPLY;
  r->description = "Transportation Supply";
  r->synonyms[0] = "supply";
  r->synonyms[1] = "room";
  r->adjectives[0] = "transportation";
  r->long_description =
      "This is a large room with exits in all directions. Unfortunately, the room\n"
      "is completely empty.";
  r->flags = F_RLANDBIT;
  r->south = R_ADMIN_CORRIDOR_N;
  r->out = R_ADMIN_CORRIDOR_N;

  // R_SMALL_OFFICE
  r = &objects[R_SMALL_OFFICE];
  r->id = R_SMALL_OFFICE;
  r->description = "Small Office";
  r->synonyms[0] = "office";
  r->adjectives[0] = "small";
  r->long_description =
      "You have entered a small office of some sort. A small desk faces the main\n"
      "doorway which lies to the east. Another exit leads west.";
  r->flags = F_RLANDBIT | F_FLOYDBIT | F_ONBIT;
  r->east = R_ADMIN_CORRIDOR_N;
  r->west = R_LARGE_OFFICE;

  // O_SMALL_DESK
  o = &objects[O_SMALL_DESK];
  o->id = O_SMALL_DESK;
  o->description = "small desk";
  o->synonyms[0] = "desk";
  o->synonyms[1] = "drawer";
  o->adjectives[0] = "small";
  o->flags = F_NDESCBIT | F_CONTBIT | F_SEARCHBIT;
  o->capacity = 10;
  o->action = desk_f;
  obj_move(O_SMALL_DESK, R_SMALL_OFFICE);

  // O_KITCHEN_CARD
  o = &objects[O_KITCHEN_CARD];
  o->id = O_KITCHEN_CARD;
  o->description = "kitchen access card";
  o->synonyms[0] = "card";
  o->synonyms[1] = "cards";
  o->adjectives[0] = "kitchen";
  o->adjectives[1] = "access";
  o->flags = F_TAKEBIT | F_READBIT;
  o->text = "The card is embossed \"kitcin akses kard.\"";
  o->value = 1;
  o->size = 3;
  obj_move(O_KITCHEN_CARD, O_SMALL_DESK);

  // O_UPPER_ELEVATOR_CARD
  o = &objects[O_UPPER_ELEVATOR_CARD];
  o->id = O_UPPER_ELEVATOR_CARD;
  o->description = "upper elevator access card";
  o->synonyms[0] = "card";
  o->synonyms[1] = "cards";
  o->adjectives[0] = "upper";
  o->adjectives[1] = "elevator";
  o->adjectives[2] = "access";
  o->flags = F_TAKEBIT | F_READBIT | F_VOWELBIT;
  o->text = "The card is embossed \"upur elivaatur akses kard.\"";
  o->value = 1;
  o->size = 3;
  obj_move(O_UPPER_ELEVATOR_CARD, O_SMALL_DESK);

  // R_LARGE_OFFICE
  r = &objects[R_LARGE_OFFICE];
  r->id = R_LARGE_OFFICE;
  r->description = "Large Office";
  r->synonyms[0] = "office";
  r->adjectives[0] = "large";
  r->long_description =
      "This is a large, plush office. The far wall is one large picture window,\n"
      "scratched but unbroken, offering a view of this installation and the ocean\n"
      "beyond. In front of the window is a wide wooden desk. The only exit is east.";
  r->flags = F_FLOYDBIT | F_RLANDBIT | F_ONBIT;
  r->east = R_SMALL_OFFICE;
  r->out = R_SMALL_OFFICE;

  // O_LARGE_DESK
  o = &objects[O_LARGE_DESK];
  o->id = O_LARGE_DESK;
  o->description = "large desk";
  o->synonyms[0] = "desk";
  o->synonyms[1] = "drawer";
  o->adjectives[0] = "large";
  o->adjectives[1] = "wide";
  o->adjectives[2] = "wooden";
  o->flags = F_NDESCBIT | F_CONTBIT | F_SEARCHBIT;
  o->capacity = 20;
  o->action = desk_f;
  obj_move(O_LARGE_DESK, R_LARGE_OFFICE);

  // O_SHUTTLE_CARD
  o = &objects[O_SHUTTLE_CARD];
  o->id = O_SHUTTLE_CARD;
  o->description = "shuttle access card";
  o->synonyms[0] = "card";
  o->synonyms[1] = "cards";
  o->adjectives[0] = "shuttle";
  o->adjectives[1] = "access";
  o->flags = F_TAKEBIT | F_READBIT;
  o->text = "The card is embossed \"shutul akses kard.\"";
  o->value = 1;
  o->size = 3;
  obj_move(O_SHUTTLE_CARD, O_LARGE_DESK);

  // O_LOWER_ELEVATOR_CARD
  o = &objects[O_LOWER_ELEVATOR_CARD];
  o->id = O_LOWER_ELEVATOR_CARD;
  o->description = "lower elevator access card";
  o->synonyms[0] = "card";
  o->synonyms[1] = "cards";
  o->adjectives[0] = "lower";
  o->adjectives[1] = "elevator";
  o->adjectives[2] = "access";
  o->flags = F_TAKEBIT | F_READBIT;
  o->text = "The card is embossed \"loowur elivaatur akses kard.\"";
  o->value = 1;
  o->size = 3;
  obj_move(O_LOWER_ELEVATOR_CARD, O_LARGE_DESK);

  // R_MECH_CORRIDOR_N
  r = &objects[R_MECH_CORRIDOR_N];
  r->id = R_MECH_CORRIDOR_N;
  r->description = "Mech Corridor North";
  r->synonyms[0] = "corridor";
  r->adjectives[0] = "mech";
  r->adjectives[1] = "north";
  r->long_description =
      "Entrances to rooms lie to the east and west from this north-south hall.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->north = R_CORRIDOR_JUNCTION;
  r->south = R_MECH_CORRIDOR;
  r->east = R_STORAGE_EAST;
  r->west = R_PHYSICAL_PLANT;

  // R_MECH_CORRIDOR
  r = &objects[R_MECH_CORRIDOR];
  r->id = R_MECH_CORRIDOR;
  r->description = "Mech Corridor";
  r->synonyms[0] = "corridor";
  r->adjectives[0] = "mech";
  r->long_description =
      "Entrances to rooms lie to the east and west from this north-south hall.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->north = R_MECH_CORRIDOR_N;
  r->south = R_MECH_CORRIDOR_S;
  r->east = R_REACTOR_CONTROL;
  r->west = R_PHYSICAL_PLANT;

  // R_MECH_CORRIDOR_S
  r = &objects[R_MECH_CORRIDOR_S];
  r->id = R_MECH_CORRIDOR_S;
  r->description = "Mech Corridor South";
  r->synonyms[0] = "corridor";
  r->adjectives[0] = "mech";
  r->adjectives[1] = "south";
  r->long_description =
      "The corridor ends here with doorways to the southwest, south, and southeast.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->north = R_MECH_CORRIDOR;
  r->south = R_MACHINE_SHOP;
  r->sw = R_TOOL_ROOM;
  r->se = R_ROBOT_SHOP;

  // R_STORAGE_EAST
  r = &objects[R_STORAGE_EAST];
  r->id = R_STORAGE_EAST;
  r->description = "Storage East";
  r->synonyms[0] = "storage";
  r->adjectives[0] = "east";
  r->long_description = "A small room for storage. The exit is to the west.";
  r->flags = F_RLANDBIT | F_FLOYDBIT | F_ONBIT;
  r->west = R_MECH_CORRIDOR_N;
  r->out = R_MECH_CORRIDOR_N;
  r->globals[0] = O_SHELVES;

  // O_OIL_CAN
  o = &objects[O_OIL_CAN];
  o->id = O_OIL_CAN;
  o->description = "oil can";
  o->synonyms[0] = "can";
  o->adjectives[0] = "small";
  o->adjectives[1] = "oil";
  o->flags = F_VOWELBIT | F_TAKEBIT;
  o->size = 10;
  o->action = oil_can_f;
  obj_move(O_OIL_CAN, R_STORAGE_EAST);

  // O_CARTON
  o = &objects[O_CARTON];
  o->id = O_CARTON;
  o->description = "cardboard box";
  o->synonyms[0] = "box";
  o->synonyms[1] = "carton";
  o->adjectives[0] = "small";
  o->adjectives[1] = "cardboard";
  o->flags = F_TAKEBIT | F_CONTBIT | F_SEARCHBIT | F_OPENBIT;
  o->size = 10;
  o->capacity = 50;
  o->action = carton_f;
  obj_move(O_CARTON, R_STORAGE_EAST);

  // O_CRACKED_BOARD
  o = &objects[O_CRACKED_BOARD];
  o->id = O_CRACKED_BOARD;
  o->description = "cracked seventeen-centimeter fromitz board";
  o->synonyms[0] = "board";
  o->synonyms[1] = "boards";
  o->adjectives[0] = "cracked";
  o->adjectives[1] = "seventeen";
  o->adjectives[2] = "centimeter";
  o->adjectives[3] = "fromitz";
  o->flags = F_ACIDBIT | F_TAKEBIT;
  o->size = 10;
  o->action = cracked_board_f;
  obj_move(O_CRACKED_BOARD, O_CARTON);

  // O_MEGAFUSE_B
  o = &objects[O_MEGAFUSE_B];
  o->id = O_MEGAFUSE_B;
  o->description = "B-series megafuse";
  o->synonyms[0] = "fuse";
  o->synonyms[1] = "megafuse";
  o->adjectives[0] = "b-series";
  o->adjectives[1] = "b";
  o->adjectives[2] = "series";
  o->adjectives[3] = "mega";
  o->flags = F_ACIDBIT | F_TAKEBIT;
  o->size = 5;
  obj_move(O_MEGAFUSE_B, O_CARTON);

  // O_MEGAFUSE_K
  o = &objects[O_MEGAFUSE_K];
  o->id = O_MEGAFUSE_K;
  o->description = "K-series megafuse";
  o->synonyms[0] = "fuse";
  o->synonyms[1] = "megafuse";
  o->adjectives[0] = "k-series";
  o->adjectives[1] = "k";
  o->adjectives[2] = "series";
  o->adjectives[3] = "mega";
  o->flags = F_ACIDBIT | F_TAKEBIT;
  o->size = 5;
  obj_move(O_MEGAFUSE_K, O_CARTON);

  // O_GOOD_BEDISTOR
  o = &objects[O_GOOD_BEDISTOR];
  o->id = O_GOOD_BEDISTOR;
  o->description = "good ninety-ohm bedistor";
  o->synonyms[0] = "bedistor";
  o->adjectives[0] = "good";
  o->adjectives[1] = "ninety";
  o->adjectives[2] = "ohm";
  o->flags = F_ACIDBIT | F_TAKEBIT;
  o->size = 8;
  o->action = good_bedistor_f;
  obj_move(O_GOOD_BEDISTOR, O_CARTON);

  // R_PHYSICAL_PLANT
  r = &objects[R_PHYSICAL_PLANT];
  r->id = R_PHYSICAL_PLANT;
  r->description = "Physical Plant";
  r->synonyms[0] = "plant";
  r->adjectives[0] = "physical";
  r->long_description =
      "This is a huge, dim room with exits in the northeast and southeast\n"
      "corners. The room is criss-crossed with catwalks and is filled with\n"
      "heavy equipment presumably intended to heat and ventilate this complex.\n"
      "Hardly any of the equipment is still operating.";
  r->flags = F_FLOYDBIT | F_RLANDBIT | F_ONBIT;
  r->ne = R_MECH_CORRIDOR_N;
  r->se = R_MECH_CORRIDOR;
  r->globals[0] = O_EQUIPMENT_PSEUDO;
  r->globals[1] = O_CATWALK_PSEUDO;

  // Pseudo Object for Catwalk
  o = &objects[O_CATWALK_PSEUDO];
  o->id = O_CATWALK_PSEUDO;
  o->description = "catwalk";
  o->synonyms[0] = "catwalk";
  o->synonyms[1] = "catwalks";
  o->flags = F_NDESCBIT;
  o->action = catwalk_pseudo_action;
  obj_move(O_CATWALK_PSEUDO, OBJ_LOCAL_GLOBALS);

  // R_REACTOR_CONTROL
  r = &objects[R_REACTOR_CONTROL];
  r->id = R_REACTOR_CONTROL;
  r->description = "Reactor Control";
  r->synonyms[0] = "control";
  r->synonyms[1] = "room";
  r->adjectives[0] = "reactor";
  r->long_description =
      "This room contains many dials and gauges for controlling a massive planetary\n"
      "power reactor which, according to a diagram on the wall, must be buried far\n"
      "below this very complex. The exit is to the west. To the east is a metal door,\n"
      "and next to it, a button. A dark stairway winds downward.";
  r->flags = F_RLANDBIT | F_FLOYDBIT | F_ONBIT;
  r->west = R_MECH_CORRIDOR;
  r->east = R_REACTOR_ELEVATOR;
  r->in = R_REACTOR_ELEVATOR;
  r->down = R_REACTOR_ACCESS_STAIRS;
  r->globals[0] = O_CONTROLS;
  r->globals[1] = O_STAIRS;
  r->globals[2] = O_REACTOR_ELEVATOR_DOOR;
  r->globals[3] = O_REACTOR_BUTTON_PSEUDO;
  r->globals[4] = O_DIAGRAM_PSEUDO;

  // O_REACTOR_ELEVATOR_DOOR
  o = &objects[O_REACTOR_ELEVATOR_DOOR];
  o->id = O_REACTOR_ELEVATOR_DOOR;
  o->description = "reactor elevator door";
  o->synonyms[0] = "door";
  o->adjectives[0] = "reactor";
  o->adjectives[1] = "elevator";
  o->adjectives[2] = "metal";
  o->flags = F_DOORBIT;
  o->action = reactor_elevator_door_f;
  obj_move(O_REACTOR_ELEVATOR_DOOR, OBJ_LOCAL_GLOBALS);

  // O_REACTOR_BUTTON_PSEUDO
  o = &objects[O_REACTOR_BUTTON_PSEUDO];
  o->id = O_REACTOR_BUTTON_PSEUDO;
  o->description = "button";
  o->synonyms[0] = "button";
  o->flags = F_NDESCBIT;
  o->action = reactor_button_pseudo_action;
  obj_move(O_REACTOR_BUTTON_PSEUDO, OBJ_LOCAL_GLOBALS);

  // O_DIAGRAM_PSEUDO
  o = &objects[O_DIAGRAM_PSEUDO];
  o->id = O_DIAGRAM_PSEUDO;
  o->description = "diagram";
  o->synonyms[0] = "diagram";
  o->flags = F_NDESCBIT;
  o->action = diagram_pseudo_action;
  obj_move(O_DIAGRAM_PSEUDO, OBJ_LOCAL_GLOBALS);

  // R_REACTOR_ACCESS_STAIRS
  r = &objects[R_REACTOR_ACCESS_STAIRS];
  r->id = R_REACTOR_ACCESS_STAIRS;
  r->description = "Reactor Access Stairs";
  r->synonyms[0] = "stairs";
  r->synonyms[1] = "stairway";
  r->adjectives[0] = "reactor";
  r->adjectives[1] = "access";
  r->long_description = "The stairway seems to go down forever into darkness. The only exit is up.";
  r->flags = F_RLANDBIT;
  r->up = R_REACTOR_CONTROL;
  r->down = R_REACTOR_ACCESS_STAIRS;
  r->globals[0] = O_STAIRS;

  // R_REACTOR_ELEVATOR
  r = &objects[R_REACTOR_ELEVATOR];
  r->id = R_REACTOR_ELEVATOR;
  r->description = "Reactor Elevator";
  r->synonyms[0] = "elevator";
  r->adjectives[0] = "reactor";
  r->long_description =
      "This is an elevator with a door to the west, currently open. A control panel\n"
      "contains an Up button, a Down button, and a small slot.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->west = R_REACTOR_CONTROL;
  r->out = R_REACTOR_CONTROL;
  r->globals[0] = O_REACTOR_ELEVATOR_DOOR;
  r->globals[1] = O_SLOT;
  r->globals[2] = O_CONTROLS;

  // R_TOOL_ROOM
  r = &objects[R_TOOL_ROOM];
  r->id = R_TOOL_ROOM;
  r->description = "Tool Room";
  r->synonyms[0] = "room";
  r->adjectives[0] = "tool";
  r->long_description = "This is apparently a storage room for tools. Exits lead northeast and east.";
  r->flags = F_RLANDBIT | F_FLOYDBIT | F_ONBIT;
  r->ne = R_MECH_CORRIDOR_S;
  r->east = R_MACHINE_SHOP;
  r->globals[0] = O_SHELVES;

  // O_FLASK
  o = &objects[O_FLASK];
  o->id = O_FLASK;
  o->description = "glass flask";
  o->synonyms[0] = "flask";
  o->adjectives[0] = "glass";
  o->adjectives[1] = "large";
  o->adjectives[2] = "plastic";
  o->flags = F_CONTBIT | F_SEARCHBIT | F_OPENBIT | F_TAKEBIT;
  o->size = 10;
  o->capacity = 2;
  o->action = flask_f;
  obj_move(O_FLASK, R_TOOL_ROOM);

  // O_MAGNET
  o = &objects[O_MAGNET];
  o->id = O_MAGNET;
  o->description = "curved metal bar";
  o->synonyms[0] = "magnet";
  o->synonyms[1] = "bar";
  o->adjectives[0] = "curved";
  o->adjectives[1] = "metal";
  o->adjectives[2] = "horseshoe";
  o->flags = F_TRYTAKEBIT | F_TAKEBIT;
  o->size = 10;
  o->action = magnet_f;
  obj_move(O_MAGNET, R_TOOL_ROOM);

  // O_PLIERS
  o = &objects[O_PLIERS];
  o->id = O_PLIERS;
  o->description = "pair of wide-nosed pliers";
  o->synonyms[0] = "pair";
  o->synonyms[1] = "pliers";
  o->adjectives[0] = "wide-nosed";
  o->adjectives[1] = "wide";
  o->adjectives[2] = "nosed";
  o->flags = F_TAKEBIT;
  o->size = 15;
  obj_move(O_PLIERS, R_TOOL_ROOM);

  // R_MACHINE_SHOP
  r = &objects[R_MACHINE_SHOP];
  r->id = R_MACHINE_SHOP;
  r->description = "Machine Shop";
  r->synonyms[0] = "shop";
  r->adjectives[0] = "machine";
  r->long_description =
      "This room is probably some sort of machine shop filled with a variety\n"
      "of unusual machines. Doorways lead north, east, and west.\n"
      "\n"
      "Standing against the rear wall is a large dispensing machine with a\n"
      "spout. The dispenser is lined with brightly-colored buttons. The first four\n"
      "buttons, labelled \"KUULINTS 1 - 4\", are colored red, blue, green, and\n"
      "yellow. The next three buttons, labelled \"KATALISTS 1 - 3\", are colored\n"
      "gray, brown, and black. The last two buttons are both white. One of\n"
      "these is square and says \"BAAS.\" The other white button is round and\n"
      "says \"ASID.\"";
  r->flags = F_RLANDBIT | F_FLOYDBIT | F_ONBIT;
  r->north = R_MECH_CORRIDOR_S;
  r->east = R_ROBOT_SHOP;
  r->west = R_TOOL_ROOM;
  r->action = machine_shop_f;
  r->globals[0] = O_CHEM_SPOUT_PSEUDO;

  // Pseudo: SPOUT
  o = &objects[O_CHEM_SPOUT_PSEUDO];
  o->id = O_CHEM_SPOUT_PSEUDO;
  o->description = "spout";
  o->synonyms[0] = "spout";
  o->flags = F_NDESCBIT;
  o->action = chem_spout_pseudo_action;
  obj_move(O_CHEM_SPOUT_PSEUDO, OBJ_LOCAL_GLOBALS);

  // O_CHEMICAL_DISPENSER
  o = &objects[O_CHEMICAL_DISPENSER];
  o->id = O_CHEMICAL_DISPENSER;
  o->description = "chemical dispenser";
  o->synonyms[0] = "dispenser";
  o->synonyms[1] = "machine";
  o->adjectives[0] = "chemical";
  o->adjectives[1] = "large";
  o->flags = F_MUNGBIT | F_NDESCBIT;
  o->action = chemical_dispenser_f;
  obj_move(O_CHEMICAL_DISPENSER, R_MACHINE_SHOP);

  // Buttons in Machine Shop
  o = &objects[O_RED_BUTTON];
  o->id = O_RED_BUTTON;
  o->description = "red button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "red";
  o->flags = F_NDESCBIT;
  o->action = chem_button_f;
  obj_move(O_RED_BUTTON, R_MACHINE_SHOP);

  o = &objects[O_BLUE_BUTTON];
  o->id = O_BLUE_BUTTON;
  o->description = "blue button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "blue";
  o->flags = F_NDESCBIT;
  o->action = chem_button_f;
  obj_move(O_BLUE_BUTTON, R_MACHINE_SHOP);

  o = &objects[O_GREEN_BUTTON];
  o->id = O_GREEN_BUTTON;
  o->description = "green button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "green";
  o->flags = F_NDESCBIT;
  o->action = chem_button_f;
  obj_move(O_GREEN_BUTTON, R_MACHINE_SHOP);

  o = &objects[O_YELLOW_BUTTON];
  o->id = O_YELLOW_BUTTON;
  o->description = "yellow button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "yellow";
  o->flags = F_NDESCBIT;
  o->action = chem_button_f;
  obj_move(O_YELLOW_BUTTON, R_MACHINE_SHOP);

  o = &objects[O_GRAY_BUTTON];
  o->id = O_GRAY_BUTTON;
  o->description = "gray button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "gray";
  o->flags = F_NDESCBIT;
  o->action = chem_button_f;
  obj_move(O_GRAY_BUTTON, R_MACHINE_SHOP);

  o = &objects[O_BROWN_BUTTON];
  o->id = O_BROWN_BUTTON;
  o->description = "brown button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "brown";
  o->flags = F_NDESCBIT;
  o->action = chem_button_f;
  obj_move(O_BROWN_BUTTON, R_MACHINE_SHOP);

  o = &objects[O_BLACK_BUTTON];
  o->id = O_BLACK_BUTTON;
  o->description = "black button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "black";
  o->flags = F_NDESCBIT;
  o->action = chem_button_f;
  obj_move(O_BLACK_BUTTON, R_MACHINE_SHOP);

  o = &objects[O_ROUND_WHITE_BUTTON];
  o->id = O_ROUND_WHITE_BUTTON;
  o->description = "round white button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "white";
  o->adjectives[1] = "round";
  o->flags = F_NDESCBIT;
  o->action = chem_button_f;
  obj_move(O_ROUND_WHITE_BUTTON, R_MACHINE_SHOP);

  o = &objects[O_SQUARE_WHITE_BUTTON];
  o->id = O_SQUARE_WHITE_BUTTON;
  o->description = "square white button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "white";
  o->adjectives[1] = "square";
  o->flags = F_NDESCBIT;
  o->action = chem_button_f;
  obj_move(O_SQUARE_WHITE_BUTTON, R_MACHINE_SHOP);

  // O_CHEMICAL_FLUID
  o = &objects[O_CHEMICAL_FLUID];
  o->id = O_CHEMICAL_FLUID;
  o->description = "chemical fluid";
  o->synonyms[0] = "fluid";
  o->synonyms[1] = "liquid";
  o->adjectives[0] = "chemical";
  o->flags = 0;

  // R_ROBOT_SHOP
  r = &objects[R_ROBOT_SHOP];
  r->id = R_ROBOT_SHOP;
  r->description = "Robot Shop";
  r->synonyms[0] = "shop";
  r->adjectives[0] = "robot";
  r->long_description =
      "This room, with exits west and northwest, is filled with robot-like\n"
      "devices of every conceivable description, all in various states of\n"
      "disassembly.";
  r->flags = F_RLANDBIT | F_FLOYDBIT | F_ONBIT;
  r->west = R_MACHINE_SHOP;
  r->nw = R_MECH_CORRIDOR_S;
  r->globals[0] = O_DEVICES_PSEUDO;

  // Pseudo: DEVICES
  o = &objects[O_DEVICES_PSEUDO];
  o->id = O_DEVICES_PSEUDO;
  o->description = "devices";
  o->synonyms[0] = "device";
  o->synonyms[1] = "devices";
  o->flags = F_NDESCBIT;
  o->action = devices_pseudo_action;
  obj_move(O_DEVICES_PSEUDO, OBJ_LOCAL_GLOBALS);

  // O_FLOYD
  o = &objects[O_FLOYD];
  o->id = O_FLOYD;
  o->description = "multiple purpose robot";
  o->long_description =
      "Only one robot, about four feet high, looks even remotely close to being\n"
      "in working order.";
  o->synonyms[0] = "floyd";
  o->synonyms[1] = "robot";
  o->synonyms[2] = "companion";
  o->adjectives[0] = "multiple";
  o->adjectives[1] = "purpose";
  o->adjectives[2] = "dim-witted";
  o->flags = F_CONTBIT | F_SEARCHBIT | F_TRANSBIT | F_OPENBIT | F_LIGHTBIT;
  o->capacity = 5;
  o->action = floyd_f;
  obj_move(O_FLOYD, R_ROBOT_SHOP);
  // R_ELEVATOR_LOBBY
  r = &objects[R_ELEVATOR_LOBBY];
  r->id = R_ELEVATOR_LOBBY;
  r->description = "Elevator Lobby";
  r->synonyms[0] = "lobby";
  r->adjectives[0] = "elevator";
  r->long_description =
      "This is a wide, brightly lit lobby. A blue metal door to the north is "
      "closed\n"
      "and a larger red metal door to the south is closed. Beside the blue "
      "door is a\n"
      "blue button, and beside the red door is a red button. A corridor leads "
      "west.\n"
      "To the east is a small room about the size of a telephone booth.";
  r->flags = F_RLANDBIT | F_FLOYDBIT | F_ONBIT;
  r->north = R_UPPER_ELEVATOR;
  r->south = R_LOWER_ELEVATOR;
  r->west = R_CORRIDOR_JUNCTION;
  r->east = R_BOOTH_2;
  r->in = R_BOOTH_2;
  r->globals[0] = O_UPPER_ELEVATOR_DOOR;
  r->globals[1] = O_LOWER_ELEVATOR_DOOR;
  r->globals[2] = O_NEAR_BOOTH_PSEUDO;
  r->action = elevator_lobby_f;

  // O_UPPER_ELEVATOR_DOOR
  o = &objects[O_UPPER_ELEVATOR_DOOR];
  o->id = O_UPPER_ELEVATOR_DOOR;
  o->description = "blue door";
  o->synonyms[0] = "door";
  o->adjectives[0] = "blue";
  o->adjectives[1] = "upper";
  o->adjectives[2] = "elevator";
  o->flags = F_NDESCBIT | F_DOORBIT;
  o->action = upper_elevator_door_f;
  obj_move(O_UPPER_ELEVATOR_DOOR, OBJ_LOCAL_GLOBALS);

  // O_LOWER_ELEVATOR_DOOR
  o = &objects[O_LOWER_ELEVATOR_DOOR];
  o->id = O_LOWER_ELEVATOR_DOOR;
  o->description = "red door";
  o->synonyms[0] = "door";
  o->adjectives[0] = "red";
  o->adjectives[1] = "lower";
  o->adjectives[2] = "elevator";
  o->adjectives[3] = "metal";
  o->flags = F_NDESCBIT | F_DOORBIT;
  o->action = lower_elevator_door_f;
  obj_move(O_LOWER_ELEVATOR_DOOR, OBJ_LOCAL_GLOBALS);

  // O_BLUE_ELEVATOR_BUTTON
  o = &objects[O_BLUE_ELEVATOR_BUTTON];
  o->id = O_BLUE_ELEVATOR_BUTTON;
  o->description = "blue button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "blue";
  o->adjectives[1] = "north";
  o->flags = F_NDESCBIT;
  o->action = blue_elevator_button_f;
  obj_move(O_BLUE_ELEVATOR_BUTTON, R_ELEVATOR_LOBBY);

  // O_RED_ELEVATOR_BUTTON
  o = &objects[O_RED_ELEVATOR_BUTTON];
  o->id = O_RED_ELEVATOR_BUTTON;
  o->description = "red button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "red";
  o->adjectives[1] = "south";
  o->flags = F_NDESCBIT;
  o->action = red_elevator_button_f;
  obj_move(O_RED_ELEVATOR_BUTTON, R_ELEVATOR_LOBBY);

  // R_UPPER_ELEVATOR
  r = &objects[R_UPPER_ELEVATOR];
  r->id = R_UPPER_ELEVATOR;
  r->description = "Upper Elevator";
  r->synonyms[0] = "elevator";
  r->adjectives[0] = "upper";
  r->long_description =
      "You have entered a tiny room with a sliding door to the south which is\n"
      "closed. A control panel contains an Up button, a Down button, and a\n"
      "narrow slot.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->south = R_ELEVATOR_LOBBY;
  r->out = R_ELEVATOR_LOBBY;
  r->globals[0] = O_CONTROLS;
  r->globals[1] = O_SLOT;
  r->globals[2] = O_ELEVATOR_BUTTON;
  r->globals[3] = O_UPPER_ELEVATOR_DOOR;
  r->action = upper_elevator_f;

  // R_LOWER_ELEVATOR
  r = &objects[R_LOWER_ELEVATOR];
  r->id = R_LOWER_ELEVATOR;
  r->description = "Lower Elevator";
  r->synonyms[0] = "elevator";
  r->adjectives[0] = "lower";
  r->long_description =
      "This is a medium-sized room with a door to the north which is\n"
      "closed. A control panel contains an Up button, a Down button, and\n"
      "a narrow slot.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->north = R_ELEVATOR_LOBBY;
  r->out = R_ELEVATOR_LOBBY;
  r->globals[0] = O_CONTROLS;
  r->globals[1] = O_SLOT;
  r->globals[2] = O_ELEVATOR_BUTTON;
  r->globals[3] = O_LOWER_ELEVATOR_DOOR;
  r->action = lower_elevator_f;

  // O_ELEVATOR_BUTTON
  o = &objects[O_ELEVATOR_BUTTON];
  o->id = O_ELEVATOR_BUTTON;
  o->description = "button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "up";
  o->adjectives[1] = "down";
  o->adjectives[2] = "elevator";
  o->flags = F_NDESCBIT;
  o->action = elevator_button_f;
  obj_move(O_ELEVATOR_BUTTON, OBJ_LOCAL_GLOBALS);

  // R_BOOTH_2
  r = &objects[R_BOOTH_2];
  r->id = R_BOOTH_2;
  r->description = "Booth 2";
  r->long_description = "This is a tiny room with a large \"2\" painted on the "
                        "wall. A panel contains\n"
                        "a slot about ten centimeters wide, a brown button "
                        "labelled \"1\" and a tan\n"
                        "button labelled \"3.\"";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->west = R_ELEVATOR_LOBBY;
  r->out = R_ELEVATOR_LOBBY;
  // Globals: CONTROLS, SLOT, TELEPORTATION-BUTTON-1, TELEPORTATION-BUTTON-3

  // R_TOWER_CORE
  r = &objects[R_TOWER_CORE];
  r->id = R_TOWER_CORE;
  r->description = "Tower Core";
  r->synonyms[0] = "core";
  r->synonyms[1] = "tower";
  r->adjectives[0] = "tower";
  r->long_description = "This is a small, circular room. A sliding door leads "
                        "north, and a spiral\n"
                        "staircase heads upwards. Other exits lie to the "
                        "northeast and southwest.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->north = R_UPPER_ELEVATOR;
  r->up = R_HELIPAD;
  r->sw = R_OBSERVATION_DECK;
  r->ne = R_COMM_ROOM;
  r->value = 4;
  // Globals: STAIRS, UPPER-ELEVATOR-DOOR
  r->globals[0] = O_STAIRS;
  r->globals[1] = O_UPPER_ELEVATOR_DOOR;

  // R_HELIPAD
  r = &objects[R_HELIPAD];
  r->id = R_HELIPAD;
  r->description = "Helipad";
  r->synonyms[0] = "helipad";
  r->long_description =
      "You are at the center of a wide, flat area atop the tower. A fence\n"
      "prevents you from approaching the perimeter, so your view is limited to\n"
      "cloud-filled sky. A large vehicle, severely weathered and topped with\n"
      "rotor blades, lies nearby. A spiral staircase leads down into the tower.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->down = R_TOWER_CORE;
  r->in = R_HELICOPTER;
  r->globals[0] = O_STAIRS;
  r->globals[1] = O_HELICOPTER_OBJECT;
  r->globals[2] = O_FENCE_PSEUDO;

  // Pseudo: FENCE
  o = &objects[O_FENCE_PSEUDO];
  o->id = O_FENCE_PSEUDO;
  o->description = "fence";
  o->synonyms[0] = "fence";
  o->flags = F_NDESCBIT;
  o->action = fence_pseudo_action;
  obj_move(O_FENCE_PSEUDO, OBJ_LOCAL_GLOBALS);

  // O_HELICOPTER_OBJECT
  o = &objects[O_HELICOPTER_OBJECT];
  o->id = O_HELICOPTER_OBJECT;
  o->description = "large vehicle";
  o->synonyms[0] = "vehicle";
  o->synonyms[1] = "helicopter";
  o->adjectives[0] = "large";
  o->flags = F_VEHBIT | F_NDESCBIT;
  o->action = helicopter_object_f;
  obj_move(O_HELICOPTER_OBJECT, OBJ_LOCAL_GLOBALS);

  // R_HELICOPTER
  r = &objects[R_HELICOPTER];
  r->id = R_HELICOPTER;
  r->description = "Helicopter";
  r->synonyms[0] = "helicopter";
  r->synonyms[1] = "vehicle";
  r->long_description =
      "This is a large vehicle with a lot of cargo space. A complex control panel\n"
      "is closed and locked. Everything is covered with a thick layer of rust.\n"
      "Through the windows of the vehicle you can see a wide Helipad, and beyond\n"
      "that, endless ocean far below. Several doors lead out to the Helipad.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->out = R_HELIPAD;
  r->globals[0] = O_OCEAN;
  r->globals[1] = O_CONTROLS;
  r->globals[2] = O_WINDOW;
  r->globals[3] = O_HELICOPTER_OBJECT;
  r->globals[4] = O_LOCK_PSEUDO;

  // Pseudo: LOCK
  o = &objects[O_LOCK_PSEUDO];
  o->id = O_LOCK_PSEUDO;
  o->description = "lock";
  o->synonyms[0] = "lock";
  o->flags = F_NDESCBIT;
  o->action = lock_pseudo_action;
  obj_move(O_LOCK_PSEUDO, OBJ_LOCAL_GLOBALS);

  // R_COMM_ROOM
  r = &objects[R_COMM_ROOM];
  r->id = R_COMM_ROOM;
  r->description = "Comm Room";
  r->synonyms[0] = "room";
  r->adjectives[0] = "comm";
  r->long_description =
      "This is a small room with no windows. The sole exit is southwest. Two wide\n"
      "consoles fill either end of the room; thick cables lead up into the ceiling.\n\n"
      "The console on the left side of the room is labelled \"Reeseev Staashun.\" A\n"
      "bright red light, labelled \"Tranzmishun Reeseevd\", is blinking rapidly.\n"
      "Next to the light is a glowing button marked \"Mesij Plaabak.\"\n\n"
      "The console on the right side of the room is labelled \"Send Staashun.\" A\n"
      "screen on the console displays a message. Next to the screen is a flashing\n"
      "sign which says \"Malfunkshun in Sendeeng Kuulint Sistum.\" Next to this console is an enunciator\n"
      "whose lights are all dark. On the console next to the enunciator panel is a funnel-shaped hole\n"
      "labelled \"Kuulint Sistum Manyuuwul Oovuriid.\"";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->sw = R_TOWER_CORE;
  r->globals[0] = O_LIGHTS;
  r->globals[1] = O_CABLES_PSEUDO;
  r->globals[2] = O_ENUNCIATOR_PSEUDO;
  r->action = comm_room_f;

  // Pseudo: CABLES
  o = &objects[O_CABLES_PSEUDO];
  o->id = O_CABLES_PSEUDO;
  o->description = "cables";
  o->synonyms[0] = "cables";
  o->synonyms[1] = "cable";
  o->flags = F_NDESCBIT;
  o->action = cables_pseudo_action;
  obj_move(O_CABLES_PSEUDO, OBJ_LOCAL_GLOBALS);

  // Pseudo: ENUNCIATOR
  o = &objects[O_ENUNCIATOR_PSEUDO];
  o->id = O_ENUNCIATOR_PSEUDO;
  o->description = "enunciator panel";
  o->synonyms[0] = "panel";
  o->synonyms[1] = "enunciator";
  o->flags = F_NDESCBIT;
  o->action = enunciator_pseudo_action;
  obj_move(O_ENUNCIATOR_PSEUDO, OBJ_LOCAL_GLOBALS);

  // O_RECEIVE_CONSOLE
  o = &objects[O_RECEIVE_CONSOLE];
  o->id = O_RECEIVE_CONSOLE;
  o->description = "communications receive console";
  o->synonyms[0] = "console";
  o->synonyms[1] = "control";
  o->adjectives[0] = "communications";
  o->adjectives[1] = "receive";
  o->adjectives[2] = "left";
  o->adjectives[3] = "first";
  o->flags = F_NDESCBIT;
  obj_move(O_RECEIVE_CONSOLE, R_COMM_ROOM);

  // O_PLAYBACK_BUTTON
  o = &objects[O_PLAYBACK_BUTTON];
  o->id = O_PLAYBACK_BUTTON;
  o->description = "glowing button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "glowing";
  o->adjectives[1] = "playback";
  o->flags = F_NDESCBIT;
  o->action = playback_button_f;
  obj_move(O_PLAYBACK_BUTTON, R_COMM_ROOM);

  // O_SEND_CONSOLE
  o = &objects[O_SEND_CONSOLE];
  o->id = O_SEND_CONSOLE;
  o->description = "communication send console";
  o->synonyms[0] = "console";
  o->synonyms[1] = "control";
  o->adjectives[0] = "communications";
  o->adjectives[1] = "send";
  o->adjectives[2] = "right";
  o->adjectives[3] = "second";
  o->flags = F_NDESCBIT;
  obj_move(O_SEND_CONSOLE, R_COMM_ROOM);

  // O_COMM_SCREEN
  o = &objects[O_COMM_SCREEN];
  o->id = O_COMM_SCREEN;
  o->description = "screen";
  o->synonyms[0] = "screen";
  o->synonyms[1] = "communications";
  o->synonyms[2] = "message";
  o->flags = F_NDESCBIT | F_READBIT;
  o->text = "\"Tuu enee ship uv xe Sekund Galaktik Yuunyun: Planitwiid plaag has struk\n"
            "entiir popyuulaashun. Tiim iz kritikul. Eemurjensee asistins reekwestid.\n"
            "<reepeet mesij>\"";
  obj_move(O_COMM_SCREEN, R_COMM_ROOM);

  // O_FUNNEL_HOLE
  o = &objects[O_FUNNEL_HOLE];
  o->id = O_FUNNEL_HOLE;
  o->description = "funnel-shaped hole";
  o->synonyms[0] = "hole";
  o->adjectives[0] = "funnel";
  o->adjectives[1] = "shaped";
  o->flags = F_NDESCBIT;
  obj_move(O_FUNNEL_HOLE, R_COMM_ROOM);

  // O_CHEMICAL_FLUID
  o = &objects[O_CHEMICAL_FLUID];
  o->id = O_CHEMICAL_FLUID;
  o->description = "quantity of chemical fluid";
  o->synonyms[0] = "quantity";
  o->synonyms[1] = "liquid";
  o->synonyms[2] = "fluid";
  o->synonyms[3] = "chemical";
  o->adjectives[0] = "milky";
  o->adjectives[1] = "white";
  o->adjectives[2] = "chemical";
  o->action = chemical_fluid_f;

  // R_OBSERVATION_DECK
  r = &objects[R_OBSERVATION_DECK];
  r->id = R_OBSERVATION_DECK;
  r->description = "Observation Deck";
  r->synonyms[0] = "deck";
  r->adjectives[0] = "observation";
  r->long_description =
      "This is a balcony girdling the tower. The view is breathtaking; the tower\n"
      "must be half a kilometer tall. From here it is clear that you are on an\n"
      "island. The dormitory section of the complex is visible on the other side\n"
      "of the island, and the rest of the complex sprawls out directly below. In\n"
      "the distance, about 20 kilometers to the east, you can spot another island\n"
      "similar to this one. The only exit is a doorway leading northeast.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->ne = R_TOWER_CORE;
  r->globals[0] = O_OCEAN;

  // R_WAITING_AREA
  r = &objects[R_WAITING_AREA];
  r->id = R_WAITING_AREA;
  r->description = "Waiting Area";
  r->synonyms[0] = "area";
  r->adjectives[0] = "waiting";
  r->long_description =
      "This is a concrete platform sparsely furnished with benches. The platform\n"
      "continues to the east, and to the south is a metal door.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->east = R_KALAMONTEE_PLATFORM;
  r->south = R_LOWER_ELEVATOR;
  r->in = R_LOWER_ELEVATOR;
  r->globals[0] = O_LOWER_ELEVATOR_DOOR;
  r->globals[1] = O_BENCH_PSEUDO;

  // R_KALAMONTEE_PLATFORM
  r = &objects[R_KALAMONTEE_PLATFORM];
  r->id = R_KALAMONTEE_PLATFORM;
  r->description = "Kalamontee Platform";
  r->synonyms[0] = "platform";
  r->adjectives[0] = "kalamontee";
  r->long_description =
      "This is a wide, flat strip of concrete which continues westward.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->value = 4;
  r->west = R_WAITING_AREA;
  r->globals[0] = O_GLOBAL_SHUTTLE;
  r->action = kalamontee_platform_f;

}
