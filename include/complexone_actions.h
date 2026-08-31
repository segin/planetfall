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

bool kitchen_f(int arg);

// Navigation & Water level
ZObjectID water_level_f(void);
ZObjectID long_hall_f(void);
ZObjectID ladder_exit_f(void);

// Object / Pseudo Actions
bool can_f(int arg);
bool ladder_f(int arg);
bool walkway_pseudo_action(int arg);
bool bench_pseudo_action(int arg);
bool kitchen_door_f(int arg);
bool dispenser_f(int arg);
bool canteen_f(int arg);
bool high_protein_f(int arg);
bool kitchen_button_pseudo_action(int arg);
bool spout_pseudo_action(int arg);
bool crevice_f(int arg);
bool key_f(int arg);
bool rift_f(int arg);
bool combination_dial_f(int arg);
bool conference_door_f(int arg);
bool storage_west_door_f(int arg);
bool padlock_f(int arg);
bool chemical_dispenser_f(int arg);
bool chem_button_f(int arg);
bool floyd_f(int arg);
void floyd_comes_alive(void);
void computer_action(void);
void i_floyd(void);
bool structure_pseudo_action(int arg);
bool cleft_pseudo_action(int arg);
bool plaque_pseudo_action(int arg);
bool castle_pseudo_action(int arg);
bool rubble_pseudo_action(int arg);
bool games_pseudo_action(int arg);
bool tapes_pseudo_action(int arg);
bool near_booth_pseudo_action(int arg);
bool in_booth_pseudo_action(int arg);
void describe_monitors(void);
bool monitors_pseudo_action(int arg);
bool equipment_pseudo_action(int arg);
bool cubbyhole_pseudo_action(int arg);
bool maps_pseudo_action(int arg);
bool oil_can_f(int arg);
bool carton_f(int arg);
bool cracked_board_f(int arg);
void examine_board(void);
bool good_bedistor_f(int arg);
bool catwalk_pseudo_action(int arg);
bool desk_f(int arg);
bool reactor_control_f(int arg);
bool reactor_elevator_f(int arg);
bool reactor_elevator_door_f(int arg);
bool reactor_button_pseudo_action(int arg);
bool diagram_pseudo_action(int arg);
bool flask_f(int arg);
bool magnet_f(int arg);
void i_magnet(void);
void i_reactor_door_close(void);
bool partition_pseudo_action(int arg);
bool toilet_pseudo_action(int arg);
bool chem_spout_pseudo_action(int arg);
bool devices_pseudo_action(int arg);
bool upper_elevator_door_f(int arg);
bool lower_elevator_door_f(int arg);
bool blue_elevator_button_f(int arg);
bool red_elevator_button_f(int arg);
bool elevator_button_f(int arg);
bool slot_f(int arg);
void i_upper_elevator_arrive(void);
void i_lower_elevator_arrive(void);
void i_upper_elevator_trip(void);
void i_lower_elevator_trip(void);
void i_turnoff_upper_elevator(void);
void i_turnoff_lower_elevator(void);
bool helicopter_object_f(int arg);
bool fence_pseudo_action(int arg);
bool lock_pseudo_action(int arg);
bool cables_pseudo_action(int arg);
bool enunciator_pseudo_action(int arg);
bool playback_button_f(int arg);
bool chemical_fluid_f(int arg);
void i_unenter(void);
// Add more as needed

#endif
