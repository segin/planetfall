#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include "planetfall.h"
#include "parser.h"
#include "events.h"
#include "feinstein.h"
#include "complexone.h"
#include "actions.h"
#include "output.h"

Command current_cmd;
volatile sig_atomic_t need_repaint = 0;

void handle_winch(int sig) {
    need_repaint = 1;
}

void init_direction(ZObjectID id, const char* name, const char* syn1, const char* syn2) {
    ZObject* o = &objects[id];
    o->id = id;
    o->description = name;
    o->synonyms[0] = syn1;
    o->synonyms[1] = syn2;
    o->flags = 0; 
    obj_move(id, OBJ_GLOBAL_OBJECTS);
}

void init_game_data() {
    init_game();
    
    player = OBJ_PLAYER;
    objects[player].id = player;
    objects[player].description = "you";
    objects[player].synonyms[0] = "me";
    objects[player].synonyms[1] = "myself";
    objects[player].synonyms[2] = "you";
    objects[player].flags = F_ACTORBIT;
    
    ZObject* o = &objects[O_ALL];
    o->id = O_ALL;
    o->description = "all";
    o->synonyms[0] = "all";
    o->flags = F_INVISIBLE; 
    obj_move(O_ALL, OBJ_GLOBAL_OBJECTS);

    init_direction(O_NORTH, "north", "north", "n");
    init_direction(O_SOUTH, "south", "south", "s");
    init_direction(O_EAST, "east", "east", "e");
    init_direction(O_WEST, "west", "west", "w");
    init_direction(O_NE, "northeast", "ne", NULL);
    init_direction(O_NW, "northwest", "nw", NULL);
    init_direction(O_SE, "southeast", "se", NULL);
    init_direction(O_SW, "southwest", "sw", NULL);
    init_direction(O_UP, "up", "up", "u");
    init_direction(O_DOWN, "down", "down", "d");
    init_direction(O_IN, "in", "in", NULL);
    init_direction(O_OUT, "out", "out", NULL);

    init_feinstein_act();
    init_complexone();

    current_room = R_DECK_NINE;
    
    o = &objects[O_SCRUB_BRUSH];
    o->id = O_SCRUB_BRUSH;
    o->description = "Patrol-issue self-contained multi-purpose scrub brush";
    o->synonyms[0] = "brush";
    o->synonyms[1] = "scrub";
    o->flags = F_TAKEBIT;
    obj_move(O_SCRUB_BRUSH, player);
    
    o = &objects[O_PATROL_UNIFORM];
    o->id = O_PATROL_UNIFORM;
    o->description = "Patrol uniform";
    o->synonyms[0] = "uniform";
    o->flags = F_TAKEBIT | F_WORNBIT | F_WEARBIT | F_CONTBIT | F_OPENBIT;
    obj_move(O_PATROL_UNIFORM, player);
    
    o = &objects[O_ID_CARD];
    o->id = O_ID_CARD;
    o->description = "ID card";
    o->synonyms[0] = "card";
    o->flags = F_TAKEBIT | F_READBIT;
    o->text = "STELLAR PATROL - ID 6172-531-541";
    obj_move(O_ID_CARD, O_PATROL_UNIFORM);
    
    o = &objects[O_CHRONOMETER];
    o->id = O_CHRONOMETER;
    o->description = "chronometer";
    o->synonyms[0] = "chronometer";
    o->synonyms[1] = "watch";
    o->flags = F_TAKEBIT | F_WEARBIT | F_WORNBIT;
    obj_move(O_CHRONOMETER, player);
}

void perform_walk_dir(ZObjectID direction_field) {
    if (direction_field == NOTHING) {
        tellf("You can't go that way.\n");
        return;
    }
    if (direction_field == R_CERTAIN_DEATH_MSG) {
        tellf("Certain death.\n");
        return;
    }
    obj_move(player, direction_field);
    current_room = direction_field;
    perform_look();
}

bool show_status = true;

