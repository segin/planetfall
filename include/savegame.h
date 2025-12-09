#ifndef SAVEGAME_H
#define SAVEGAME_H

#include <stdbool.h>

bool save_game(const char* filename);
bool restore_game(const char* filename);

#endif
