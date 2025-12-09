#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "planetfall.h"
#include "parser.h"

#define MAX_INPUT_LEN 256
#define MAX_TOKENS 20

// Vocabulary Mappings
typedef struct {
    const char* word;
    int id; 
} VocabEntry;

VocabEntry verbs[] = {
    {"look", V_LOOK},
    {"l", V_LOOK},
    {"inventory", V_INVENTORY},
    {"i", V_INVENTORY},
    {"examine", V_EXAMINE},
    {"x", V_EXAMINE},
    {"take", V_TAKE},
    {"get", V_TAKE},
    {"drop", V_DROP},
    {"quit", V_QUIT},
    {"put", V_PUT},
    {"insert", V_PUT},
    {"wait", V_WAIT},
    {"z", V_WAIT},
    
    // Movement
    {"north", V_WALK_NORTH}, {"n", V_WALK_NORTH},
    {"south", V_WALK_SOUTH}, {"s", V_WALK_SOUTH},
    {"east", V_WALK_EAST},   {"e", V_WALK_EAST},
    {"west", V_WALK_WEST},   {"w", V_WALK_WEST},
    {"ne", V_WALK_NE},
    {"nw", V_WALK_NW},
    {"se", V_WALK_SE},
    {"sw", V_WALK_SW},
    {"up", V_WALK_UP},       {"u", V_WALK_UP},
    {"down", V_WALK_DOWN},   {"d", V_WALK_DOWN},
    {"in", V_WALK_IN},       {"enter", V_WALK_IN}, // 'Enter' can be V_WALK_IN or V_BOARD depending on context
    {"out", V_WALK_OUT},
    
    // Vehicle/Structure Interaction
    {"board", V_BOARD}, // "Board Pod", "Sit in Web"
    {"sit", V_BOARD},
    
    {"disembark", V_DISEMBARK}, // "Get off web"
    {"leave", V_DISEMBARK},     // "Leave Pod" (Object) or "Leave" (Room -> V_WALK_OUT)
                                // Parser needs to distinguish Transitive vs Intransitive.
                                // For now, we map string to ID. logic handled in main.
    {"exit", V_DISEMBARK},      // "Exit Pod"
    {"stand", V_DISEMBARK},     // "Stand up"
    
    {"open", V_OPEN},
    {"close", V_CLOSE},
    {"scrub", V_SCRUB},
    {"clean", V_SCRUB},
    {"attack", V_ATTACK},
    {"kill", V_ATTACK},
    {"kick", V_KICK},
    {"talk", V_TALK},
    {"eat", V_EAT},
    {"smell", V_SMELL},
    {"sniff", V_SMELL},
    {"read", V_READ},
    {"remove", V_REMOVE},
    {"wear", V_WEAR},
    {NULL, 0}
};

VocabEntry prepositions[] = {
    {"in", 1},
    {"inside", 1},
    {"into", 1},
    {"on", 2},
    {"onto", 2},
    {NULL, 0}
};

char* tokens[MAX_TOKENS];
int num_tokens = 0;

void tokenize(char* input) {
    num_tokens = 0;
    char* token = strtok(input, " \t\n\r");
    while (token != NULL && num_tokens < MAX_TOKENS) {
        // Lowercase the token
        for(int i = 0; token[i]; i++){
          token[i] = tolower(token[i]);
        }
        tokens[num_tokens++] = token;
        token = strtok(NULL, " \t\n\r");
    }
}

int match_vocab(const char* word, VocabEntry* table) {
    for (int i = 0; table[i].word != NULL; i++) {
        if (strcmp(word, table[i].word) == 0) {
            return table[i].id;
        }
    }
    return 0;
}

// Helper to match a specific object ID against a word
bool match_id(ZObjectID id, const char* word) {
    if (id == NOTHING) return false;
    ZObject* obj = &objects[id];
    if (obj->synonyms[0] && strcmp(word, obj->synonyms[0]) == 0) return true;
    if (obj->synonyms[1] && strcmp(word, obj->synonyms[1]) == 0) return true;
    return false;
}

// Helper to search a list (siblings)
ZObjectID match_in_list(ZObjectID start_node, const char* word) {
    ZObjectID curr = start_node;
    while (curr != NOTHING) {
        if (match_id(curr, word)) return curr;
        // Simple recursion for open containers
        if (obj_has_flag(curr, F_CONTBIT) && obj_has_flag(curr, F_OPENBIT)) {
            ZObjectID inner = match_in_list(objects[curr].child, word);
            if (inner != NOTHING) return inner;
        }
        curr = objects[curr].sibling;
    }
    return NOTHING;
}