void update_status_bar() {
    if (!show_status) return;
    
    struct winsize w;
    memset(&w, 0, sizeof(w));
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        w.ws_col = 80;
        w.ws_row = 24;
    }
    int width = w.ws_col;
    if (width == 0) width = 80;
    
    ZObject* room = &objects[current_room];
    const char* loc_name = room->description ? room->description : "Unknown";
    int moves = game_state.internal_moves;
    
    int height = w.ws_row;
    if (height == 0) height = 24;
    
    // Direct printf for status bar (bypass script)
    printf("\033[2;%dr", height);
    printf("\033[s"); 
    printf("\033[H"); 
    printf("\033[7m"); 
    
    char right_text[32];
    int start_hour = 8;
    int minutes_per_move = 10;
    int total_minutes = moves * minutes_per_move;
    int current_day = game_state.day + (total_minutes / 1440); 
    int daily_minutes = (start_hour * 60) + (total_minutes % 1440);
    
    if (daily_minutes >= 1440) {
        current_day++;
        daily_minutes -= 1440;
    }
    
    int hour = daily_minutes / 60;
    int minute = daily_minutes % 60;
    
    snprintf(right_text, sizeof(right_text), "Day %d, %02d:%02d", current_day, hour, minute);
    
    int right_len = strlen(right_text);
    int loc_len = strlen(loc_name);
    
    int space_len = width - 1 - loc_len - right_len - 2; 
    if (space_len < 1) space_len = 1;
    
    printf(" %s", loc_name);
    for (int i=0; i<space_len; i++) putchar(' ');
    printf("%s ", right_text);
    
    printf("\033[0m"); 
    printf("\033[u"); 
    fflush(stdout);
    
    need_repaint = 0;
}

