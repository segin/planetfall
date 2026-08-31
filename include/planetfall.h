#ifndef PLANETFALL_H
#define PLANETFALL_H

#include "ids.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Max number of objects in the game
#define MAX_OBJECTS 1000

// Time constants, from globals.zil / misc.zil.
//
// The clock is denominated in Galactic Standard Time units, not turns. Every
// action costs C_ELAPSED_DEFAULT unless its handler overrides c_elapsed;
// movement costs DEFAULT_MOVE. Queued events count down by the cost of the
// action just taken, so a turn spent walking burns roughly three times as much
// of the clock as a turn spent looking at something.
#define C_ELAPSED_DEFAULT 7 // <CONSTANT C-ELAPSED-DEFAULT 7>
#define DEFAULT_MOVE 20     // <CONSTANT DEFAULT-MOVE 20>

// Time cost the pod trip forces once it is underway (misc.zil MAIN-LOOP).
#define C_ELAPSED_POD_TRIP 54

// Object IDs
typedef int ZObjectID;

// Boolean Flags (Attributes)
typedef enum {
  F_INVISIBLE = 1ULL << 0,
  F_TOUCHBIT = 1ULL << 1,
  F_SURFACEBIT = 1ULL << 2,
  F_TRYTAKEBIT = 1ULL << 3,
  F_MUNGBIT = 1ULL << 4,
  F_MUNGEDBIT = 1ULL << 5,
  F_SCRAMBLEDBIT = 1ULL << 6,
  F_WORNBIT = 1ULL << 7,
  F_OPENBIT = 1ULL << 8,
  F_SEARCHBIT = 1ULL << 9,
  F_TRANSBIT = 1ULL << 10,
  F_WEARBIT = 1ULL << 11,
  F_NDESCBIT = 1ULL << 12,
  F_CLIMBBIT = 1ULL << 13,
  F_VOWELBIT = 1ULL << 14,
  F_ACTORBIT = 1ULL << 15,
  F_VEHBIT = 1ULL << 16,
  F_ACIDBIT = 1ULL << 17,
  F_FOODBIT = 1ULL << 18,
  F_READBIT = 1ULL << 19,
  F_TAKEBIT = 1ULL << 20,
  F_CONTBIT = 1ULL << 21,
  F_LIGHTBIT = 1ULL << 22,
  F_ONBIT = 1ULL << 23,
  F_DOORBIT = 1ULL << 24,
  F_TOOLBIT = 1ULL << 25,
  F_RLANDBIT = 1ULL << 26,
  F_FLOYDBIT = 1ULL << 27
} ZFlag;

