#!/bin/bash
# Feinstein Chapter Unit Tests

echo "=== Feinstein Chapter Verification ==="

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/.."

FAILED=0

# Helper to strip ANSI codes
strip_ansi() {
    sed 's/\x1b\[[0-9;]*m//g' | sed 's/\x1b\[[0-9]*[A-Za-z]//g' | sed 's/\x1b\[[^m]*m//g' | tr -d '[:cntrl:]'
}

run_test() {
    local name="$1"
    local input="$2"
    local pattern="$3"
    local pass_msg="$4"
    local fail_msg="$5"

    echo "$name"
    OUTPUT=$(echo -e "$input" | ./planetfall 2>/dev/null | strip_ansi)
    if echo "$OUTPUT" | grep -qiE "$pattern"; then
        echo "PASS: $pass_msg"
    else
        echo "FAIL: $fail_msg"
        FAILED=$((FAILED + 1))
    fi
}

run_test "Test 1: Room navigation (UP from Deck Nine)" "UP\nQUIT\nY" "Gangway" "UP leads to Gangway" "UP navigation"
run_test "Test 2: Port direction (should go west to pod if open)" "PORT\nQUIT\nY" "pod|closed|can't" "Port direction works" "Port direction"
run_test "Test 3: Examine ME" "EXAMINE ME\nQUIT\nY" "cretin|nothing special|special" "Examine ME works" "Examine ME"
run_test "Test 4: Look command" "LOOK\nQUIT\nY" "Deck Nine" "Look works" "Look"
run_test "Test 5: Inventory" "INVENTORY\nQUIT\nY" "carrying|uniform|chronometer|brush" "Inventory works" "Inventory"
run_test "Test 6: Wait for events" "WAIT\nWAIT\nWAIT\nWAIT\nWAIT\nQUIT\nY" "ambassador|Blather|explosion" "Events trigger" "Events"
run_test "Test 7: Scrub brush exists" "I\nQUIT\nY" "brush" "Scrub brush in inventory" "Scrub brush missing"
run_test "Test 8: Examine chronometer" "EXAMINE CHRONOMETER\nQUIT\nY" "wrist|digital|time|chronometer" "Chronometer examine works" "Chronometer"
run_test "Test 9: VERBOSE command" "VERBOSE\nQUIT\nY" "Maximum verbosity" "VERBOSE works" "VERBOSE command"
run_test "Test 10: BRIEF command" "BRIEF\nQUIT\nY" "Brief descriptions" "BRIEF works" "BRIEF command"
run_test "Test 11: SUPER-BRIEF command" "SUPER\nQUIT\nY" "Super-brief descriptions" "SUPER-BRIEF works" "SUPER-BRIEF command"
run_test "Test 12: LOOK object (cretin check)" "LOOK BRUSH\nQUIT\nY" "primitive two-word-parser" "LOOK <OBJECT> cretin message works" "LOOK <OBJECT>"
run_test "Test 13: EXAMINE container" "EXAMINE UNIFORM\nQUIT\nY" "Patrol uniform contains" "EXAMINE container works" "EXAMINE container"
run_test "Test 14: PRE-EXAMINE absent object" "EXAMINE LADDER\nQUIT\nY" "can't see any" "PRE-EXAMINE absent check works" "PRE-EXAMINE"
run_test "Test 15: SCORE command" "SCORE\nQUIT\nY" "out of 80 points" "SCORE command works" "SCORE command"
run_test "Test 16: VERSION command" "VERSION\nQUIT\nY" "Release 37" "VERSION command works" "VERSION command"
run_test "Test 17: AGAIN command" "LOOK\nAGAIN\nQUIT\nY" "Deck Nine" "AGAIN command works" "AGAIN command"
run_test "Test 18: QUIT decline" "QUIT\nN\nQUIT\nY" "Ok" "QUIT decline confirmation works" "QUIT decline"
run_test "Test 19: SAVE and RESTORE" "SAVE\nRESTORE\nQUIT\nY" "Game saved.*Game restored" "SAVE and RESTORE work" "SAVE/RESTORE"
run_test "Test 20: WALK AROUND" "WALK AROUND BRUSH\nQUIT\nY" "Use compass directions" "WALK AROUND works" "WALK AROUND"
run_test "Test 21: WALK TO" "WALK TO POD\nQUIT\nY" "It's here" "WALK TO works" "WALK TO"
run_test "Test 22: DROP and TAKE" "DROP BRUSH\nTAKE BRUSH\nQUIT\nY" "Dropped.*Taken" "DROP and TAKE work" "DROP/TAKE"
run_test "Test 23: PUT" "PUT BRUSH IN UNIFORM\nQUIT\nY" "Done" "PUT works" "PUT"
run_test "Test 24: THROW" "THROW BRUSH\nQUIT\nY" "Thrown" "THROW works" "THROW"
run_test "Test 25: SLIDE" "SLIDE BRUSH WITH UNIFORM\nQUIT\nY" "Fat chance|valiant attempt|can't be serious|bloody likely|interesting idea|concept" "SLIDE works" "SLIDE"
run_test "Test 26: CRAG and STRUCTURE pseudo" "TELEPORT CRAG\nEXAMINE STRUCTURE\nQUIT\nY" "climbed up to it" "Structure pseudo examine works" "STRUCTURE pseudo"
run_test "Test 27: BALCONY and PLAQUE" "TELEPORT BALCONY\nREAD PLAQUE\nQUIT\nY" "SEENIK VISTA" "Plaque reading works" "PLAQUE pseudo"
run_test "Test 28: COURTYARD and CASTLE" "TELEPORT COURTYARD\nEXAMINE CASTLE\nQUIT\nY" "ancient and crumbling" "Castle pseudo examine works" "CASTLE pseudo"
run_test "Test 29: WEST WING and refusal" "TELEPORT WING\nDOWN\nQUIT\nY" "Certain death" "West Wing down refusal works" "WEST WING DOWN"
run_test "Test 30: UNDERWATER drowning" "TELEPORT UNDERWATER\nWAIT\nWAIT\nQUIT\n" "mighty undertow" "Underwater drowning works" "UNDERWATER drowning"
run_test "Test 31: REC AREA dial" "TELEPORT AREA\nEXAMINE DIAL\nSET DIAL TO 500\nQUIT\nY" "dial is now set to 500" "Rec area dial setting works" "REC AREA dial"
run_test "Test 32: REC AREA games and tapes" "TELEPORT AREA\nEXAMINE GAMES\nEXAMINE TAPES\nQUIT\nY" "All the usual games" "Rec area games/tapes examine works" "GAMES/TAPES"
run_test "Test 33: DORM and SANFAC" "TELEPORT DORM\nEXAMINE PARTITION\nSOUTH\nEXAMINE TOILET\nFLUSH TOILET\nQUIT\nY" "water seems to be turned off" "Dorm and SanFac interactions work" "DORM/SANFAC"
run_test "Test 34: MESS CORRIDOR padlock and door" "TELEPORT MESS\nOPEN DOOR\nTAKE PADLOCK\nUNLOCK PADLOCK\nQUIT\nY" "padlock is locked to the door" "Mess corridor door and padlock work" "MESS CORRIDOR"
run_test "Test 35: STORAGE WEST Can and Ladder" "TELEPORT STORAGE\nEXAMINE CAN\nOPEN CAN\nEXAMINE LADDER\nOPEN LADDER\nQUIT\nY" "tiny space" "Storage West can and ladder examine/extend work" "STORAGE WEST"
run_test "Test 36: DORM CORRIDOR and Long Hall" "TELEPORT DORM CORRIDOR\nEXAMINE WALKWAY\nEAST\nQUIT\nY" "featureless hallway" "Dorm corridor walkway and long hall travel work" "LONG HALL"
run_test "Test 37: MESS HALL benches and kitchen door" "TELEPORT MESS HALL\nEXAMINE BENCH\nOPEN DOOR\nQUIT\nY" "Pleez yuuz kitcin akses kard" "Mess hall benches and door work" "MESS HALL"
run_test "Test 38: KITCHEN dispenser and canteen" "TELEPORT MESS HALL\nTAKE CANTEEN\nOPEN CANTEEN\nTELEPORT KITCHEN\nPUT CANTEEN IN DISPENSER\nPUSH BUTTON\nQUIT\nY" "canteen fills almost to the brim" "Kitchen dispenser canteen filling works" "KITCHEN DISPENSER"
run_test "Test 39: ADMIN CORRIDOR SOUTH Crevice and Key" "TELEPORT ADMIN SOUTH\nEXAMINE CREVICE\nTAKE KEY\nQUIT\nY" "shiny steel key" "Crevice examine and key discovery work" "CREVICE/KEY"
run_test "Test 40: SANFAC E" "TELEPORT ADMIN SOUTH\nEAST\nEXAMINE TOILET\nQUIT\nY" "dry and dusty" "SanFac E toilet examine works" "SANFAC E"
run_test "Test 41: ADMIN CORRIDOR Rift jump refusal" "TELEPORT ADMIN SOUTH\nNORTH\nEXAMINE RIFT\nNORTH\nQUIT\nY" "too wide to jump across" "Rift examine and jump refusal work" "RIFT"
run_test "Test 42: Throwing brush into rift" "TELEPORT ADMIN SOUTH\nNORTH\nPUT BRUSH IN RIFT\nQUIT\nY" "lost forever" "Putting item into rift works" "PUT IN RIFT"
run_test "Test 43: SYSTEMS MONITORS and equipment" "TELEPORT SYSTEMS MONITORS\nEXAMINE MONITORS\nEXAMINE EQUIPMENT\nQUIT\nY" "LIIBREREE, REEAKTURZ" "Systems monitors describe and equipment examine work" "SYSTEMS MONITORS"
run_test "Test 44: PLAN ROOM cubbyholes and maps" "TELEPORT PLAN ROOM\nEXAMINE CUBBYHOLES\nEXAMINE MAPS\nQUIT\nY" "used to hold maps" "Plan room cubbyholes and maps examine work" "PLAN ROOM"
run_test "Test 45: SMALL OFFICE desk and access cards" "TELEPORT SMALL OFFICE\nOPEN DESK\nLOOK IN DESK\nREAD KITCHEN CARD\nQUIT\nY" "kitcin akses kard" "Small office desk and kitchen card work" "SMALL OFFICE"
run_test "Test 46: LARGE OFFICE desk and access cards" "TELEPORT LARGE OFFICE\nOPEN DESK\nLOOK IN DESK\nREAD SHUTTLE CARD\nQUIT\nY" "shutul akses kard" "Large office desk and shuttle card work" "LARGE OFFICE"

echo "=== Tests Complete ==="
if [ $FAILED -ne 0 ]; then
    echo "$FAILED test(s) failed!"
    exit 1
fi
exit 0