ZObjectID match_object(const char* word) {
    if (!word) return NOTHING;
    ZObjectID found = NOTHING;

    // 1. Check Inventory (ADVENTURER)
    found = match_in_list(objects[player].child, word);
    if (found != NOTHING) return found;

    // 2. Check Room Contents (HERE)
    found = match_in_list(objects[current_room].child, word);
    if (found != NOTHING) return found;

    // 3. Check Room Globals (HERE.globals)
    for (int i = 0; i < 10; i++) {
        ZObjectID gid = objects[current_room].globals[i];
        if (gid != NOTHING) {
            if (match_id(gid, word)) return gid;
        }
    }

    // 4. Check Universal Globals (GLOBAL-OBJECTS)
    found = match_in_list(objects[OBJ_GLOBAL_OBJECTS].child, word);
    if (found != NOTHING) return found;

    return NOTHING;
}

bool parse_command(char* input, Command* cmd) {
    tokenize(input);
    if (num_tokens == 0) return false;
    
    int start_index = 0;
    // Handle "Go North", "Walk In", etc.
    if (strcmp(tokens[0], "go") == 0 || strcmp(tokens[0], "walk") == 0) {
        if (num_tokens > 1) {
            start_index = 1;
        } else {
            printf("Go where?\n");
            return false;
        }
    }

    cmd->verb = 0;
    cmd->direct_object = NOTHING;
    cmd->indirect_object = NOTHING;
    cmd->preposition = 0;
    
    // 1. Identify Verb
    cmd->verb = match_vocab(tokens[start_index], verbs);
    if (cmd->verb == 0) {
        printf("I don't know the word \"%s\".\n", tokens[start_index]);
        return false;
    }
    
    if (num_tokens == start_index + 1) return true; // Just Verb
    
    // 2. Scan for Prepositions
    int prep_index = -1;
    for (int i = start_index + 1; i < num_tokens; i++) {
        int prep_id = match_vocab(tokens[i], prepositions);
        if (prep_id != 0) {
            prep_index = i;
            cmd->preposition = prep_id;
            break;
        }
    }
    
    // 3. Resolve Objects
    if (prep_index == -1) {
        // VERB DIRECT
        // Using tokens[start_index + 1]
        cmd->direct_object = match_object(tokens[start_index + 1]);
        
        if (tokens[start_index + 1] && cmd->direct_object == NOTHING) {
             printf("You can't see any %s here.\n", tokens[start_index + 1]);
             return false;
        }
    } else {
        // Preposition present.
        
        // Case 1: VERB PREP OBJECT (e.g. "Sit on Chair", "Go In Pod")
        // If prep immediately follows verb
        if (prep_index == start_index + 1) {
            // Check if there is an object after the preposition
            if (prep_index + 1 < num_tokens) {
                cmd->direct_object = match_object(tokens[prep_index + 1]);
                if (cmd->direct_object == NOTHING) {
                    printf("You can't see any %s here.\n", tokens[prep_index + 1]);
                    return false;
                }
                
                // Handle "Get In Object" -> Board
                if (cmd->verb == V_TAKE && cmd->preposition == 1) { // 1 = "in"
                    cmd->verb = V_BOARD;
                    cmd->preposition = 0;
                }
            } else {
                 printf("Missing object.\n");
                 return false;
            }
        }
        // Case 2: VERB DIRECT PREP INDIRECT (e.g. "Put Box in Bag")
        else if (prep_index == start_index + 2) {
            cmd->direct_object = match_object(tokens[start_index + 1]);
            if (cmd->direct_object == NOTHING) {
                 printf("You can't see any %s here.\n", tokens[start_index + 1]);
                 return false;
            }
            
            if (prep_index + 1 < num_tokens) {
                cmd->indirect_object = match_object(tokens[prep_index + 1]);
                if (cmd->indirect_object == NOTHING) {
                    printf("You can't see any %s here.\n", tokens[prep_index + 1]);
                    return false;
                }
            } else {
                 printf("Missing indirect object.\n");
                 return false;
            }
        }
        else {
             printf("I didn't understand that sentence structure.\n");
             return false;
        }
    }
    
    return true;
}