typedef struct {
  ZObjectID id;
  ZObjectID parent;
  ZObjectID child;
  ZObjectID sibling;

  // Vocabulary for parser matching
  const char *synonyms[5];
  const char *adjectives[5];

  // Descriptions
  const char *description; // Short description (e.g., "brass lantern")
  const char
      *long_description; // Room description or Initial presence description
  const char *text;      // For reading (books, signs)

  uint64_t flags;

  int size;
  int capacity;
  int value; // Score value

  // Action routine ID (to be mapped to function dispatch)
  int action_id;

  // Function pointer for object action (Returns true if handled)
  // Function pointer for object action (Returns true if handled)
  bool (*action)(int context);

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
  int internal_moves; // Master clock, in Galactic Standard Time units
  int moves;          // Time as displayed; 0 when the chronometer is not carried
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
  bool super_brief;
  int munged_time;
  int drown_counter;
  int dial_number;
  int number_needed;
  bool padlock_removed;
  bool ladder_extended;
  bool ladder_flag;
  int c_elapsed;
  ZObjectID spout_placed;
  bool upper_elevator_up;
  bool lower_elevator_up;
  bool upper_elevator_on;
  bool lower_elevator_on;
  bool elevator_in_transit;
  int chemical_flag;
  int chemical_required;
  bool comm_shutdown;
  bool just_entered;
  bool defense_fixed;
  bool course_control_fixed;
  bool comm_fixed;
  bool lawanda_platform_flag;
  bool lazarus_flag;
  bool hole_trip_flag;
  bool board_reported;
  bool access_panel_full;
  bool mural_flag;
  bool cryo_score_flag;
  bool computer_flag;
  bool mini_activated;
  bool lab_lights_on;
  bool lab_flooded;
  int nuked_counter;
  bool uniform_opened;
  bool floyd_waiting;
  int waiting_counter;
  bool floyd_gave_up;
  bool floyd_forayed;
  int foray_counter;
  bool floyd_peered;
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
ZObject *get_obj(ZObjectID id);
void obj_move(ZObjectID obj, ZObjectID dest);
void obj_remove(ZObjectID obj);
bool obj_in(ZObjectID obj, ZObjectID parent);
bool obj_has_flag(ZObjectID obj, ZFlag flag);
void obj_set_flag(ZObjectID obj, ZFlag flag);
void obj_clear_flag(ZObjectID obj, ZFlag flag);
void score_obj(ZObjectID obj);

// Hierarchy Traversal
ZObjectID obj_first_child(ZObjectID parent);
ZObjectID obj_next_sibling(ZObjectID obj);
ZObjectID obj_parent(ZObjectID obj);
void obj_rob(ZObjectID victim, ZObjectID dest);

// Output
#include "output.h"

// Parser/Output
bool is_lit(ZObjectID room);
bool see_inside(ZObjectID obj);
bool is_here(ZObjectID obj);
bool global_in(ZObjectID obj, ZObjectID room);
bool firster(ZObjectID obj, int level);
bool print_cont(ZObjectID obj, bool v, int level);
bool describe_object(ZObjectID obj, bool v, int level);
bool describe_room(bool look);
bool describe_objects(bool look);
bool pre_examine(ZObjectID obj);
void perform_examine(ZObjectID obj);
bool pre_read(ZObjectID obj);
void perform_read(ZObjectID obj);
void perform_look_inside(ZObjectID obj);
void perform_look();
void perform_first_look();
void perform_verbose();
void perform_brief();
void perform_super_brief();
void perform_look_cretin();
int perform_score(bool ask);
bool ask_yes();
void perform_quit();
void finish(bool died, bool repeating);
void perform_version();
#define REXIT 0
#define UEXIT 1
#define NEXIT 2
#define FEXIT 3
#define CEXIT 4
#define DEXIT 5

#define NEXITSTR 0
#define FEXITFCN 0
#define CEXITFLAG 1
#define CEXITSTR 1
#define DEXITOBJ 1
#define DEXITSTR 1

extern const char *indents[6];
// Direction of the move in progress, for NEXIT-style per-direction refusals.
extern ZObjectID walk_direction;
void use_directions();
void perform_walk_around();
void perform_walk_to(ZObjectID obj);
void perform_walk(ZObjectID dest);
void perform_save();
void perform_restore();
void perform_restart();

// Object Manipulation Verbs
bool is_held(ZObjectID obj);
bool pre_take(ZObjectID obj, ZObjectID prsi);
bool trytake(ZObjectID obj);
bool itake(ZObjectID obj, bool verbose);
void perform_take(ZObjectID obj);
bool pre_put(ZObjectID prso, ZObjectID prsi);
void perform_put(ZObjectID prso, ZObjectID prsi);
void perform_slide();
bool pre_give(ZObjectID prso, ZObjectID prsi);
void perform_give(ZObjectID prso, ZObjectID prsi);
void perform_sgive(ZObjectID prso, ZObjectID prsi);
bool idrop(ZObjectID obj);
void perform_drop(ZObjectID obj);
void perform_throw(ZObjectID obj, ZObjectID prsi);

// Events (Included from events.h generally, but we need forward decls if not
// including) Actually, circular include if events.h includes planetfall.h?
// events.h doesn't seem to include planetfall.h (it just includes stdbool).
// So I can include it.
#include "events.h"

// Globals
extern int brigs_up;

#endif
