#include <stdio.h>
#include "planetfall.h"
#include "complexone.h"
#include "feinstein.h"

void init_complexone() {
    ZObject* o;
    ZObject* r;

    // === GLOBALS ===
    
    // CLIFF
    o = &objects[O_CLIFF];
    o->id = O_CLIFF;
    o->description = "cliff";
    o->synonyms[0] = "cliff"; o->synonyms[1] = "wall";
    o->flags = F_NDESCBIT;
    obj_move(O_CLIFF, OBJ_GLOBAL_OBJECTS);

    // OCEAN
    o = &objects[O_OCEAN];
    o->id = O_OCEAN;
    o->description = "ocean";
    o->synonyms[0] = "ocean"; o->synonyms[1] = "sea"; o->synonyms[2] = "water";
    o->flags = F_NDESCBIT;
    obj_move(O_OCEAN, OBJ_GLOBAL_OBJECTS);

    // STAIRS
    o = &objects[O_STAIRS];
    o->id = O_STAIRS;
    o->description = "stairs";
    o->synonyms[0] = "stairs"; o->synonyms[1] = "stairway"; o->synonyms[2] = "steps";
    o->flags = F_NDESCBIT;
    obj_move(O_STAIRS, OBJ_GLOBAL_OBJECTS);

    // WINDOW
    o = &objects[O_WINDOW];
    o->id = O_WINDOW;
    o->description = "window";
    o->synonyms[0] = "window";
    o->flags = F_NDESCBIT;
    obj_move(O_WINDOW, OBJ_GLOBAL_OBJECTS);

    // === ROOMS ===

    // R_UNDERWATER
    r = &objects[R_UNDERWATER];
    r->id = R_UNDERWATER;
    r->description = "Underwater";
    r->long_description = "You are momentarily disoriented as you enter the turbulent waters.\n"
                          "Currents buffet you against the sharp rocks of an underwater\n"
                          "cliff. A dim light filters down from above.";
    r->flags = F_ONBIT; // F_RWATERBIT?
    r->up = R_CRAG;
    // Down/West/North/South -> Underwater (Loop)
    r->down = R_UNDERWATER;
    r->west = R_UNDERWATER;
    r->north = R_UNDERWATER;
    r->south = R_UNDERWATER;
    
    // R_CRAG (Update)
    r = &objects[R_CRAG];
    r->long_description = "You have reached a cleft in the cliff wall where the island rises from the\n"
                          "water. The edge of the cleft displays recently exposed rock where it collapsed\n"
                          "under the weight of the escape pod. About two meters below, turbulent waters\n"
                          "swirl against sharp rocks. A small structure clings to the face of the cliff\n"
                          "about eight meters above you. Even an out-of-shape Ensign Seventh Class could\n"
                          "probably climb up to it.";
    r->down = R_UNDERWATER;
    // Up is Balcony (already set in feinstein.c)
    // In is Escape Pod (already set)
    
    // Pseudo Objects for Crag
    o = &objects[O_STRUCTURE_PSEUDO];
    o->id = O_STRUCTURE_PSEUDO;
    o->description = "structure";
    o->synonyms[0] = "structure";
    o->flags = F_NDESCBIT;
    obj_move(O_STRUCTURE_PSEUDO, R_CRAG);
    
    o = &objects[O_CLEFT_PSEUDO];
    o->id = O_CLEFT_PSEUDO;
    o->description = "cleft";
    o->synonyms[0] = "cleft";
    o->flags = F_NDESCBIT;
    obj_move(O_CLEFT_PSEUDO, R_CRAG);

    // R_BALCONY (Update)
    r = &objects[R_BALCONY];
    // Description logic handled in look routine, here default long desc
    r->long_description = "This is an octagonal room, half carved into and half built out from the cliff\n"
                          "wall. Through the shattered windows which ring the outer wall you can see\n"
                          "ocean to the horizon. A weathered metal plaque with barely readable lettering\n"
                          "rests below the windows. The language seems to be a corrupt form of\n"
                          "Galalingua. A steep stairway, roughly cut into the face of the cliff, leads\n"
                          "upward.";
    // Dynamic Down: Need logic. For now default to Crag (Day 1).
    r->down = R_CRAG; 
    
    // Pseudo Objects for Balcony
    o = &objects[O_PLAQUE_PSEUDO];
    o->id = O_PLAQUE_PSEUDO;
    o->description = "metal plaque";
    o->synonyms[0] = "plaque";
    o->flags = F_NDESCBIT | F_READBIT;
    o->text = "\nSEENIK VISTA\n\n"
              "Xis stuneeng vuu uf xee Kalamontee Valee kuvurz oovur fortee skwaar miilz\n"
              "uf xat faamus tuurist spot. Xee larj bildeeng at xee bend in xee Gulmaan Rivur\n"
              "iz xee formur pravincul kapitul bildeeng.";
    obj_move(O_PLAQUE_PSEUDO, R_BALCONY);

    // R_WINDING_STAIR (Update)
    r = &objects[R_WINDING_STAIR];
    r->long_description = "The middle of a long, steep stairway carved into the face of a cliff.";
    // Up to Courtyard (already set)
    // Down to Balcony (already set)

    // R_COURTYARD (Update)
    r = &objects[R_COURTYARD];
    r->long_description = "You are in the courtyard of an ancient stone edifice, vaguely reminiscent of\n"
                          "the castles you saw during your leave on Ramos Two. It has decayed to\n"
                          "the point where it can probably be termed a ruin. Openings lead north and west,\n"
                          "and a stairway downward is visible to the south.";
    r->south = R_WINDING_STAIR; // Also Down
    r->down = R_WINDING_STAIR;
    r->west = R_WEST_WING;
    r->north = R_PLAIN_HALL;
    
    // Pseudo Objects for Courtyard
    o = &objects[O_CASTLE_PSEUDO];
    o->id = O_CASTLE_PSEUDO;
    o->description = "castle";
    o->long_description = "The castle is ancient and crumbling.";
    o->synonyms[0] = "castle"; o->synonyms[1] = "edifice"; o->synonyms[2] = "ruin";
    o->flags = F_NDESCBIT;
    obj_move(O_CASTLE_PSEUDO, R_COURTYARD);
    
    o = &objects[O_RUBBLE_PSEUDO];
    o->id = O_RUBBLE_PSEUDO;
    o->description = "rubble";
    o->long_description = "Don't be silly.";
    o->synonyms[0] = "rubble";
    o->flags = F_NDESCBIT;
    obj_move(O_RUBBLE_PSEUDO, R_COURTYARD);

    // R_WEST_WING
    r = &objects[R_WEST_WING];
    r->id = R_WEST_WING;
    r->description = "West Wing";
    r->long_description = "This was once the west wing of the castle, but the walls are now mostly\n"
                          "rubble, allowing a view of the cliff and ocean below. Rubble blocks all exits\n"
                          "save one, eastward to the courtyard.";
    r->flags = F_RLANDBIT | F_ONBIT;
    r->east = R_COURTYARD;
    // Down leads to refusal message "Certain death." handled in main.c
    r->down = R_CERTAIN_DEATH_MSG; 
    
    // Add Pseudos to West Wing (Castle, Rubble) - Need duplicate objects or reuse?
    // Objects can only be in one place. I need separate objects or a "Global" list for room.
    // My engine supports `globals[10]` on Room.
    // I can put `O_CASTLE_PSEUDO` in `OBJ_LOCAL_GLOBALS` (or just leave it) and add to `r->globals`.
    // But `O_CASTLE_PSEUDO` is defined as "Pseudo", usually local.
    // ZIL defines separate pseudos for each room sometimes or reuses.
    // Here `COURTYARD` and `WEST-WING` share "CASTLE" and "RUBBLE".
    // I'll make O_CASTLE_PSEUDO a global object visible in these rooms via `globals` list?
    // Or I'll just move it to `OBJ_LOCAL_GLOBALS` and add it to `r->globals` for both rooms.
    // Let's do that.
    
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
    r->long_description = "This is a featureless hall leading north and south. Although the hallway is\n"
                          "old and dusty, the construction is of a much more modern style than the\n"
                          "castle to the south. A similar hall branches off to the northeast.";
    r->flags = F_RLANDBIT | F_ONBIT;
    r->south = R_COURTYARD;
    // r->north = R_REC_AREA;
    // r->ne = R_REC_CORRIDOR;
}
