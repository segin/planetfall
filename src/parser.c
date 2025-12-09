#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "parser.h"
#include "syntax_gen.h"

#define MAX_TOKENS 32
#define MAX_WORD_LEN 64

typedef struct {
    char word[MAX_WORD_LEN];
    VocabEntry* vocab; 
} Token;

Token tokens[MAX_TOKENS];
int num_tokens = 0;

VocabEntry* lookup_vocab(const char* word) {
    for (int i = 0; i < vocab_table_size; i++) {
        if (strcasecmp(word, vocab_table[i].word) == 0) {
            return &vocab_table[i];
        }
    }
    return NULL;
}

void tokenize(char* input) {
    num_tokens = 0;
    
    char buffer[MAX_WORD_LEN];
    int buf_idx = 0;
    
    for (int i = 0; input[i]; i++) {
        char c = tolower(input[i]);
        if (isspace(c) || c == ',' || c == '.') {
            if (buf_idx > 0) {
                buffer[buf_idx] = '\0';
                if (num_tokens < MAX_TOKENS) {
                    strncpy(tokens[num_tokens].word, buffer, MAX_WORD_LEN);
                    tokens[num_tokens].vocab = lookup_vocab(buffer);
                    num_tokens++;
                }
                buf_idx = 0;
            }
        } else {
            if (buf_idx < MAX_WORD_LEN - 1) {
                buffer[buf_idx++] = c;
            }
        }
    }
    if (buf_idx > 0) {
        buffer[buf_idx] = '\0';
        if (num_tokens < MAX_TOKENS) {
            strncpy(tokens[num_tokens].word, buffer, MAX_WORD_LEN);
            tokens[num_tokens].vocab = lookup_vocab(buffer);
            num_tokens++;
        }
    }
}

bool word_matches_object(ZObject* obj, const char* word) {
    for (int i = 0; i < 5; i++) {
        if (obj->synonyms[i] && strcasecmp(word, obj->synonyms[i]) == 0) return true;
    }
    for (int i = 0; i < 5; i++) {
        if (obj->adjectives[i] && strcasecmp(word, obj->adjectives[i]) == 0) return true;
    }
    return false;
}

bool phrase_matches_object(ZObject* obj, int start, int end) {
    for (int i = start; i < end; i++) {
        if (!word_matches_object(obj, tokens[i].word)) {
             if (strcmp(tokens[i].word, "the") == 0) continue;
             if (strcmp(tokens[i].word, "a") == 0) continue;
             if (strcmp(tokens[i].word, "an") == 0) continue;
             return false;
        }
    }
    return true;
}

// Returns count found
int snarf_objects(int start, int end, unsigned int search_flags, unsigned int find_flags, ZObjectID* out_list, int max_count) {
    if (start >= end) return 0;
    
    int count = 0;
    bool is_all = (strcasecmp(tokens[start].word, "all") == 0 && (end - start == 1));
    bool is_it = (strcasecmp(tokens[start].word, "it") == 0 && (end - start == 1));

    // Special case: "IT"
    if (is_it) {
        // Return P-IT-OBJECT (global, but we need to track it)
        // For now, fail
        return 0;
    }

    void check_list(ZObjectID parent) {
        ZObjectID curr = objects[parent].child;
        while (curr != NOTHING) {
            if (!obj_has_flag(curr, F_INVISIBLE)) {
                
                bool flags_match = true;
                if (find_flags != 0) {
                     if ((objects[curr].flags & find_flags) == 0) {
                         flags_match = false;
                     }
                }
                
                if (flags_match) {
                    bool match = false;
                    if (is_all) {
                        match = true; // "ALL" matches everything that passes flag check
                    } else {
                        if (phrase_matches_object(&objects[curr], start, end)) {
                            match = true;
                        }
                    }
                    
                    if (match) {
                        // Check if already in list (dedup if searching multiple scopes)
                        bool exists = false;
                        for(int k=0; k<count; k++) if(out_list[k] == curr) exists = true;
                        
                        if (!exists && count < max_count) {
                            out_list[count++] = curr;
                        }
                    }
                }
            }
            
            // Recurse for containers
            if (obj_has_flag(curr, F_CONTBIT) && obj_has_flag(curr, F_OPENBIT)) {
                ZObjectID inner = objects[curr].child;
                 while (inner != NOTHING) {
                      if (!obj_has_flag(inner, F_INVISIBLE)) {
                        bool inner_flags_match = true;
                        if (find_flags != 0) {
                             if ((objects[inner].flags & find_flags) == 0) inner_flags_match = false;
                        }
                        
                        bool match = false;
                        if (is_all) match = true;
                        else if (phrase_matches_object(&objects[inner], start, end)) match = true;
                        
                        if (inner_flags_match && match) {
                             bool exists = false;
                             for(int k=0; k<count; k++) if(out_list[k] == inner) exists = true;
                             if (!exists && count < max_count) out_list[count++] = inner;
                        }
                      }
                      inner = objects[inner].sibling;
                 }
            }
            curr = objects[curr].sibling;
        }
    }

    if (search_flags == 0) {
        check_list(player);
        check_list(current_room);
    } else {
        if (search_flags & SEARCH_HELD) check_list(player);
        if (search_flags & SEARCH_ROOM) check_list(current_room); 
        if (search_flags & SEARCH_GROUND) check_list(current_room);
    }

    check_list(OBJ_GLOBAL_OBJECTS);
    check_list(OBJ_LOCAL_GLOBALS);
    
    // Disambiguation Logic (If not ALL)
    if (!is_all && count > 1) {
        printf("[Ambiguous: found %d matches, picking one]\n", count);
        // Just keep the first one
        return 1;
    }
    
    return count;
}

