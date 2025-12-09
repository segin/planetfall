#ifndef ACTIONS_H
#define ACTIONS_H

#include "planetfall.h"
#include "syntax_gen.h"

// Core Dispatch
bool dispatch_action(int verb, ZObjectID prso, ZObjectID prsi);

// Verbs
void perform_look();
void perform_inventory();
void perform_take(ZObjectID obj);
void perform_drop(ZObjectID obj);
void perform_examine(ZObjectID obj);
void perform_save();
void perform_restore();
void perform_restart();
void perform_script();
void perform_unscript();

void jigs_up(const char* msg);

#endif
