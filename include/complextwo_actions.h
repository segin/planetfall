#ifndef COMPLEXTWO_ACTIONS_H
#define COMPLEXTWO_ACTIONS_H

#include <stdbool.h>

// Room Actions
bool lawanda_platform_f(int arg);
bool infirmary_f(int arg);
bool robot_hole_f(int arg);
bool planetary_defense_f(int arg);
bool access_panel_f(int arg);
bool planetary_course_control_f(int arg);
bool cube_f(void);
bool terminal_f(void);
bool spool_reader_f(void);
bool projcon_office_f(int arg);
bool cryo_elevator_f(int arg);
bool cryo_anteroom_f(int arg);
bool bio_lock_east_f(int arg);
bool bio_lab_f(int arg);
bool radiation_lab_f(int arg);
bool lab_office_f(int arg);

// Object Actions
bool red_spool_f(void);
bool medicine_f(void);
bool good_board_f(void);
bool fried_board_f(void);
bool board_f(void);
bool bad_bedistor_f(int arg);
bool green_spool_f(void);
bool print_out_f(void);
bool mini_card_f(void);
bool bio_door_east_f(void);
bool bio_door_west_f(void);
bool rad_door_east_f(void);
bool rad_door_west_f(void);
bool lab_uniform_f(void);
bool combination_paper_f(void);
bool lab_desk_f(void);
bool light_button_f(void);
bool dark_button_f(void);
bool fungicide_button_f(void);
bool lamp_f(void);

// Events / Helpers
void floyd_through_hole(void);
bool cryo_exit_f(void);
void i_cryo_elevator_arrive(void);
void i_clear_floyd_peer(void);
void i_bio_east_closes(void);
void i_bio_west_closes(void);
void i_floyd_foray(void);
void monster_death(void);
void i_chase_scene(void);
void i_nuked_blue(void);
void i_unflood(void);
void i_turnoff_mini(void);

#endif // COMPLEXTWO_ACTIONS_H
