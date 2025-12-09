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

volatile sig_atomic_t need_repaint = 0;

void handle_winch(int sig) {
    need_repaint = 1;
}

void init_game_data() {
    init_game();
    
    // Player
    player = OBJ_PLAYER;
    objects[player].id = player;
    objects[player].description = "you";
    objects[player].synonyms[0] = "me";
    objects[player].synonyms[1] = "myself";
    objects[player].synonyms[2] = "you";
    objects[player].flags = F_ACTORBIT;
    
    // O_ALL
    ZObject* o = &objects[O_ALL];
    o->id = O_ALL;
    o->description = "all";
    o->synonyms[0] = "all";
    o->flags = F_INVISIBLE; // Don't show in lists
    obj_move(O_ALL, OBJ_GLOBAL_OBJECTS);

    // Initialize Feinstein Act Data
    init_feinstein_act();
    init_complexone();

    // Set starting room
    current_room = R_DECK_NINE;
    
    // Scrub Brush
    o = &objects[O_SCRUB_BRUSH];
    o->id = O_SCRUB_BRUSH;
    o->description = "Patrol-issue self-contained multi-purpose scrub brush";
    o->synonyms[0] = "brush";
    o->synonyms[1] = "scrub";
    o->flags = F_TAKEBIT;
    obj_move(O_SCRUB_BRUSH, player);
    
    // Patrol Uniform
    o = &objects[O_PATROL_UNIFORM];
    o->id = O_PATROL_UNIFORM;
    o->description = "Patrol uniform";
    o->synonyms[0] = "uniform";
    o->flags = F_TAKEBIT | F_WORNBIT | F_WEARBIT | F_CONTBIT | F_OPENBIT;
    obj_move(O_PATROL_UNIFORM, player);
    
    // ID Card
    o = &objects[O_ID_CARD];
    o->id = O_ID_CARD;
    o->description = "ID card";
    o->synonyms[0] = "card";
    o->flags = F_TAKEBIT | F_READBIT;
    o->text = "STELLAR PATROL - ID 6172-531-541";
    obj_move(O_ID_CARD, O_PATROL_UNIFORM);
    
    // Chronometer
    o = &objects[O_CHRONOMETER];
    o->id = O_CHRONOMETER;
    o->description = "chronometer";
    o->synonyms[0] = "chronometer";
    o->synonyms[1] = "watch";
    o->flags = F_TAKEBIT | F_WEARBIT | F_WORNBIT;
    obj_move(O_CHRONOMETER, player);
}

// Verb Routines

void perform_walk(ZObjectID direction_field) {
    if (direction_field == NOTHING) {
        printf("You can't go that way.\n");
        return;
    }
    if (direction_field == R_CERTAIN_DEATH_MSG) {
        printf("Certain death.\n");
        return;
    }
    obj_move(player, direction_field);
    current_room = direction_field;
    perform_look();
}

void perform_look() {
    ZObject* r = &objects[current_room];
    tell_ln(r->description);
    tell_ln(r->long_description);
    
    // List contents
    ZObjectID child = r->child;
    while (child != NOTHING) {
        ZObject* obj = &objects[child];
        if (child != player && !obj_has_flag(child, F_NDESCBIT) && !obj_has_flag(child, F_INVISIBLE)) {
            printf("There is a %s here.\n", obj->description);
        }
        child = obj->sibling;
    }
}

void perform_inventory() {
    printf("You are carrying:\n");
    ZObjectID child = objects[player].child;
    if (child == NOTHING) {
        printf("  Nothing.\n");
        return;
    }
    while (child != NOTHING) {
        ZObject* obj = &objects[child];
        printf("  %s", obj->description);
        if (obj_has_flag(child, F_WORNBIT)) printf(" (being worn)");
        if (obj_has_flag(child, F_OPENBIT) && obj_has_flag(child, F_CONTBIT)) {
             // Show contents (simple recursion)
             if (obj->child != NOTHING) {
                 printf("\n    containing:");
                 ZObjectID inner = obj->child;
                 while (inner != NOTHING) {
                     printf("\n      %s", objects[inner].description);
                     inner = objects[inner].sibling;
                 }
             }
        }
        printf("\n");
        child = obj->sibling;
    }
}