bool parse_command(char* input, Command* cmd) {
    tokenize(input);
    if (num_tokens == 0) return false;

    cmd->prso_count = 0;
    cmd->prsi = NOTHING;

    // Implicit Verb: Direction
    for (int dir = O_NORTH; dir <= O_OUT; dir++) {
         if (word_matches_object(&objects[dir], tokens[0].word)) {
             if (num_tokens == 1) {
                 cmd->verb = V_WALK;
                 cmd->prso_list[0] = dir;
                 cmd->prso_count = 1;
                 cmd->prsi = NOTHING;
                 return true;
             }
         }
    }
    
    for (int i = 0; i < syntax_table_size; i++) {
        SyntaxEntry* se = &syntax_table[i];
        
        if (strcasecmp(tokens[0].word, se->verb_word) != 0) {
            if (tokens[0].vocab && tokens[0].vocab->type == VOCAB_SYNONYM) {
                 if (strcasecmp(tokens[0].vocab->target, se->verb_word) != 0) continue;
            } else {
                 continue;
            }
        }
        
        int input_prep1_idx = -1;
        int input_prep2_idx = -1;
        
        for (int k = 1; k < num_tokens; k++) {
             if (tokens[k].vocab && tokens[k].vocab->type == VOCAB_PREP) {
                 if (input_prep1_idx == -1) input_prep1_idx = k;
                 else if (input_prep2_idx == -1) input_prep2_idx = k;
             }
        }
        
        bool p1_match = false;
        if (se->prep1 == NULL) {
             if (input_prep1_idx == -1) p1_match = true;
        } else {
             if (input_prep1_idx != -1 && strcasecmp(tokens[input_prep1_idx].word, se->prep1) == 0) {
                 p1_match = true;
             }
        }
        
        bool p2_match = false;
        if (se->prep2 == NULL) {
             if (input_prep2_idx == -1) p2_match = true;
        } else {
             if (input_prep2_idx != -1 && strcasecmp(tokens[input_prep2_idx].word, se->prep2) == 0) {
                 p2_match = true;
             }
        }
        
        if (!p1_match || !p2_match) continue;

        int nc1_start = -1, nc1_end = -1;
        int nc2_start = -1, nc2_end = -1;
        int ptr = 1;
        
        if (se->obj1_present) {
             if (se->prep1_loc == PREP_LOC_BEFORE_OBJ1) {
                  if (input_prep1_idx != ptr) goto next_syntax;
                  ptr++; 
                  nc1_start = ptr;
                  if (se->prep2 != NULL) {
                       if (input_prep2_idx == -1 || input_prep2_idx <= ptr) goto next_syntax;
                       nc1_end = input_prep2_idx;
                       ptr = input_prep2_idx + 1; 
                  } else {
                       nc1_end = num_tokens;
                       ptr = num_tokens;
                  }
             } else if (se->prep1_loc == PREP_LOC_AFTER_OBJ1) {
                  nc1_start = ptr;
                  if (input_prep1_idx == -1 || input_prep1_idx <= ptr) goto next_syntax;
                  nc1_end = input_prep1_idx;
                  ptr = input_prep1_idx + 1;
             } else {
                  nc1_start = ptr;
                  if (se->prep2 != NULL) {
                       if (input_prep1_idx != -1) {
                           nc1_end = input_prep1_idx;
                           ptr = input_prep1_idx; 
                       }
                  } else {
                       nc1_end = num_tokens;
                       ptr = num_tokens;
                  }
             }
             
             if (nc1_start >= nc1_end) goto next_syntax;
        }
        
        if (se->obj2_present) {
             nc2_start = ptr;
             nc2_end = num_tokens;
             if (nc2_start >= nc2_end) goto next_syntax;
        }
        
        // Snarf Objects
        int count1 = 0;
        
        if (se->obj1_present) {
            count1 = snarf_objects(nc1_start, nc1_end, se->obj1_search, se->obj1_find, cmd->prso_list, MAX_OBJECTS_PER_CMD);
            if (count1 == 0) {
                 printf("You can't see any %s here.\n", tokens[nc1_start].word);
                 return false;
            }
            cmd->prso_count = count1;
        }
        
        if (se->obj2_present) {
            // Prsi
            ZObjectID prsi_list[2];
            int count2 = snarf_objects(nc2_start, nc2_end, se->obj2_search, se->obj2_find, prsi_list, 2);
            if (count2 == 0) {
                 printf("You can't see any %s here.\n", tokens[nc2_start].word);
                 return false;
            }
            cmd->prsi = prsi_list[0];
        }
        
        cmd->verb = se->action_id;
        return true;

        next_syntax: continue;
    }
    
    printf("I don't understand that sentence.\n");
    return false;
}
