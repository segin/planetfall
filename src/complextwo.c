#include "complextwo.h"
#include "complextwo_actions.h"
#include "ids.h"
#include "planetfall.h"
#include <stdio.h>

void init_complextwo() {
  ZObject *o;
  ZObject *r;

  // === ROOMS ===

  // R_LAWANDA_PLATFORM
  r = &objects[R_LAWANDA_PLATFORM];
  r->id = R_LAWANDA_PLATFORM;
  r->description = "Lawanda Platform";
  r->long_description = "This is a wide, flat strip of concrete. An open "
                        "shuttle car lies to the\n"
                        "north. A wide escalator, not currently operating, "
                        "beckons upward at the east\n"
                        "end of the platform. A faded sign reads \"Shutul "
                        "Platform -- Lawanda Staashun.\"";
  // Dynamic descriptions needed based on Alfie/Betty location
  r->flags = F_FLOYDBIT | F_RLANDBIT | F_ONBIT;
  r->east = R_ESCALATOR;
  r->up = R_ESCALATOR;
  r->north = R_KALAMONTEE_PLATFORM; // Shuttle logic needed. Direct link for
                                    // now? Or use action.
  r->south = R_KALAMONTEE_PLATFORM; // Shuttle logic needed.
  // Globals: GLOBAL_SHUTTLE, STAIRS
  r->globals[0] = O_GLOBAL_SHUTTLE;
  // Pseudo: ESCALATOR

  // R_ESCALATOR
  r = &objects[R_ESCALATOR];
  r->id = R_ESCALATOR;
  r->description = "Escalator";
  r->long_description = "You are in the middle of a long mechanical stairway. "
                        "It is not running,\n"
                        "and seems to be in disrepair.";
  r->flags = F_RLANDBIT | F_FLOYDBIT | F_ONBIT;
  r->up = R_FORK;
  r->east = R_FORK;
  r->down = R_LAWANDA_PLATFORM;
  r->west = R_LAWANDA_PLATFORM;
  // Pseudo: ESCALATOR

  // R_FORK
  r = &objects[R_FORK];
  r->id = R_FORK;
  r->description = "Fork";
  r->long_description = "This is a hallway which forks to the northeast and "
                        "southeast. To the west\n"
                        "is the top of a long escalator.";
  r->flags = F_FLOYDBIT | F_RLANDBIT | F_ONBIT;
  r->west = R_ESCALATOR;
  r->down = R_ESCALATOR;
  r->ne = R_SYSTEMS_CORRIDOR_WEST;
  // r->se = R_PROJECT_CORRIDOR_WEST; // Not yet defined
  // Pseudo: ESCALATOR

  // R_INFIRMARY
  r = &objects[R_INFIRMARY];
  r->id = R_INFIRMARY;
  r->description = "Infirmary";
  r->long_description = "You have entered a clean, well-lighted place. There "
                        "are a number of beds,\n"
                        "some complicated looking equipment, and many shelves "
                        "that are mostly bare.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->se = R_SYSTEMS_CORRIDOR_WEST;
  r->out = R_SYSTEMS_CORRIDOR_WEST;
  // Globals: BED, SHELVES
  // Pseudo: EQUIPMENT

  // R_REPAIR_ROOM
  r = &objects[R_REPAIR_ROOM];
  r->id = R_REPAIR_ROOM;
  r->description = "Repair Room";
  r->long_description = "You are in a dimly lit room, filled with strange "
                        "machines and wide storage\n"
                        "cabinets, all locked. To the south, a narrow stairway "
                        "leads upward. On the\n"
                        "north wall of the room is a very small doorway.";
  r->flags = F_RLANDBIT | F_FLOYDBIT | F_ONBIT;
  r->adjectives[0] = "repair";
  r->synonyms[0] = "room";
  r->up = R_SYSTEMS_CORRIDOR_WEST;
  r->south = R_SYSTEMS_CORRIDOR_WEST;
  // North is robot hole (too small)
  // Globals: STAIRS
  // Pseudo: CABINETS, EQUIPMENT

  // R_SYSTEMS_CORRIDOR_WEST
  r = &objects[R_SYSTEMS_CORRIDOR_WEST];
  r->id = R_SYSTEMS_CORRIDOR_WEST;
  r->description = "Systems Corridor West";
  r->long_description =
      "The corridor bends here, leading east and southwest. A doorway opens\n"
      "to the northwest, and a narrow stairway leads down to the north.";
  r->flags = F_ONBIT | F_RLANDBIT;
  r->nw = R_INFIRMARY;
  r->north = R_REPAIR_ROOM;
  r->down = R_REPAIR_ROOM;
  r->east = R_SYSTEMS_CORRIDOR;
  r->sw = R_FORK;

  // R_SYSTEMS_CORRIDOR
  r = &objects[R_SYSTEMS_CORRIDOR];
  r->id = R_SYSTEMS_CORRIDOR;
  r->description = "Systems Corridor";
  r->long_description = "This section of hallway has a doorway to the north "
                        "labelled \"Planateree\n"
                        "Deefens.\" The corridor continues east and west.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->north = R_PLANETARY_DEFENSE;
  r->west = R_SYSTEMS_CORRIDOR_WEST;
  r->east = R_SYSTEMS_CORRIDOR_EAST;

  // R_SYSTEMS_CORRIDOR_EAST
  r = &objects[R_SYSTEMS_CORRIDOR_EAST];
  r->id = R_SYSTEMS_CORRIDOR_EAST;
  r->description = "Systems Corridor East";
  r->long_description = "The hallway ends here with a large doorway leading "
                        "east, and smaller doorways\n"
                        "to the north and south. The northern doorway is "
                        "labelled \"Planateree Kors\n"
                        "Kontrool.\" The hallway itself leads west.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->west = R_SYSTEMS_CORRIDOR;
  r->north = R_PLANETARY_COURSE_CONTROL;
  r->east = R_PHYSICAL_PLANT_TWO;
  r->south = R_LIBRARY_LOBBY;

  // R_PHYSICAL_PLANT_TWO
  r = &objects[R_PHYSICAL_PLANT_TWO];
  r->id = R_PHYSICAL_PLANT_TWO;
  r->description = "Physical Plant";
  r->long_description = "This is an enormous room full of environmental "
                        "control equipment presumably\n"
                        "intended to heat and ventilate the Lawanda Complex. "
                        "Oddly, although the\n"
                        "Lawanda Complex is slightly smaller than its "
                        "counterpart, this plant is much\n"
                        "larger than the one in the Kalamontee Complex. The "
                        "only exit is westward.";
  r->flags = F_FLOYDBIT | F_RLANDBIT | F_ONBIT;
  r->west = R_SYSTEMS_CORRIDOR_EAST;
  r->out = R_SYSTEMS_CORRIDOR_EAST;
  // Pseudo: EQUIPMENT

  // R_PLANETARY_DEFENSE
  r = &objects[R_PLANETARY_DEFENSE];
  r->id = R_PLANETARY_DEFENSE;
  r->description = "Planetary Defense";
  r->long_description =
      "This room is filled with a dazzling array of lights and controls. There "
      "is a small access panel on one wall.";
  r->flags = F_RLANDBIT | F_FLOYDBIT | F_ONBIT;
  r->south = R_SYSTEMS_CORRIDOR;
  r->out = R_SYSTEMS_CORRIDOR;
  r->adjectives[0] = "planetary";
  r->synonyms[0] = "defense";
  // Globals: CONTROLS, LIGHTS

  // R_PLANETARY_COURSE_CONTROL
  r = &objects[R_PLANETARY_COURSE_CONTROL];
  r->id = R_PLANETARY_COURSE_CONTROL;
  r->description = "Course Control";
  r->long_description =
      "This is a long room whose walls are covered with complicated controls\n"
      "and colored lights.";
  // Dynamic desc with blinking lights
  r->flags = F_FLOYDBIT | F_RLANDBIT | F_ONBIT;
  r->south = R_SYSTEMS_CORRIDOR_EAST;
  r->out = R_SYSTEMS_CORRIDOR_EAST;
  r->synonyms[0] = "course";
  r->synonyms[1] = "control";
  r->adjectives[0] = "planetary";
  // Globals: CONTROLS, LIGHTS

  // R_LIBRARY_LOBBY
  r = &objects[R_LIBRARY_LOBBY];
  r->id = R_LIBRARY_LOBBY;
  r->description = "Library Lobby";
  r->long_description = "This is a carpeted room, thick with dust, with exits "
                        "to the north and south.\n"
                        "To the west, up a few steps, is a wide doorway. A "
                        "small booth lies to the\n"
                        "east.";
  r->flags = F_FLOYDBIT | F_RLANDBIT | F_ONBIT;
  r->up = R_LIBRARY;
  r->west = R_LIBRARY;
  r->north = R_SYSTEMS_CORRIDOR_EAST;
  // r->south = R_PROJECT_CORRIDOR_EAST; // Not yet defined
  r->east = R_BOOTH_3;
  r->in = R_BOOTH_3;
  // Globals: TABLES, STAIRS
  // Pseudo: CARPET, BOOTH

  // R_BOOTH_3
  r = &objects[R_BOOTH_3];
  r->id = R_BOOTH_3;
  r->description = "Booth 3";
  r->long_description = "This is a tiny room with a large \"3\" painted on the "
                        "wall. A panel contains\n"
                        "a slot about ten centimeters wide, a brown button "
                        "labelled \"1\" and a beige\n"
                        "button labelled \"2.\"";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->west = R_LIBRARY_LOBBY;
  r->out = R_LIBRARY_LOBBY;
  // Globals: CONTROLS, SLOT, TELEPORTATION_BUTTON_1, TELEPORTATION_BUTTON_2
  // Pseudo: BOOTH

  // R_LIBRARY
  r = &objects[R_LIBRARY];
  r->id = R_LIBRARY;
  r->description = "Library";
  r->long_description = "This is a large carpeted room with a desk and many "
                        "small tables. The sole\n"
                        "exit is down a few steps to the east.";
  r->flags = F_RLANDBIT | F_ONBIT | F_FLOYDBIT;
  r->east = R_LIBRARY_LOBBY;
  r->out = R_LIBRARY_LOBBY;
  // Globals: TABLES, STAIRS
  // Pseudo: CARPET, DESK

  // === OBJECTS ===

  // O_LAZARUS_PART
  o = &objects[O_LAZARUS_PART];
  o->id = O_LAZARUS_PART;
  o->description = "medical robot breastplate";
  o->synonyms[0] = "plate";
  o->synonyms[1] = "part";
  o->synonyms[2] = "breastplate";
  o->adjectives[0] = "medical";
  o->adjectives[1] = "breast";
  o->size = 35;
  o->flags = F_TAKEBIT;
  // Location: Initially Nowhere, brought by Floyd logic

  // O_RED_SPOOL
  o = &objects[O_RED_SPOOL];
  o->id = O_RED_SPOOL;
  o->description = "red spool";
  o->synonyms[0] = "spool";
  o->adjectives[0] = "red";
  o->adjectives[1] = "small";
  o->flags = F_TAKEBIT | F_ACIDBIT | F_READBIT;
  o->size = 3;
  o->text = "The spool is labelled \"Simptumz uv Xe Dizeez.\"";
  obj_move(O_RED_SPOOL, R_INFIRMARY);

  // O_MEDICINE_BOTTLE
  o = &objects[O_MEDICINE_BOTTLE];
  o->id = O_MEDICINE_BOTTLE;
  o->description = "medicine bottle";
  o->synonyms[0] = "bottle";
  o->adjectives[0] = "medicine";
  o->adjectives[1] = "small";
  o->adjectives[2] = "white";
  o->flags = F_CONTBIT | F_SEARCHBIT | F_TAKEBIT | F_TRANSBIT | F_READBIT;
  o->size = 7;
  o->capacity = 7;
  o->text = "\"Dizeez supreshun medisin -- eksperimentul\"";
  obj_move(O_MEDICINE_BOTTLE, R_INFIRMARY);

  // O_MEDICINE
  o = &objects[O_MEDICINE];
  o->id = O_MEDICINE;
  o->description = "quantity of medicine";
  o->synonyms[0] = "medicine";
  o->adjectives[0] = "small";
  o->adjectives[1] = "quantity";
  o->adjectives[2] = "experimental";
  o->flags = F_FOODBIT;
  o->size = 7;
  obj_move(O_MEDICINE, O_MEDICINE_BOTTLE);

  // O_ACHILLES
  o = &objects[O_ACHILLES];
  o->id = O_ACHILLES;
  o->description = "broken robot";
  o->long_description =
      "Lying face down at the bottom of the stairs is a motionless robot. It\n"
      "appears to be damaged beyond repair.";
  o->synonyms[0] = "robot";
  o->synonyms[1] = "achilles";
  o->adjectives[0] = "broken";
  o->adjectives[1] = "dead";
  obj_move(O_ACHILLES, R_REPAIR_ROOM);

  // O_ROBOT_HOLE
  o = &objects[O_ROBOT_HOLE];
  o->id = O_ROBOT_HOLE;
  o->description = "small doorway";
  o->synonyms[0] = "doorway";
  o->synonyms[1] = "hole";
  o->adjectives[0] = "small";
  o->adjectives[1] = "very";
  o->flags = F_NDESCBIT | F_TRANSBIT | F_CONTBIT;
  obj_move(O_ROBOT_HOLE, R_REPAIR_ROOM);

  // O_GOOD_BOARD (inside hole)
  o = &objects[O_GOOD_BOARD];
  o->id = O_GOOD_BOARD;
  o->description = "shiny seventeen-centimeter fromitz board";
  o->synonyms[0] = "board";
  o->adjectives[0] = "good";
  o->adjectives[1] = "shiny";
  o->adjectives[2] = "fromitz";
  o->size = 10;
  o->flags =
      F_ACIDBIT | F_INVISIBLE | F_NDESCBIT; // Invisible until Floyd finds it
  obj_move(O_GOOD_BOARD, O_ROBOT_HOLE);

  // O_ACCESS_PANEL
  o = &objects[O_ACCESS_PANEL];
  o->id = O_ACCESS_PANEL;
  o->description = "access panel";
  o->synonyms[0] = "panel";
  o->synonyms[1] = "door";
  o->synonyms[2] = "socket";
  o->flags = F_VOWELBIT | F_NDESCBIT | F_CONTBIT | F_SEARCHBIT;
  o->capacity = 4;
  o->action = access_panel_f;
  obj_move(O_ACCESS_PANEL, R_PLANETARY_DEFENSE);

  // O_FIRST_BOARD
  o = &objects[O_FIRST_BOARD];
  o->id = O_FIRST_BOARD;
  o->description = "first seventeen-centimeter fromitz board";
  o->synonyms[0] = "board";
  o->adjectives[0] = "first";
  o->adjectives[1] = "fromitz";
  obj_move(O_FIRST_BOARD, O_ACCESS_PANEL);

  // O_SECOND_BOARD
  o = &objects[O_SECOND_BOARD];
  o->id = O_SECOND_BOARD;
  o->description = "second seventeen-centimeter fromitz board";
  o->synonyms[0] = "board";
  o->adjectives[0] = "second";
  o->flags = F_TRYTAKEBIT | F_TAKEBIT;
  obj_move(O_SECOND_BOARD, O_ACCESS_PANEL);

  // O_THIRD_BOARD
  o = &objects[O_THIRD_BOARD];
  o->id = O_THIRD_BOARD;
  o->description = "third seventeen-centimeter fromitz board";
  o->synonyms[0] = "board";
  o->adjectives[0] = "third";
  obj_move(O_THIRD_BOARD, O_ACCESS_PANEL);

  // O_FOURTH_BOARD
  o = &objects[O_FOURTH_BOARD];
  o->id = O_FOURTH_BOARD;
  o->description = "fourth seventeen-centimeter fromitz board";
  o->synonyms[0] = "board";
  o->adjectives[0] = "fourth";
  obj_move(O_FOURTH_BOARD, O_ACCESS_PANEL);

  // O_FRIED_BOARD
  o = &objects[O_FRIED_BOARD];
  o->id = O_FRIED_BOARD;
  o->description = "fried seventeen-centimeter fromitz board";
  o->synonyms[0] = "board";
  o->adjectives[0] = "fried";
  o->flags = F_ACIDBIT | F_TAKEBIT;
  o->size = 10;
  // Location: Nowhere initially

  // O_CUBE
  o = &objects[O_CUBE];
  o->id = O_CUBE;
  o->description = "large metal cube";
  o->synonyms[0] = "cube";
  o->synonyms[1] = "lid";
  o->adjectives[0] = "large";
  o->adjectives[1] = "metal";
  o->flags = F_MUNGBIT | F_NDESCBIT | F_CONTBIT | F_SEARCHBIT;
  obj_move(O_CUBE, R_DECK_NINE); // Temp for test, was R_PLANETARY_COURSE_CONTROL

  // O_BAD_BEDISTOR
  o = &objects[O_BAD_BEDISTOR];
  o->id = O_BAD_BEDISTOR;
  o->description = "fused ninety-ohm bedistor";
  o->synonyms[0] = "bedistor";
  o->adjectives[0] = "fused";
  o->adjectives[1] = "ninety-ohm";
  o->size = 8;
  o->flags = F_ACIDBIT | F_TRYTAKEBIT | F_TAKEBIT;
  o->action = bad_bedistor_f;
  obj_move(O_BAD_BEDISTOR, O_CUBE);

  // O_GREEN_SPOOL
  o = &objects[O_GREEN_SPOOL];
  o->id = O_GREEN_SPOOL;
  o->description = "green spool";
  o->synonyms[0] = "spool";
  o->adjectives[0] = "green";
  o->size = 3;
  o->flags = F_ACIDBIT | F_TAKEBIT | F_READBIT;
  o->text = "The spool is labelled \"Helikoptur Opuraateeng Manyuuwul.\"";
  obj_move(O_GREEN_SPOOL, R_LIBRARY_LOBBY);

  // O_TERMINAL
  o = &objects[O_TERMINAL];
  o->id = O_TERMINAL;
  o->description = "terminal";
  o->long_description = "On the table is a computer terminal.";
  o->synonyms[0] = "terminal";
  o->synonyms[1] = "keyboard";
  o->synonyms[2] = "screen";
  o->adjectives[0] = "computer";
  o->adjectives[1] = "video";
  o->flags = F_LIGHTBIT;
  obj_move(O_TERMINAL, R_LIBRARY_LOBBY);

  // === REMAINING ROOMS ===

  // R_PROJECT_CORRIDOR_WEST
  r = &objects[R_PROJECT_CORRIDOR_WEST];
  r->id = R_PROJECT_CORRIDOR_WEST;
  r->description = "Project Corridor West";
  r->long_description = "This is a curving hallway leading east and "
                        "northwest. There is an opening\n"
                        "to the west.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->east = R_PROJECT_CORRIDOR;
  r->nw = R_FORK;
  r->west = R_SANFAC_F;

  // R_SANFAC_F
  r = &objects[R_SANFAC_F];
  r->id = R_SANFAC_F;
  r->description = "SanFac F";
  r->long_description = "This is another dusty sanitary facility. Unlike the "
                        "ones near the dorms,\n"
                        "this one is smaller and has no bathing fixtures.";
  r->flags = F_FLOYDBIT | F_RLANDBIT | F_ONBIT;
  r->east = R_PROJECT_CORRIDOR_WEST;
  r->out = R_PROJECT_CORRIDOR_WEST;
  // Pseudo: FIXTURES, TOILET

  // R_PROJECT_CORRIDOR
  r = &objects[R_PROJECT_CORRIDOR];
  r->id = R_PROJECT_CORRIDOR;
  r->description = "Project Corridor";
  r->long_description = "You are at the center of a long east-west hallway. A "
                        "doorway, labelled\n"
                        "\"PrajKon Awfis\", opens to the south.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->east = R_PROJECT_CORRIDOR_EAST;
  r->west = R_PROJECT_CORRIDOR_WEST;
  r->south = R_PROJCON_OFFICE;

  // R_PROJCON_OFFICE
  r = &objects[R_PROJCON_OFFICE];
  r->id = R_PROJCON_OFFICE;
  r->description = "ProjCon Office";
  r->long_description =
      "This office looks like a headquarters of some kind."; // Simplified
  r->flags = F_RLANDBIT | F_FLOYDBIT | F_ONBIT;
  r->north = R_PROJECT_CORRIDOR;
  r->east = R_COMPUTER_ROOM;
  r->south = R_CRYO_ELEVATOR; // Dynamic check needed in action
  // Global: CRYO-ELEVATOR-DOOR
  // Pseudo: MURAL, LOGO

  // R_CRYO_ELEVATOR
  r = &objects[R_CRYO_ELEVATOR];
  r->id = R_CRYO_ELEVATOR;
  r->description = "Cryo-Elevator";
  r->flags = F_RLANDBIT | F_ONBIT;
  // Dynamic exit north
  // Pseudo: BUTTON

  // R_CRYO_ANTEROOM
  r = &objects[R_CRYO_ANTEROOM];
  r->id = R_CRYO_ANTEROOM;
  r->description = "Cryo-Anteroom";
  r->flags = F_RLANDBIT | F_ONBIT;

  // R_PROJECT_CORRIDOR_EAST
  r = &objects[R_PROJECT_CORRIDOR_EAST];
  r->id = R_PROJECT_CORRIDOR_EAST;
  r->description = "Project Corridor East";
  r->long_description = "The hallway ends here but continues back toward the "
                        "west. Doorways lead\n"
                        "north, south and east.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->north = R_LIBRARY_LOBBY;
  r->south = R_COMPUTER_ROOM;
  r->west = R_PROJECT_CORRIDOR;
  r->east = R_MAIN_LAB;

  // R_COMPUTER_ROOM
  r = &objects[R_COMPUTER_ROOM];
  r->id = R_COMPUTER_ROOM;
  r->description = "Computer Room";
  r->long_description = "This is the main computer room for the Project. The "
                        "only sign of activity is\n"
                        "a glowing red light. The exits are north, west, and "
                        "northeast. To the south\n"
                        "is a small booth.";
  r->flags = F_RLANDBIT | F_FLOYDBIT | F_ONBIT;
  r->north = R_PROJECT_CORRIDOR_EAST;
  r->west = R_PROJCON_OFFICE;
  r->south = R_MINI_BOOTH;
  r->in = R_MINI_BOOTH;
  r->ne = R_MAIN_LAB;
  // Global: LIGHTS
  // Pseudo: BOOTH

  // R_MINI_BOOTH
  r = &objects[R_MINI_BOOTH];
  r->id = R_MINI_BOOTH;
  r->description = "Miniaturization Booth";
  r->long_description = "This is a small room barely large enough for one "
                        "person. Mounted on the wall\n"
                        "is a small slot, and next to it a keyboard with "
                        "numeric keys. The exit is to\n"
                        "the north.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->north = R_COMPUTER_ROOM;
  r->out = R_COMPUTER_ROOM;
  // Global: SLOT
  // Pseudo: KEYBOARD, BOOTH

  // R_MAIN_LAB
  r = &objects[R_MAIN_LAB];
  r->id = R_MAIN_LAB;
  r->description = "Main Lab";
  r->long_description = "This is the heart of the Project's vast laboratory "
                        "complex. There are exits\n"
                        "to the west and southwest, and heavy metal doors to "
                        "the northeast and\n"
                        "southeast. A small doorway leads south.";
  r->flags = F_RLANDBIT | F_FLOYDBIT | F_ONBIT;
  r->west = R_PROJECT_CORRIDOR_EAST;
  r->south = R_LAB_STORAGE;
  r->sw = R_COMPUTER_ROOM;
  r->se = R_BIO_LOCK_WEST;       // Dynamic check needed
  r->ne = R_RADIATION_LOCK_WEST; // Dynamic check needed
  // Global: BIO-DOOR-WEST, RAD-DOOR-WEST

  // R_LAB_STORAGE
  r = &objects[R_LAB_STORAGE];
  r->id = R_LAB_STORAGE;
  r->description = "Lab Storage";
  r->long_description = "This is a tiny room for the storage of laboratory "
                        "supplies. The sole exit is\n"
                        "to the north.";
  r->flags = F_RLANDBIT | F_FLOYDBIT | F_ONBIT;
  r->north = R_MAIN_LAB;
  r->out = R_MAIN_LAB;
  // Global: SHELVES
  // Pseudo: RACK, SUPPLIES

  // R_BIO_LOCK_WEST
  r = &objects[R_BIO_LOCK_WEST];
  r->id = R_BIO_LOCK_WEST;
  r->description = "Bio Lock West";
  r->long_description = "This is the first half of a sterilization chamber. "
                        "The door to the west leads to the main lab, and the "
                        "bio lock continues eastward.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->east = R_BIO_LOCK_EAST;
  r->west = R_MAIN_LAB; // Dynamic
  // Global: BIO-DOOR-WEST

  // R_BIO_LOCK_EAST
  r = &objects[R_BIO_LOCK_EAST];
  r->id = R_BIO_LOCK_EAST;
  r->description = "Bio Lock East";
  r->long_description = "The is the second half of the sterilization chamber "
                        "leading from the main\n"
                        "lab to the Bio Lab. The door to the east, leading to "
                        "the Bio Lab, has a\n"
                        "window. The bio lock continues to the west.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->east = R_BIO_LAB; // Dynamic
  r->west = R_BIO_LOCK_WEST;
  // Global: BIO-DOOR-EAST, WINDOW

  // R_RADIATION_LOCK_WEST
  r = &objects[R_RADIATION_LOCK_WEST];
  r->id = R_RADIATION_LOCK_WEST;
  r->description = "Radiation Lock West";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->east = R_RADIATION_LOCK_EAST;
  r->west = R_MAIN_LAB; // Dynamic

  // R_RADIATION_LOCK_EAST
  r = &objects[R_RADIATION_LOCK_EAST];
  r->id = R_RADIATION_LOCK_EAST;
  r->description = "Radiation Lock East";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->east = R_RADIATION_LAB; // Dynamic
  r->west = R_RADIATION_LOCK_WEST;

  // R_BIO_LAB
  r = &objects[R_BIO_LAB];
  r->id = R_BIO_LAB;
  r->description = "Bio Lab";
  r->long_description = "This is a huge laboratory filled with many biological "
                        "experiments.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->east = R_LAB_OFFICE;    // Dynamic
  r->west = R_BIO_LOCK_EAST; // Dynamic
  // Global: WINDOW, OFFICE-DOOR
  // Pseudo: CRACK

  // R_RADIATION_LAB
  r = &objects[R_RADIATION_LAB];
  r->id = R_RADIATION_LAB;
  r->description = "Radiation Lab";
  r->long_description = "This room is filled with unfathomable equipment and "
                        "large canisters bearing\n"
                        "radioactive warnings.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->west = R_RADIATION_LOCK_EAST; // Dynamic
  // Pseudo: CRACK, EQUIPMENT

  // R_LAB_OFFICE
  r = &objects[R_LAB_OFFICE];
  r->id = R_LAB_OFFICE;
  r->description = "Lab Office";
  r->long_description = "This is the office for storing files on Bio Lab "
                        "experiments. A large and\n"
                        "messy desk is surrounded by locked files. A small "
                        "booth lies to the south.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->west = R_BIO_LAB; // Dynamic
  r->south = R_AUXILIARY_BOOTH;
  r->in = R_AUXILIARY_BOOTH;
  // Pseudo: FILES, BOOTH

  // R_AUXILIARY_BOOTH
  r = &objects[R_AUXILIARY_BOOTH];
  r->id = R_AUXILIARY_BOOTH;
  r->description = "Auxiliary Booth";
  r->long_description = "This is another small booth. Unlike the "
                        "Miniaturization Booth, this room\n"
                        "has no slot or keyboard, so presumably it is intended "
                        "only as a receiving\n"
                        "station. The exit is on the northern side.";
  r->flags = F_RLANDBIT | F_ONBIT;
  r->north = R_LAB_OFFICE;
  r->out = R_LAB_OFFICE;
  // Pseudo: BOOTH

  // R_STATION_384
  r = &objects[R_STATION_384];
  r->id = R_STATION_384;
  r->description = "Station 384";
  r->flags = F_RLANDBIT | F_ONBIT;

  // === REMAINING OBJECTS ===

  // O_CRYO_ELEVATOR_DOOR
  o = &objects[O_CRYO_ELEVATOR_DOOR];
  o->id = O_CRYO_ELEVATOR_DOOR;
  o->description = "cryo-elevator door";
  o->synonyms[0] = "door";
  o->adjectives[0] = "elevator";
  o->flags = F_NDESCBIT | F_INVISIBLE;

  // O_PRINT_OUT
  o = &objects[O_PRINT_OUT];
  o->id = O_PRINT_OUT;
  o->description = "pile of computer output";
  o->synonyms[0] = "pile";
  o->synonyms[1] = "paper";
  o->synonyms[2] = "output";
  o->synonyms[3] = "printout";
  o->flags = F_ACIDBIT | F_TAKEBIT | F_READBIT;
  obj_move(O_PRINT_OUT, R_COMPUTER_ROOM);

  // O_MINI_CARD
  o = &objects[O_MINI_CARD];
  o->id = O_MINI_CARD;
  o->description = "miniaturization access card";
  o->synonyms[0] = "card";
  o->adjectives[0] = "mini";
  o->adjectives[1] = "access";
  o->flags = F_TAKEBIT | F_NDESCBIT | F_INVISIBLE | F_READBIT;
  o->text = "The card is embossed \"minitcurizaashun akses kard.\"";
  obj_move(O_MINI_CARD, R_BIO_LOCK_EAST);

  // O_BIO_DOOR_EAST etc. need to be initialized if handled as objects
  o = &objects[O_BIO_DOOR_EAST];
  o->id = O_BIO_DOOR_EAST;
  o->description = "lab door";
  o->synonyms[0] = "door";
  o->flags = F_DOORBIT | F_NDESCBIT;

  o = &objects[O_BIO_DOOR_WEST];
  o->id = O_BIO_DOOR_WEST;
  o->description = "bio-lock door";
  o->synonyms[0] = "door";
  o->flags = F_DOORBIT | F_NDESCBIT;

  o = &objects[O_RAD_DOOR_EAST];
  o->id = O_RAD_DOOR_EAST;
  o->description = "lab door";
  o->synonyms[0] = "door";
  o->flags = F_DOORBIT | F_NDESCBIT;

  o = &objects[O_RAD_DOOR_WEST];
  o->id = O_RAD_DOOR_WEST;
  o->description = "radiation-lock door";
  o->synonyms[0] = "door";
  o->flags = F_DOORBIT | F_NDESCBIT;

  // O_LAB_UNIFORM
  o = &objects[O_LAB_UNIFORM];
  o->id = O_LAB_UNIFORM;
  o->description = "lab uniform";
  o->synonyms[0] = "uniform";
  o->adjectives[0] = "lab";
  o->flags = F_TAKEBIT | F_CONTBIT | F_SEARCHBIT | F_WEARBIT;
  o->capacity = 5;
  obj_move(O_LAB_UNIFORM, R_LAB_STORAGE);

  // O_TELEPORTATION_CARD
  o = &objects[O_TELEPORTATION_CARD];
  o->id = O_TELEPORTATION_CARD;
  o->description = "teleportation access card";
  o->synonyms[0] = "card";
  o->flags = F_TAKEBIT | F_READBIT;
  obj_move(O_TELEPORTATION_CARD, O_LAB_UNIFORM);

  // O_COMBINATION_PAPER
  o = &objects[O_COMBINATION_PAPER];
  o->id = O_COMBINATION_PAPER;
  o->description = "piece of paper";
  o->synonyms[0] = "paper";
  o->flags = F_ACIDBIT | F_TAKEBIT | F_READBIT;
  obj_move(O_COMBINATION_PAPER, O_LAB_UNIFORM);

  // O_LAB_DESK
  o = &objects[O_LAB_DESK];
  o->id = O_LAB_DESK;
  o->description = "desk";
  o->synonyms[0] = "desk";
  o->flags = F_CONTBIT | F_SEARCHBIT | F_NDESCBIT;
  o->capacity = 10;
  obj_move(O_LAB_DESK, R_LAB_OFFICE);

  // O_GAS_MASK
  o = &objects[O_GAS_MASK];
  o->id = O_GAS_MASK;
  o->description = "gas mask";
  o->synonyms[0] = "mask";
  o->flags = F_ACIDBIT | F_TAKEBIT | F_WEARBIT;
  obj_move(O_GAS_MASK, O_LAB_DESK);

  // O_MEMO
  o = &objects[O_MEMO];
  o->id = O_MEMO;
  o->description = "memo";
  o->synonyms[0] = "memo";
  o->flags = F_ACIDBIT | F_TAKEBIT | F_READBIT;
  o->text = "Memo to all lab personnel..."; // Simplified
  // Not moved anywhere? ZIL says MOVE MEMO ADVENTURER in action. Wait, ZIL
  // object def doesn't have IN. It says (DESC "memo"). So it's initially
  // nowhere? Wait, LAB-DESK-F moves it to adventurer. So yeah, initially
  // nowhere or inside desk if not handled by action. I will put it in desk for
  // now or leave it nowhere if logic handles it. ZIL: <OBJECT MEMO ...> no IN.
  // But LAB-DESK-F says: <MOVE ,MEMO ,ADVENTURER>. So it logic-spawns.

  // Buttons
  o = &objects[O_LIGHT_BUTTON];
  o->id = O_LIGHT_BUTTON;
  o->description = "white button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "white";
  o->flags = F_NDESCBIT;
  obj_move(O_LIGHT_BUTTON, R_LAB_OFFICE);

  o = &objects[O_DARK_BUTTON];
  o->id = O_DARK_BUTTON;
  o->description = "black button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "black";
  o->flags = F_NDESCBIT;
  obj_move(O_DARK_BUTTON, R_LAB_OFFICE);

  o = &objects[O_FUNGICIDE_BUTTON];
  o->id = O_FUNGICIDE_BUTTON;
  o->description = "red button";
  o->synonyms[0] = "button";
  o->adjectives[0] = "red";
  o->flags = F_NDESCBIT;
  obj_move(O_FUNGICIDE_BUTTON, R_LAB_OFFICE);

  // O_BROWN_SPOOL
  o = &objects[O_BROWN_SPOOL];
  o->id = O_BROWN_SPOOL;
  o->description = "brown spool";
  o->synonyms[0] = "spool";
  o->adjectives[0] = "brown";
  o->flags = F_ACIDBIT | F_TAKEBIT | F_READBIT;
  obj_move(O_BROWN_SPOOL, R_RADIATION_LAB);

  // O_LAMP
  o = &objects[O_LAMP];
  o->id = O_LAMP;
  o->description = "portable lamp";
  o->synonyms[0] = "lamp";
  o->flags = F_TAKEBIT;
  obj_move(O_LAMP, R_RADIATION_LAB);
}
