#ifndef COMPLEXONE_ACTIONS_H
#define COMPLEXONE_ACTIONS_H

#include "actions.h"

// Room Actions
bool underwater_f(int arg);
bool crag_f(int arg);
bool balcony_f(int arg);
bool winding_stair_f(int arg);
bool courtyard_f(int arg);
bool rec_area_f(int arg);
bool conference_room_f(int arg);
bool elevator_lobby_f(int arg);
bool upper_elevator_f(int arg);
bool lower_elevator_f(int arg);
bool machine_shop_f(int arg);
bool mess_corridor_f(int arg);
bool mess_hall_f(int arg);
bool admin_corridor_f(int arg);
bool admin_corridor_n_f(int arg);
bool admin_corridor_s_f(int arg);
bool kalamontee_platform_f(int arg);
bool comm_room_f(int arg);
bool systems_monitors_f(int arg);

// Navigation & Water level
ZObjectID water_level_f(void);

// Object / Pseudo Actions
bool combination_dial_f(int arg);
bool conference_door_f(int arg);
bool storage_west_door_f(int arg);
bool padlock_f(int arg);
bool chemical_dispenser_f(int arg);
bool chem_button_f(int arg);
bool floyd_f(int arg);
bool structure_pseudo_action(int arg);
bool cleft_pseudo_action(int arg);
bool plaque_pseudo_action(int arg);
bool castle_pseudo_action(int arg);
bool rubble_pseudo_action(int arg);
bool games_pseudo_action(int arg);
bool tapes_pseudo_action(int arg);
bool near_booth_pseudo_action(int arg);
bool in_booth_pseudo_action(int arg);
bool partition_pseudo_action(int arg);
bool toilet_pseudo_action(int arg);
// Add more as needed

#endif
