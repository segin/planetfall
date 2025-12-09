#ifndef PARSER_H
#define PARSER_H

#include "planetfall.h"
#include "syntax_gen.h"

#define MAX_OBJECTS_PER_CMD 64

typedef struct {
    int verb; // ActionID from syntax_gen.h
    
    // Direct Object(s)
    ZObjectID prso_list[MAX_OBJECTS_PER_CMD];
    int prso_count;
    
    // Indirect Object (usually one)
    ZObjectID prsi;
    
    // Legacy support (to be removed or mapped to prso_list[0])
    // ZObjectID direct_object; 
    // ZObjectID indirect_object;
} Command;

// Global Command Context
extern Command current_cmd;

bool parse_command(char* input, Command* cmd);

#endif