void perform_take(ZObjectID obj_id) {
    if (obj_id == NOTHING) return;
    
    if (obj_id == player) {
        printf("How romantic.\n");
        return;
    }
    
    if (obj_id == O_ALL) {
        // Take All logic
        ZObjectID child = objects[current_room].child;
        bool found_any = false;
        while (child != NOTHING) {
            ZObjectID next = objects[child].sibling;
            if (child != player && obj_has_flag(child, F_TAKEBIT) && !obj_has_flag(child, F_INVISIBLE)) {
                 printf("%s: Taken.\n", objects[child].description);
                 obj_move(child, player);
                 found_any = true;
            }
            child = next;
        }
        if (!found_any) printf("There is nothing here to take.\n");
        return;
    }

    if (obj_in(obj_id, player)) {
        printf("You already have that.\n");
        return;
    }
    if (!obj_has_flag(obj_id, F_TAKEBIT)) {
        printf("You can't take that.\n");
        return;
    }
    
    obj_move(obj_id, player);
    printf("Taken.\n");
}

void perform_drop(ZObjectID obj_id) {
    if (obj_id == NOTHING) return;
    if (!obj_in(obj_id, player)) {
        printf("You're not carrying that.\n");
        return;
    }
    
    obj_move(obj_id, current_room);
    printf("Dropped.\n");
}

void perform_examine(ZObjectID obj_id) {
    if (obj_id == NOTHING) return;
    // Check visibility
    
    ZObject* obj = &objects[obj_id];
    
    if (obj->long_description) {
        printf("%s\n", obj->long_description);
    } else {
        printf("%s\n", obj->description);
    }

    // If it has text
    if (obj_has_flag(obj_id, F_READBIT) && obj->text) {
        printf("It reads: %s\n", obj->text);
    }
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
    
    // Set scrolling region to 2..Height to protect status bar
    printf("\033[2;%dr", height);
    
    printf("\033[s"); // Save cursor
    printf("\033[H"); // Move to 0,0
    printf("\033[7m"); // Reverse video
    
    // Construct Status Line
    // Left: Location
    // Right: Time: N
    // Fill middle with spaces
    
    char right_text[32];
    snprintf(right_text, sizeof(right_text), "Time: %d", moves);
    
    int right_len = strlen(right_text);
    int loc_len = strlen(loc_name);
    
    // Time Calculation (Start at Day 1, 08:00)
    int start_hour = 8;
    int minutes_per_move = 10;
    int total_minutes = moves * minutes_per_move;
    int current_day = game_state.day + (total_minutes / 1440); // 1440 mins/day
    int daily_minutes = (start_hour * 60) + (total_minutes % 1440);
    
    if (daily_minutes >= 1440) {
        current_day++;
        daily_minutes -= 1440;
    }
    
    int hour = daily_minutes / 60;
    int minute = daily_minutes % 60;
    
    snprintf(right_text, sizeof(right_text), "Day %d, %02d:%02d", current_day, hour, minute);
    
    right_len = strlen(right_text);

    // Safe margin
    int space_len = width - 1 - loc_len - right_len - 2; // -2 for padding spaces
    if (space_len < 1) space_len = 1;
    
    printf(" %s", loc_name);
    for (int i=0; i<space_len; i++) putchar(' ');
    printf("%s ", right_text);
    
    // Clear rest of line if any weirdness (though we calculated width)
    // Actually we just printed exactly width characters (approx).
    
    printf("\033[0m"); // Reset
    printf("\033[u"); // Restore cursor
    fflush(stdout);
    
    need_repaint = 0;
}

