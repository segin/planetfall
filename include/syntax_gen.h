#ifndef SYNTAX_GEN_H
#define SYNTAX_GEN_H

#include "planetfall.h"

// Actions
typedef enum {
    V_NULL = 0,
    V_$COMMAND,
    V_$RANDOM,
    V_$RECORD,
    V_$UNRECORD,
    V_$VERIFY,
    V_AGAIN,
    V_ALARM,
    V_ANSWER,
    V_ASK_FOR,
    V_ATTACK,
    V_ATTRACT,
    V_BOARD,
    V_BRIEF,
    V_CLIMB_DOWN,
    V_CLIMB_FOO,
    V_CLIMB_ON,
    V_CLIMB_UP,
    V_CLOSE,
    V_CURSE,
    V_DIAGNOSE,
    V_DISEMBARK,
    V_DROP,
    V_EAT,
    V_EAT_FROM,
    V_EMPTY,
    V_ENTER,
    V_ESCAPE,
    V_EXAMINE,
    V_EXIT,
    V_FIND,
    V_FIX_IT,
    V_FLUSH,
    V_FLY,
    V_FOLLOW,
    V_GIVE,
    V_GO_UP,
    V_HELLO,
    V_HELP,
    V_INSERT,
    V_INVENTORY,
    V_KICK,
    V_KISS,
    V_KNOCK,
    V_LAMP_OFF,
    V_LAMP_ON,
    V_LEAP,
    V_LEAVE,
    V_LISTEN,
    V_LOCK,
    V_LOOK,
    V_LOOK_BEHIND,
    V_LOOK_CRETIN,
    V_LOOK_DOWN,
    V_LOOK_INSIDE,
    V_LOOK_UNDER,
    V_MAYBE,
    V_MOVE,
    V_MUNG,
    V_NO,
    V_OIL,
    V_OPEN,
    V_OPEN_WITH,
    V_PLAY,
    V_PLAY_WITH,
    V_POINT,
    V_POUR,
    V_PULL,
    V_PUSH,
    V_PUSH_DOWN,
    V_PUSH_UP,
    V_PUT,
    V_PUT_ON,
    V_PUT_UNDER,
    V_QUIT,
    V_RAPE,
    V_REACH,
    V_REACH_FOR,
    V_READ,
    V_REMOVE,
    V_REPLY,
    V_RESTART,
    V_RESTORE,
    V_RUB,
    V_SALUTE,
    V_SAVE,
    V_SAY,
    V_SCOLD,
    V_SCORE,
    V_SCRIPT,
    V_SCRUB,
    V_SEARCH,
    V_SET,
    V_SGIVE,
    V_SHAKE,
    V_SHAKE_WITH,
    V_SHOW,
    V_SIT,
    V_SIT_DOWN,
    V_SKIP,
    V_SLEEP,
    V_SLIDE,
    V_SMELL,
    V_SMILE,
    V_SPAN,
    V_STAND,
    V_STAND_ON,
    V_STEP_ON,
    V_SUPER_BRIEF,
    V_SWIM,
    V_SWIM_DIR,
    V_SWIM_UP,
    V_SZAP,
    V_TAKE,
    V_TAKE_OFF,
    V_TALK,
    V_TASTE,
    V_TELL,
    V_THROUGH,
    V_THROW,
    V_THROW_OFF,
    V_TIME,
    V_TURN,
    V_TYPE,
    V_UNLOCK,
    V_UNSCRIPT,
    V_VERBOSE,
    V_VERSION,
    V_WAIT,
    V_WALK,
    V_WALK_AROUND,
    V_WALK_TO,
    V_WAVE,
    V_WEAR,
    V_YELL,
    V_YES,
    V_ZAP,
    V_ZATTRACT,
    V_ZESCAPE,
    V_ZORK,
} ActionID;

// Search Flags
#define SEARCH_HELD   (1<<0)
#define SEARCH_ROOM   (1<<1)
#define SEARCH_GROUND (1<<2)
#define PARSE_MANY    (1<<3)
#define PARSE_TRY_TAKE (1<<4)

// Preposition Locations
#define PREP_LOC_NONE 0
#define PREP_LOC_BEFORE_OBJ1 1
#define PREP_LOC_AFTER_OBJ1 2
#define PREP_LOC_AFTER_OBJ2 3

typedef enum { VOCAB_UNKNOWN, VOCAB_VERB, VOCAB_PREP, VOCAB_BUZZ, VOCAB_SYNONYM } VocabType;

typedef struct {
    const char* word;
    VocabType type;
    const char* target; // For synonyms
} VocabEntry;

typedef struct {
    const char* verb_word;
    const char* prep1;
    const char* prep2;
    int obj1_present;
    int obj2_present;
    int prep1_loc;
    int prep2_loc;
    unsigned int obj1_search;
    unsigned int obj2_search;
    unsigned int obj1_find;
    unsigned int obj2_find;
    ActionID action_id;
} SyntaxEntry;

extern VocabEntry vocab_table[];
extern int vocab_table_size;
extern SyntaxEntry syntax_table[];
extern int syntax_table_size;

#endif