int main(int argc, char **argv) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_winch;
    sigaction(SIGWINCH, &sa, NULL);

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--no-status") == 0) {
            show_status = false;
        }
    }

    init_game_data();
    
    if (show_status) {
        printf("\033[2J"); 
        printf("\033[H"); 
    }
    
    tellf("Planetfall\n");
    tellf("INTERLOGIC Science Fiction Phonytasy\n");
    tellf("Copyright (c) 1983 Infocom, Inc. All rights reserved.\n\n");
    
    update_status_bar();
    perform_look();
    
    char input[256];
    
    while (game_running) {
        if (need_repaint) update_status_bar();
        
        printf("\n> "); // Prompt (not scripted)
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        
        if (is_scripting()) {
             // Echo input to script
             tellf("\n> %s", input); 
        }
        
        if (need_repaint) update_status_bar(); 
        
        if (!parse_command(input, &current_cmd)) continue;
        
        if (current_cmd.verb == V_THROUGH) {
             if (current_cmd.prso_count > 0 && obj_has_flag(current_cmd.prso_list[0], F_VEHBIT)) {
                 current_cmd.verb = V_BOARD;
             }
        }

        int count = current_cmd.prso_count;
        if (count == 0) {
            if (!dispatch_action(current_cmd.verb, NOTHING, current_cmd.prsi)) {
                 goto legacy_dispatch;
            }
        } else {
            for (int i = 0; i < count; i++) {
                ZObjectID prso = current_cmd.prso_list[i];
                if (count > 1) {
                    tellf("%s: ", objects[prso].description); 
                }
                
                if (!dispatch_action(current_cmd.verb, prso, current_cmd.prsi)) {
                     if (i == 0) goto legacy_dispatch;
                }
            }
        }
        
        goto end_turn;

        legacy_dispatch:
        ZObjectID prso = (current_cmd.prso_count > 0) ? current_cmd.prso_list[0] : NOTHING;
        
        switch (current_cmd.verb) {
            case V_QUIT:
                game_running = false;
                break;
            case V_PUT:
                tellf("Put not fully implemented.\n");
                break;
            case V_WAIT:
                tellf("Time passes...\n");
                break;
            case V_BOARD:
                if (prso == NOTHING) {
                    tellf("Board what?\n");
                } else if (obj_has_flag(prso, F_VEHBIT)) {
                    if (prso == O_SAFETY_WEB && current_room == R_ESCAPE_POD) {
                        obj_move(player, O_SAFETY_WEB);
                        tellf("You are now safely cushioned within the web.\n");
                    } else if ((prso == O_GLOBAL_POD || prso == R_ESCAPE_POD) && (current_room == R_DECK_NINE || current_room == R_CRAG)) {
                        if (current_room == R_DECK_NINE && !obj_has_flag(O_POD_DOOR, F_OPENBIT)) {
                             tellf("The escape pod bulkhead is closed.\n");
                        } else {
                             obj_move(player, R_ESCAPE_POD); 
                             current_room = R_ESCAPE_POD;
                             perform_look();
                        }
                    } else {
                        tellf("You can't get in that.\n");
                    }
                } else {
                    tellf("You can't board that.\n");
                }
                break;
            case V_DISEMBARK:
                if (obj_in(player, O_SAFETY_WEB)) {
                    obj_move(player, current_room);
                    tellf("You are standing again.\n");
                } else if (objects[current_room].out != NOTHING) {
                    ZObjectID dest = objects[current_room].out;
                    obj_move(player, dest);
                    current_room = dest;
                    perform_look();
                } else {
                     tellf("You aren't in anything you can get out of here.\n");
                }
                break;
            case V_OPEN:
                if (prso == NOTHING) {
                    tellf("Open what?\n");
                } else {
                    if (obj_has_flag(prso, F_OPENBIT)) {
                        tellf("It is already open.\n");
                    } else if (obj_has_flag(prso, F_DOORBIT) || obj_has_flag(prso, F_CONTBIT)) {
                        obj_set_flag(prso, F_OPENBIT);
                        tellf("Opened.\n");
                        if (prso == O_POD_DOOR && current_room == R_ESCAPE_POD) {
                            if (game_state.trip_counter > 0) {
                                tellf("The bulkhead opens and cold ocean water rushes in! You drown.\n");
                            } 
                        }
                    } else {
                        tellf("You can't open that.\n");
                    }
                }
                break;
            case V_CLOSE:
                if (prso == NOTHING) {
                    tellf("Close what?\n");
                } else {
                    if (!obj_has_flag(prso, F_OPENBIT)) {
                         tellf("It is already closed.\n");
                    } else if (obj_has_flag(prso, F_DOORBIT) || obj_has_flag(prso, F_CONTBIT)) {
                         obj_clear_flag(prso, F_OPENBIT);
                         tellf("Closed.\n");
                         if (prso == O_POD_DOOR && current_room == R_ESCAPE_POD) {
                             if (!is_event_enabled(EVT_POD_TRIP)) {
                                 queue_event(EVT_POD_TRIP, -1); 
                                 tellf("\nThe pod door clangs shut and the pod begins to vibrate.\n");
                             }
                         }
                    } else {
                         tellf("You can't close that.\n");
                    }
                }
                break;
            case V_SCRUB:
                if (prso == NOTHING) {
                    tellf("Scrub what?\n");
                } else if (prso == O_BLATHER) {
                    tellf("Blather isn't dirty, though his personality is filthy.\n");
                } else if (prso == O_AMBASSADOR) {
                    tellf("You've cleaned up maybe one ten-thousandth of the slime.\n");
                } else {
                    tellf("You scrub furiously. Nothing much happens.\n");
                    if (obj_in(O_BLATHER, current_room)) {
                        tellf("\nBlather sneers. 'Missed a spot, Ensign!'\n");
                    }
                }
                break;
            case V_ATTACK:
            case V_KICK:
                if (prso == O_BLATHER) {
                    tellf("\nBlather removes several of your appendages and internal organs. You die.\n");
                    game_running = false; 
                } else if (prso == O_AMBASSADOR) {
                    tellf("\nThe ambassador is startled, and emits an amazing quantity of slime which spreads across the section of the deck you just polished.\n");
                } else {
                    tellf("Violence isn't the answer.\n");
                }
                break;
            case V_TALK:
                if (prso == O_BLATHER) {
                    tellf("\nBlather shouts \"Speak when you're spoken to, Ensign Seventh Class!\" He breaks three pencil points in a frenzied rush to give you more demerits.\n");
                    game_state.brigs_up++; 
                } else if (prso == O_AMBASSADOR) {
                    tellf("\nThe ambassador taps his translator, and then touches his center knee to his left ear (the Blow'k-bibben-Gordoan equivalent of shrugging).\n");
                } else {
                    tellf("Talking to yourself?\n");
                }
                break;
            case V_EAT:
                if (prso == NOTHING) {
                    tellf("Eat what?\n");
                } else if (prso == O_CELERY) {
                    tellf("\nOops. Looks like Blow'k-Bibben-Gordoan metabolism is not compatible with our own. You die of all sorts of convulsions.\n");
                    game_running = false;
                } else if (obj_has_flag(prso, F_FOODBIT)) {
                    if (obj_in(prso, player)) {
                        tellf("Delicious.\n");
                        obj_remove(prso); 
                        game_state.hunger_level = 0; 
                    } else {
                        tellf("You don't have that.\n");
                    }
                } else {
                    tellf("You can't eat that.\n");
                }
                break;
            case V_SMELL:
                if (prso == O_AMBASSADOR) { 
                    tellf("It smells like slime.\n");
                } else {
                    tellf("It smells like nothing special.\n");
                }
                break;
            case V_READ:
                if (prso == O_TOWEL) {
                    tellf("%s\n", objects[O_TOWEL].text);
                } else if (prso == O_BROCHURE) {
                    tellf("%s\n", objects[O_BROCHURE].text);
                } else if (prso == O_ID_CARD) {
                    tellf("%s\n", objects[O_ID_CARD].text);
                } else if (current_room == R_BRIG) {
                    tellf("All the graffiti seem to be about Blather. One reads:\n'There once was a krip, name of Blather...'\nIt's not a very good limerick.\n");
                } else {
                    tellf("You can't read that.\n");
                }
                break;
            case V_REMOVE:
                if (prso == O_PATROL_UNIFORM) {
                     if (obj_in(O_PATROL_UNIFORM, player) && obj_has_flag(O_PATROL_UNIFORM, F_WORNBIT)) {
                         obj_clear_flag(O_PATROL_UNIFORM, F_WORNBIT);
                         tellf("You have removed your Patrol uniform.\n");
                         if (obj_in(O_BLATHER, current_room)) {
                             tellf("\n'Removing your uniform while on duty? Five hundred demerits!'\n");
                         }
                     } else {
                         tellf("You aren't wearing it.\n");
                     }
                } else {
                    tellf("You can't remove that.\n");
                }
                break;
            case V_WEAR:
                if (prso == O_PATROL_UNIFORM) {
                    if (obj_in(O_PATROL_UNIFORM, player) && !obj_has_flag(O_PATROL_UNIFORM, F_WORNBIT)) {
                         obj_set_flag(O_PATROL_UNIFORM, F_WORNBIT);
                         tellf("You are now wearing the Patrol uniform.\n");
                    } else {
                        tellf("You can't wear that.\n");
                    }
                }
                break;
            case V_WALK: {
                ZObjectID dir = prso;
                if (dir == NOTHING) { tellf("Walk where?\n"); break; }
                ZObjectID dest = NOTHING;
                switch (dir) {
                    case O_NORTH: dest = objects[current_room].north; break;
                    case O_SOUTH: dest = objects[current_room].south; break;
                    case O_EAST: dest = objects[current_room].east; break;
                    case O_WEST: dest = objects[current_room].west; break;
                    case O_NE: dest = objects[current_room].ne; break;
                    case O_NW: dest = objects[current_room].nw; break;
                    case O_SE: dest = objects[current_room].se; break;
                    case O_SW: dest = objects[current_room].sw; break;
                    case O_UP: dest = objects[current_room].up; break;
                    case O_DOWN: dest = objects[current_room].down; break;
                    case O_IN: dest = objects[current_room].in; break;
                    case O_OUT: dest = objects[current_room].out; break;
                    default: tellf("You can't walk there.\n"); break;
                }
                if (dest != NOTHING) perform_walk_dir(dest);
                break;
            }
            default:
                tellf("That verb is not implemented yet.\n");
        }
        
        end_turn:
        game_state.internal_moves++;
        if (!run_events()) {
            break;
        }
        
        if (current_room == R_UNDERWATER) {
             tellf("\nA mighty undertow drags you across some underwater obstructions.\n");
             tellf("**** You have died ****\n");
             game_running = false;
        }

        update_status_bar();
    }
    
    printf("\033[r"); // Reset scrolling
    return 0;
}
