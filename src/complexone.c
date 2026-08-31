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
  r->flags = F_ONBIT; // F_RWATERBIT?
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
  r->flags = F_RLANDBIT | F_ONBIT;
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
  r->long_description =
      "This section of hallway seems to have suffered some minor structural\n"
      "damage. The walls are cracked, and a jagged crevice crosses the floor.\n"
      "An opening leads east and the corridor heads north and south.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->south = R_CORRIDOR_JUNCTION;
  r->north = R_ADMIN_CORRIDOR;
  r->east = R_SANFAC_E;

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
  obj_move(O_CREVICE, R_ADMIN_CORRIDOR_S);

  // O_KEY
  o = &objects[O_KEY];
  o->id = O_KEY;
  o->description = "key";
  o->synonyms[0] = "key";
  o->adjectives[0] = "steel";
  o->adjectives[1] = "shiny";
  o->flags =
      F_TAKEBIT | F_TOOLBIT | F_INVISIBLE | F_TRYTAKEBIT; // Invisible initially
  o->size = 3;
  obj_move(O_KEY, R_ADMIN_CORRIDOR_S);
  // R_MACHINE_SHOP
  r = &objects[R_MACHINE_SHOP];
  r->id = R_MACHINE_SHOP;
  r->description = "Machine Shop";
  r->long_description =
      "This room is probably some sort of machine shop filled with a variety\n"
      "of unusual machines. Doorways lead north, east, and west.\n"
      "\n"
      "Standing against the rear wall is a large dispensing machine with a\n"
      "spout. The dispenser is lined with brightly-colored buttons. The first "
      "four\n"
      "buttons, labelled \"KUULINTS 1 - 4\", are colored red, blue, green, "
      "and\n"
      "yellow. The next three buttons, labelled \"KATALISTS 1 - 3\", are "
      "colored\n"
      "gray, brown, and black. The last two buttons are both white. One of\n"
      "these is square and says \"BAAS.\" The other white button is round and\n"
      "says \"ASID.\"";
  // Logic for displaying what is under spout is dynamic in look routine
  r->flags = F_RLANDBIT | F_FLOYDBIT | F_ONBIT;
  r->north = R_MECH_CORRIDOR_S;
  r->east = R_ROBOT_SHOP;
  r->west = R_TOOL_ROOM;
  // Pseudo: SPOUT
  r->globals[0] = O_CHEM_SPOUT_PSEUDO;

  // O_CHEMICAL_DISPENSER
  o = &objects[O_CHEMICAL_DISPENSER];
  o->id = O_CHEMICAL_DISPENSER;
  o->description = "chemical dispenser";
  o->synonyms[0] = "dispenser";
  o->synonyms[1] = "machine";
  o->adjectives[0] = "chemical";
  o->adjectives[1] = "large";
  o->flags = F_MUNGBIT | F_NDESCBIT;
  obj_move(O_CHEMICAL_DISPENSER, R_MACHINE_SHOP);

  // Buttons in Machine Shop
  o = &objects[O_RED_BUTTON];
  o->id = O_RED_BUTTON;
  o->description = "red button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "red";
  o->flags = F_NDESCBIT;
  obj_move(O_RED_BUTTON, R_MACHINE_SHOP);

  o = &objects[O_BLUE_BUTTON];
  o->id = O_BLUE_BUTTON;
  o->description = "blue button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "blue";
  o->flags = F_NDESCBIT;
  obj_move(O_BLUE_BUTTON, R_MACHINE_SHOP);

  o = &objects[O_GREEN_BUTTON];
  o->id = O_GREEN_BUTTON;
  o->description = "green button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "green";
  o->flags = F_NDESCBIT;
  obj_move(O_GREEN_BUTTON, R_MACHINE_SHOP);

  o = &objects[O_YELLOW_BUTTON];
  o->id = O_YELLOW_BUTTON;
  o->description = "yellow button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "yellow";
  o->flags = F_NDESCBIT;
  obj_move(O_YELLOW_BUTTON, R_MACHINE_SHOP);

  o = &objects[O_GRAY_BUTTON];
  o->id = O_GRAY_BUTTON;
  o->description = "gray button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "gray";
  o->flags = F_NDESCBIT;
  obj_move(O_GRAY_BUTTON, R_MACHINE_SHOP);

  o = &objects[O_BROWN_BUTTON];
  o->id = O_BROWN_BUTTON;
  o->description = "brown button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "brown";
  o->flags = F_NDESCBIT;
  obj_move(O_BROWN_BUTTON, R_MACHINE_SHOP);

  o = &objects[O_BLACK_BUTTON];
  o->id = O_BLACK_BUTTON;
  o->description = "black button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "black";
  o->flags = F_NDESCBIT;
  obj_move(O_BLACK_BUTTON, R_MACHINE_SHOP);

  o = &objects[O_ROUND_WHITE_BUTTON];
  o->id = O_ROUND_WHITE_BUTTON;
  o->description = "round white button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "white";
  o->adjectives[1] = "round";
  o->flags = F_NDESCBIT;
  obj_move(O_ROUND_WHITE_BUTTON, R_MACHINE_SHOP);

  o = &objects[O_SQUARE_WHITE_BUTTON];
  o->id = O_SQUARE_WHITE_BUTTON;
  o->description = "square white button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "white";
  o->adjectives[1] = "square";
  o->flags = F_NDESCBIT;
  obj_move(O_SQUARE_WHITE_BUTTON, R_MACHINE_SHOP);

  // R_ROBOT_SHOP
  r = &objects[R_ROBOT_SHOP];
  r->id = R_ROBOT_SHOP;
  r->description = "Robot Shop";
  r->long_description =
      "This room, with exits west and northwest, is filled with robot-like\n"
      "devices of every conceivable description, all in various states of\n"
      "disassembly.";
  r->flags = F_RLANDBIT | F_FLOYDBIT | F_ONBIT;
  r->west = R_MACHINE_SHOP;
  r->nw = R_MECH_CORRIDOR_S;
  r->globals[0] = O_DEVICES_PSEUDO;

  // O_FLOYD
  o = &objects[O_FLOYD];
  o->id = O_FLOYD;
  o->description = "multiple purpose robot";
  o->long_description = "Only one robot, about four feet high, looks even "
                        "remotely close to being\n"
                        "in working order.";
  o->synonyms[0] = "floyd";
  o->synonyms[1] = "robot";
  o->synonyms[2] = "companion";
  // Adjectives: multiple purpose, dim-witted
  o->flags = F_CONTBIT | F_SEARCHBIT | F_TRANSBIT | F_OPENBIT | F_LIGHTBIT;
  o->capacity = 5;
  obj_move(O_FLOYD, R_ROBOT_SHOP);
  // R_ELEVATOR_LOBBY
  r = &objects[R_ELEVATOR_LOBBY];
  r->id = R_ELEVATOR_LOBBY;
  r->description = "Elevator Lobby";
  r->long_description =
      "This is a wide, brightly lit lobby. A blue metal door to the north is "
      "closed\n"
      "and a larger red metal door to the south is closed. Beside the blue "
      "door is a\n"
      "blue button, and beside the red door is a red button. A corridor leads "
      "west.\n"
      "To the east is a small room about the size of a telephone booth.";
  r->flags = F_RLANDBIT | F_FLOYDBIT | F_ONBIT;
  r->north = R_UPPER_ELEVATOR; // Guarded by UPPER-ELEVATOR-DOOR
  r->south = R_LOWER_ELEVATOR; // Guarded by LOWER-ELEVATOR-DOOR
  r->west = R_CORRIDOR_JUNCTION;
  r->east = R_BOOTH_2;
  r->in = R_BOOTH_2;
  // Globals: UPPER-ELEVATOR-DOOR, LOWER-ELEVATOR-DOOR
  r->globals[0] = O_UPPER_ELEVATOR_DOOR;
  r->globals[1] = O_LOWER_ELEVATOR_DOOR;
  // Pseudo: BOOTH

  // O_UPPER_ELEVATOR_DOOR
  o = &objects[O_UPPER_ELEVATOR_DOOR];
  o->id = O_UPPER_ELEVATOR_DOOR;
  o->description = "blue door";
  o->synonyms[0] = "door";
  o->adjectives[0] = "blue";
  o->adjectives[1] = "upper";
  o->adjectives[2] = "elevator";
  o->flags = F_NDESCBIT | F_DOORBIT; // Initially closed
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
  o->flags = F_NDESCBIT | F_DOORBIT; // Initially closed
  obj_move(O_LOWER_ELEVATOR_DOOR, OBJ_LOCAL_GLOBALS);

  // O_BLUE_ELEVATOR_BUTTON
  o = &objects[O_BLUE_ELEVATOR_BUTTON];
  o->id = O_BLUE_ELEVATOR_BUTTON;
  o->description = "blue button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "blue";
  o->adjectives[1] = "north";
  o->flags = F_NDESCBIT;
  obj_move(O_BLUE_ELEVATOR_BUTTON, R_ELEVATOR_LOBBY);

  // O_RED_ELEVATOR_BUTTON
  o = &objects[O_RED_ELEVATOR_BUTTON];
  o->id = O_RED_ELEVATOR_BUTTON;
  o->description = "red button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "red";
  o->adjectives[1] = "south";
  o->flags = F_NDESCBIT;
  obj_move(O_RED_ELEVATOR_BUTTON, R_ELEVATOR_LOBBY);

  // R_UPPER_ELEVATOR
  r = &objects[R_UPPER_ELEVATOR];
  r->id = R_UPPER_ELEVATOR;
  r->description = "Upper Elevator";
  r->long_description =
      "You have entered a tiny room with a sliding door to the south which is\n"
      "closed. A control panel contains an Up button, a Down button, and a\n"
      "narrow slot.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->south =
      R_ELEVATOR_LOBBY; // Guarded (Exit is handled by ELEVATOR-EXIT-F usually)
  r->out = R_ELEVATOR_LOBBY;
  // Globals: LIGHTS, CONTROLS, SLOT, ELEVATOR-BUTTON, UPPER-ELEVATOR-DOOR
  r->globals[0] = O_CONTROLS;
  r->globals[1] = O_SLOT;
  r->globals[2] = O_ELEVATOR_BUTTON;
  r->globals[3] = O_UPPER_ELEVATOR_DOOR;

  // R_LOWER_ELEVATOR
  r = &objects[R_LOWER_ELEVATOR];
  r->id = R_LOWER_ELEVATOR;
  r->description = "Lower Elevator";
  r->long_description =
      "This is a medium-sized room with a door to the north which is\n"
      "closed. A control panel contains an Up button, a Down button, and\n"
      "a narrow slot.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->north = R_ELEVATOR_LOBBY;
  r->out = R_ELEVATOR_LOBBY;
  // Globals: LIGHTS, CONTROLS, SLOT, ELEVATOR-BUTTON, LOWER-ELEVATOR-DOOR
  r->globals[0] = O_CONTROLS;
  r->globals[1] = O_SLOT;
  r->globals[2] = O_ELEVATOR_BUTTON;
  r->globals[3] = O_LOWER_ELEVATOR_DOOR;

  // O_ELEVATOR_BUTTON
  o = &objects[O_ELEVATOR_BUTTON];
  o->id = O_ELEVATOR_BUTTON;
  o->description = "button";
  o->synonyms[0] = "button";
  o->flags = F_NDESCBIT;
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
  r->long_description =
      "You are at the center of a wide, flat area atop the tower. A fence\n"
      "prevents you from approaching the perimeter, so your view is limited "
      "to\n"
      "cloud-filled sky. A large vehicle, severely weathered and topped with "
      "rotor\n"
      "blades, lies nearby. A spiral staircase leads down into the tower.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->down = R_TOWER_CORE;
  r->in = R_HELICOPTER;
  // Globals: STAIRS, HELICOPTER-OBJECT
  r->globals[0] = O_STAIRS;
  r->globals[1] = O_HELICOPTER_OBJECT;
  // Pseudo: FENCE

  // O_HELICOPTER_OBJECT
  o = &objects[O_HELICOPTER_OBJECT];
  o->id = O_HELICOPTER_OBJECT;
  o->description = "large vehicle";
  o->synonyms[0] = "vehicle";
  o->synonyms[1] = "helicopter";
  o->adjectives[0] = "large";
  o->flags = F_VEHBIT | F_NDESCBIT;
  obj_move(O_HELICOPTER_OBJECT, OBJ_LOCAL_GLOBALS);

  // R_HELICOPTER
  r = &objects[R_HELICOPTER];
  r->id = R_HELICOPTER;
  r->description = "Helicopter";
  r->long_description =
      "This is a large vehicle with a lot of cargo space. A complex control "
      "panel\n"
      "is closed and locked. Everything is covered with a thick layer of "
      "rust.\n"
      "Through the windows of the vehicle you can see a wide Helipad, and "
      "beyond\n"
      "that, endless ocean far below. Several doors lead out to the Helipad.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->out = R_HELIPAD;
  // Globals: OCEAN, CONTROLS, WINDOW, HELICOPTER-OBJECT
  r->globals[0] = O_OCEAN;
  r->globals[1] = O_CONTROLS;
  r->globals[2] = O_WINDOW;
  r->globals[3] = O_HELICOPTER_OBJECT;
  // Pseudo: LOCK

  // R_COMM_ROOM
  r = &objects[R_COMM_ROOM];
  r->id = R_COMM_ROOM;
  r->description = "Comm Room";
  r->long_description =
      "This is a small room with no windows. The sole exit is southwest. Two "
      "wide\n"
      "consoles fill either end of the room; thick cables lead up into the "
      "ceiling.\n"
      "\n"
      "The console on the left side of the room is labelled \"Reeseev "
      "Staashun.\" A\n"
      "bright red light, labelled \"Tranzmishun Reeseevd\", is blinking "
      "rapidly.\n"
      "Next to the light is a glowing button marked \"Mesij Plaabak.\"\n"
      "\n"
      "The console on the right side of the room is labelled \"Send "
      "Staashun.\" "
      "A\n"
      "screen on the console displays a message. Next to the screen is a "
      "flashing\n"
      "sign which says \"Malfunkshun in Sendeeng Kuulint Sistum.\" Next to "
      "this console is an enunciator\n"
      "whose lights are all dark. On the console next to the enunciator panel "
      "is a funnel-shaped hole\n"
      "labelled \"Kuulint Sistum Manyuuwul Oovuriid.\"";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->sw = R_TOWER_CORE;
  // Globals: LIGHTS
  r->globals[0] = O_LIGHTS;
  // Pseudo: CABLES, ENUNCIATOR

  // O_RECEIVE_CONSOLE
  o = &objects[O_RECEIVE_CONSOLE];
  o->id = O_RECEIVE_CONSOLE;
  o->description = "communications receive console";
  o->synonyms[0] = "console";
  o->synonyms[1] = "control"; // "CONTRO"
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
  o->adjectives[1] = "playback"; // "PLAYBA"
  o->flags = F_NDESCBIT;
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
  o->text = "\"Tuu enee ship uv xe Sekund Galaktik Yuunyun: Planitwiid plaag "
            "haz struk\n"
            "entiir popyuulaashun. Tiim iz kritikul. Eemurjensee asistins "
            "reekwestid.\n"
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
  // Location undefined initially (~ NIL)

  // R_OBSERVATION_DECK
  r = &objects[R_OBSERVATION_DECK];
  r->id = R_OBSERVATION_DECK;
  r->description = "Observation Deck";
  r->long_description =
      "This is a balcony girdling the tower. The view is breathtaking; the "
      "tower\n"
      "must be half a kilometer tall. From here it is clear that you are on "
      "an\n"
      "island. The dormitory section of the complex is visible on the other "
      "side\n"
      "of the island, and the rest of the complex sprawls out directly below. "
      "In\n"
      "the distance, about 20 kilometers to the east, you can spot another "
      "island\n"
      "similar to this one. The only exit is a doorway leading northeast.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->ne = R_TOWER_CORE;
  r->globals[0] = O_OCEAN;

  // R_WAITING_AREA
  r = &objects[R_WAITING_AREA];
  r->id = R_WAITING_AREA;
  r->description = "Waiting Area";
  r->long_description =
      "This is a concrete platform sparsely furnished with benches. The "
      "platform\n"
      "continues to the east, and to the south is a metal door.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->east = R_KALAMONTEE_PLATFORM;
  r->south = R_LOWER_ELEVATOR; // Guarded by OTHER-ELEVATOR-ENTER-F
  r->in = R_LOWER_ELEVATOR;
  r->globals[0] = O_LOWER_ELEVATOR_DOOR;
  // Pseudo: BENCH

  // R_KALAMONTEE_PLATFORM
  r = &objects[R_KALAMONTEE_PLATFORM];
  r->id = R_KALAMONTEE_PLATFORM;
  r->description = "Kalamontee Platform";
  r->long_description = "This is a wide, flat strip of concrete which "
                        "continues westward."; // Dynamic
  r->flags = F_RLANDBIT | F_ONBIT;
  r->value = 4;
  r->west = R_WAITING_AREA;
  // South/North -> Shuttle Enter
  r->globals[0] = O_GLOBAL_SHUTTLE;
}
