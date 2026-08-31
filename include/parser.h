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

  // Who the command was addressed to. NOTHING (or the player) when you are
  // simply acting yourself; an ACTORBIT object when you ordered someone about.
  ZObjectID winner;

  // Legacy support (to be removed or mapped to prso_list[0])
  // ZObjectID direct_object;
  // ZObjectID indirect_object;

  int parsed_number;
} Command;

// Global Command Context
extern Command current_cmd;

bool parse_command(char *input, Command *cmd);

#endif