int main(int argc, char **argv) {
    // Setup Signal Handler for Resize
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_winch;
    sigaction(SIGWINCH, &sa, NULL);

    // Parse args
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--no-status") == 0) {
            show_status = false;
        }
    }

    init_game_data();
    
    if (show_status) {
        printf("\033[2J"); // Clear screen
        printf("\033[H");  // Home
    }
    
    printf("Planetfall\n");
    printf("INTERLOGIC Science Fiction Phonytasy\n");
    printf("Copyright (c) 1983 Infocom, Inc. All rights reserved.\n\n");
    
    update_status_bar();
    perform_look();
    
    char input[256];
    Command cmd;
    
    while (1) {
        if (need_repaint) update_status_bar();
        
        printf("\n> ");
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        
        if (need_repaint) update_status_bar(); // Check again after input return (resize during input)
        
        if (!parse_command(input, &cmd)) continue;
        
        // Handle "Enter [Object]" -> Board
        if (cmd.verb == V_WALK_IN && cmd.direct_object != NOTHING) {
            cmd.verb = V_BOARD;
        }
        // Handle "Exit [Object]" or "Out [Object]" -> Disembark
        if (cmd.verb == V_WALK_OUT && cmd.direct_object != NOTHING) {
            cmd.verb = V_DISEMBARK;
        }

        switch (cmd.verb) {
            case V_QUIT:
                printf("\033[r"); // Reset scrolling region
                return 0;
            case V_LOOK:
                perform_look();
                break;
            case V_INVENTORY:
                perform_inventory();
                break;
            case V_TAKE:
                perform_take(cmd.direct_object);
                break;
            case V_DROP:
                perform_drop(cmd.direct_object);
                break;
            case V_EXAMINE:
                perform_examine(cmd.direct_object);
                break;
            case V_PUT:
                // perform_put(cmd.direct_object, cmd.indirect_object);
                // Placeholder
                if (cmd.direct_object != NOTHING && cmd.indirect_object != NOTHING) {
                    if (obj_in(cmd.direct_object, player)) {
                         if (obj_has_flag(cmd.indirect_object, F_CONTBIT) && obj_has_flag(cmd.indirect_object, F_OPENBIT)) {
                             obj_move(cmd.direct_object, cmd.indirect_object);
                             printf("Done.\n");
                         } else {
                             printf("You can't put things in that.\n");
                         }
                    } else {
                        printf("You aren't holding that.\n");
                    }
                } else {
                    printf("Put what in what?\n");
                }
                break;
            case V_WAIT:
                printf("Time passes...\n");
                break;
            case V_BOARD:
                if (cmd.direct_object == NOTHING) {
                    printf("Board what?\n");
                } else if (obj_has_flag(cmd.direct_object, F_VEHBIT)) {
                    // Logic for entering vehicles
                    // Simplified: just move player inside
                    if (cmd.direct_object == O_SAFETY_WEB && current_room == R_ESCAPE_POD) {
                        obj_move(player, O_SAFETY_WEB);
                        printf("You are now safely cushioned within the web.\n");
                    } else if ((cmd.direct_object == O_GLOBAL_POD || cmd.direct_object == R_ESCAPE_POD) && (current_room == R_DECK_NINE || current_room == R_CRAG)) {
                        // Enter pod
                        obj_move(player, R_ESCAPE_POD); // Technically player isn't IN the room object in standard ZIL but logically location is room
                        current_room = R_ESCAPE_POD;
                        perform_look();
                    } else {
                        printf("You can't get in that.\n");
                    }
                } else {
                    printf("You can't board that.\n");
                }
                break;
            case V_DISEMBARK:
                // printf("DEBUG: V_DISEMBARK. Current Room: %d. Out Exit: %d\n", current_room, objects[current_room].out);
                if (obj_in(player, O_SAFETY_WEB)) {
                    obj_move(player, current_room);
                    printf("You are standing again.\n");
                } else if (objects[current_room].out != NOTHING) {
                    // Use defined 'out' exit
                    ZObjectID dest = objects[current_room].out;
                    obj_move(player, dest);
                    current_room = dest;
                    perform_look();
                } else {
                     printf("You aren't in anything you can get out of here.\n");
                }
                break;
            case V_OPEN:
                if (cmd.direct_object == NOTHING) {
                    printf("Open what?\n");
                } else {
                    if (obj_has_flag(cmd.direct_object, F_OPENBIT)) {
                        printf("It is already open.\n");
                    } else if (obj_has_flag(cmd.direct_object, F_DOORBIT) || obj_has_flag(cmd.direct_object, F_CONTBIT)) {
                        obj_set_flag(cmd.direct_object, F_OPENBIT);
                        printf("Opened.\n");
                        // Special triggers
                        if (cmd.direct_object == O_POD_DOOR && current_room == R_ESCAPE_POD) {
                            if (trip_counter > 0) {
                                printf("The bulkhead opens and cold ocean water rushes in! You drown.\n");
                                // Death logic
                            } else {
                                // Normal open on ship
                            }
                        }
                    } else {
                        printf("You can't open that.\n");
                    }
                }
                break;
            case V_CLOSE:
                if (cmd.direct_object == NOTHING) {
                    printf("Close what?\n");
                } else {
                    if (!obj_has_flag(cmd.direct_object, F_OPENBIT)) {
                         printf("It is already closed.\n");
                    } else if (obj_has_flag(cmd.direct_object, F_DOORBIT) || obj_has_flag(cmd.direct_object, F_CONTBIT)) {
                         obj_clear_flag(cmd.direct_object, F_OPENBIT);
                         printf("Closed.\n");
                         
                         // Pod Launch Trigger
                         if (cmd.direct_object == O_POD_DOOR && current_room == R_ESCAPE_POD) {
                             if (!is_event_enabled(EVT_POD_TRIP)) {
                                 queue_event(EVT_POD_TRIP, -1); // Start trip (Daemon)
                                 printf("\nThe pod door clangs shut and the pod begins to vibrate.\n");
                             }
                         }
                    } else {
                         printf("You can't close that.\n");
                    }
                }
                break;
            case V_SCRUB:
                if (cmd.direct_object == NOTHING) {
                    printf("Scrub what?\n");
                } else if (cmd.direct_object == O_BLATHER) {
                    printf("Blather isn't dirty, though his personality is filthy.\n");
                } else if (cmd.direct_object == O_AMBASSADOR) {
                    printf("You've cleaned up maybe one ten-thousandth of the slime.\n");
                } else {
                    printf("You scrub furiously. Nothing much happens.\n");
                    if (obj_in(O_BLATHER, current_room)) {
                        printf("\nBlather sneers. 'Missed a spot, Ensign!'\n");
                    }
                }
                break;
            case V_ATTACK:
            case V_KICK:
                if (cmd.direct_object == O_BLATHER) {
                    printf("\nBlather removes several of your appendages and internal organs. You die.\n");
                    return 0; // End game
                } else if (cmd.direct_object == O_AMBASSADOR) {
                    printf("\nThe ambassador is startled, and emits an amazing quantity of slime which spreads across the section of the deck you just polished.\n");
                } else {
                    printf("Violence isn't the answer.\n");
                }
                break;
            case V_TALK:
                if (cmd.direct_object == O_BLATHER) {
                    printf("\nBlather shouts \"Speak when you're spoken to, Ensign Seventh Class!\" He breaks three pencil points in a frenzied rush to give you more demerits.\n");
                    brigs_up++; // Annoying Blather increases Brig count
                } else if (cmd.direct_object == O_AMBASSADOR) {
                    printf("\nThe ambassador taps his translator, and then touches his center knee to his left ear (the Blow'k-bibben-Gordoan equivalent of shrugging).\n");
                } else {
                    printf("Talking to yourself?\n");
                }
                break;
            case V_EAT:
                if (cmd.direct_object == NOTHING) {
                    printf("Eat what?\n");
                } else if (cmd.direct_object == O_CELERY) {
                    printf("\nOops. Looks like Blow'k-Bibben-Gordoan metabolism is not compatible with our own. You die of all sorts of convulsions.\n");
                    return 0;
                } else if (obj_has_flag(cmd.direct_object, F_FOODBIT)) {
                    if (obj_in(cmd.direct_object, player)) {
                        printf("Delicious.\n");
                        obj_remove(cmd.direct_object); // Consumed
                        game_state.hunger_level = 0; // Reset hunger
                    } else {
                        printf("You don't have that.\n");
                    }
                } else {
                    printf("You can't eat that.\n");
                }
                break;
            case V_SMELL:
                if (cmd.direct_object == O_AMBASSADOR) { // or slime if implemented
                    printf("It smells like slime.\n");
                } else {
                    printf("It smells like nothing special.\n");
                }
                break;
            case V_READ:
                if (cmd.direct_object == O_TOWEL) {
                    printf("%s\n", objects[O_TOWEL].text);
                } else if (cmd.direct_object == O_BROCHURE) {
                    printf("%s\n", objects[O_BROCHURE].text);
                } else if (cmd.direct_object == O_ID_CARD) {
                    printf("%s\n", objects[O_ID_CARD].text);
                } else if (current_room == R_BRIG) {
                    printf("All the graffiti seem to be about Blather. One reads:\n'There once was a krip, name of Blather...'\nIt's not a very good limerick.\n");
                } else {
                    printf("You can't read that.\n");
                }
                break;
            case V_REMOVE:
                if (cmd.direct_object == O_PATROL_UNIFORM) {
                     if (obj_in(O_PATROL_UNIFORM, player) && obj_has_flag(O_PATROL_UNIFORM, F_WORNBIT)) {
                         obj_clear_flag(O_PATROL_UNIFORM, F_WORNBIT);
                         printf("You have removed your Patrol uniform.\n");
                         if (obj_in(O_BLATHER, current_room)) {
                             printf("\n'Removing your uniform while on duty? Five hundred demerits!'\n");
                         }
                     } else {
                         printf("You aren't wearing it.\n");
                     }
                } else {
                    printf("You can't remove that.\n");
                }
                break;
            case V_WEAR:
                if (cmd.direct_object == O_PATROL_UNIFORM) {
                    if (obj_in(O_PATROL_UNIFORM, player) && !obj_has_flag(O_PATROL_UNIFORM, F_WORNBIT)) {
                         obj_set_flag(O_PATROL_UNIFORM, F_WORNBIT);
                         printf("You are now wearing the Patrol uniform.\n");
                    } else {
                        printf("You can't wear that.\n");
                    }
                }
                break;
            case V_WALK_NORTH: perform_walk(objects[current_room].north); break;
            case V_WALK_SOUTH: perform_walk(objects[current_room].south); break;
            case V_WALK_EAST: perform_walk(objects[current_room].east); break;
            case V_WALK_WEST: perform_walk(objects[current_room].west); break;
            case V_WALK_NE: perform_walk(objects[current_room].ne); break;
            case V_WALK_NW: perform_walk(objects[current_room].nw); break;
            case V_WALK_SE: perform_walk(objects[current_room].se); break;
            case V_WALK_SW: perform_walk(objects[current_room].sw); break;
            case V_WALK_UP: perform_walk(objects[current_room].up); break;
            case V_WALK_DOWN: perform_walk(objects[current_room].down); break;
            case V_WALK_IN: perform_walk(objects[current_room].in); break;
            case V_WALK_OUT: perform_walk(objects[current_room].out); break;
            default:
                printf("That verb is not implemented yet.\n");
        }
        
        // Daemons/Timers
        game_state.internal_moves++;
        if (!run_events()) {
            break;
        }
        
        // Check for immediate death rooms
        if (current_room == R_UNDERWATER) {
             printf("\nA mighty undertow drags you across some underwater obstructions.\n");
             printf("**** You have died ****\n");
             return 0;
        }

        update_status_bar();
    }
    
    return 0;
}
