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

# Same as run_test, but fails if the pattern IS present.
run_test_absent() {
    local name="$1"
    local input="$2"
    local pattern="$3"
    local pass_msg="$4"
    local fail_msg="$5"

    echo "$name"
    OUTPUT=$(echo -e "$input" | ./planetfall 2>/dev/null | strip_ansi)
    if echo "$OUTPUT" | grep -qiE "$pattern"; then
        echo "FAIL: $fail_msg"
        FAILED=$((FAILED + 1))
    else
        echo "PASS: $pass_msg"
    fi
}

run_test "Test 1: Room navigation (UP from Deck Nine)" "UP\nQUIT\nY" "Gangway" "UP leads to Gangway" "UP navigation"
run_test "Test 2: Port direction (should go west to pod if open)" "PORT\nQUIT\nY" "pod|closed|can't" "Port direction works" "Port direction"
# The pod bulkhead is sealed until the first explosion blows it open. Walking
# west, going in, and boarding must all be refused, and the player must not end
# up inside the pod.
run_test "Test 2a: Pod bulkhead refuses entry before the emergency" "WEST\nQUIT\nY" "escape pod bulkhead is closed" "Walking west into a sealed pod is refused" "Premature pod entry"
run_test_absent "Test 2b: Player stays on Deck Nine when refused" "WEST\nLOOK\nQUIT\nY" "safety webbing" "Refused walk leaves player outside the pod" "Player entered sealed pod"
run_test "Test 2c: IN is refused the same way" "IN\nQUIT\nY" "escape pod bulkhead is closed" "IN into a sealed pod is refused" "Premature pod entry via IN"
run_test "Test 2d: Opening the bulkhead early is refused" "OPEN BULKHEAD\nQUIT\nY" "if there's no emergency" "Pod bulkhead cannot be opened early" "Pod bulkhead opened early"
run_test "Test 2e: Deck Nine reports the bulkhead state" "LOOK\nQUIT\nY" "pod bulkhead is closed" "Deck Nine describes the sealed bulkhead" "Bulkhead state not described"
run_test "Test 3: Examine ME" "EXAMINE ME\nQUIT\nY" "eyes are prehensile" "CRETIN-F handles examining yourself" "Examine ME"
run_test "Test 4: Look command" "LOOK\nQUIT\nY" "Deck Nine" "Look works" "Look"
run_test "Test 5: Inventory" "INVENTORY\nQUIT\nY" "carrying|uniform|chronometer|brush" "Inventory works" "Inventory"
# The Feinstein explodes on a 241..330 GST timer. Standing still costs 7 units a
# turn, so the first blast lands somewhere around turn 35-47 -- not turn 5. Give
# it a wide enough window to fire, and a short one to confirm it does NOT fire
# early, which is the regression that made the ship unexplorable.
WAIT_50=$(printf 'WAIT\\n%.0s' $(seq 1 50))
run_test "Test 6: Explosion eventually triggers" "${WAIT_50}QUIT\nY" "explosion" "Events trigger" "Events"
run_test_absent "Test 6b: Ship survives long enough to explore" "WAIT\nWAIT\nWAIT\nWAIT\nWAIT\nLOOK\nQUIT\nY" "explosion" "No explosion within the first 5 turns" "Premature explosion"
run_test "Test 7: Scrub brush exists" "I\nQUIT\nY" "brush" "Scrub brush in inventory" "Scrub brush missing"
run_test "Test 8: Examine chronometer" "EXAMINE CHRONOMETER\nQUIT\nY" "wrist|digital|time|chronometer" "Chronometer examine works" "Chronometer"
run_test "Test 9: VERBOSE command" "VERBOSE\nQUIT\nY" "Maximum verbosity" "VERBOSE works" "VERBOSE command"
run_test "Test 10: BRIEF command" "BRIEF\nQUIT\nY" "Brief descriptions" "BRIEF works" "BRIEF command"
run_test "Test 11: SUPER-BRIEF command" "SUPER\nQUIT\nY" "Super-brief descriptions" "SUPER-BRIEF works" "SUPER-BRIEF command"
run_test "Test 12: LOOK object (cretin check)" "LOOK BRUSH\nQUIT\nY" "primitive two-word-parser" "LOOK <OBJECT> cretin message works" "LOOK <OBJECT>"
# PATROL-UNIFORM-F describes the uniform; it does not list the pocket contents.
run_test "Test 13: EXAMINE uniform" "EXAMINE UNIFORM\nQUIT\nY" "miracle of modern" "PATROL-UNIFORM-F describes the uniform" "EXAMINE uniform"
run_test "Test 13a: The uniform pocket cannot be worked" "OPEN UNIFORM\nQUIT\nY" "no way to open or close the pocket" "Uniform open/close refusal works" "Uniform pocket"
run_test "Test 13b: READ ID CARD resolves" "READ ID CARD\nQUIT\nY" "6172-531-541" "ID card adjectives parse" "ID card parse"
run_test "Test 14: PRE-EXAMINE absent object" "EXAMINE LADDER\nQUIT\nY" "can't see any" "PRE-EXAMINE absent check works" "PRE-EXAMINE"
run_test "Test 15: SCORE command" "SCORE\nQUIT\nY" "out of 80 points" "SCORE command works" "SCORE command"
run_test "Test 16: VERSION command" "VERSION\nQUIT\nY" "Release 37" "VERSION command works" "VERSION command"
run_test "Test 17: AGAIN command" "LOOK\nAGAIN\nQUIT\nY" "Deck Nine" "AGAIN command works" "AGAIN command"
run_test "Test 18: QUIT decline" "QUIT\nN\nQUIT\nY" "Ok" "QUIT decline confirmation works" "QUIT decline"
run_test "Test 19: SAVE and RESTORE" "SAVE\nRESTORE\nQUIT\nY" "Game saved.*Game restored" "SAVE and RESTORE work" "SAVE/RESTORE"
run_test "Test 20: WALK AROUND" "WALK AROUND BRUSH\nQUIT\nY" "Use compass directions" "WALK AROUND works" "WALK AROUND"
# V-WALK-TO only says "It's here!" for something actually in the room; the floor
# is a global object, so it falls through to USE-DIRECTIONS as the original does.
run_test "Test 21: WALK TO a global falls through" "WALK TO FLOOR\nQUIT\nY" "compass directions" "WALK TO defers to USE-DIRECTIONS" "WALK TO"
# GLOBAL-POD-F intercepts WALK-TO and routes it through the bulkhead rather than
# answering "It's here!", so walking to the pod is refused while it is sealed.
run_test "Test 21a: WALK TO POD goes through the bulkhead" "WALK TO POD\nQUIT\nY" "escape pod bulkhead is closed" "WALK TO POD routes through the door" "WALK TO POD"
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
run_test "Test 47: STORAGE EAST box and cracked board" "TELEPORT STORAGE EAST\nLOOK IN BOX\nEXAMINE BOARD\nQUIT\nY" "twisted maze of silicon circuits" "Storage East box and board examine work" "STORAGE EAST"
run_test "Test 48: OIL CAN and BOX close refusal" "TELEPORT STORAGE EAST\nTAKE OIL CAN\nPOUR CAN\nCLOSE BOX\nQUIT\nY" "You can't close that" "Oil can pour and box close refusal work" "OIL CAN / BOX"
run_test "Test 49: PHYSICAL PLANT catwalk climb" "TELEPORT PHYSICAL PLANT\nCLIMB CATWALK\nQUIT\nY" "catwalks are too high" "Physical plant catwalk climb refusal works" "PHYSICAL PLANT"
run_test "Test 50: REACTOR CONTROL elevator door button" "TELEPORT REACTOR CONTROL\nPUSH BUTTON\nEAST\nLOOK\nWEST\nQUIT\nY" "Reactor Elevator" "Reactor control elevator door button works" "REACTOR CONTROL"
run_test "Test 51: TOOL ROOM flask and pliers" "TELEPORT TOOL ROOM\nEXAMINE FLASK\nEXAMINE PLIERS\nQUIT\nY" "wide-nosed pliers" "Tool room flask and pliers examine work" "TOOL ROOM"
run_test "Test 52: MAGNET crevice key extraction" "TELEPORT TOOL ROOM\nTAKE MAGNET\nTELEPORT ADMIN SOUTH\nEXAMINE CREVICE\nPUT MAGNET OVER CREVICE\nQUIT\nY" "metal leaps from" "Magnet pulling key from crevice works" "MAGNET KEY EXTRACTION"
run_test "Test 53: MACHINE SHOP dispenser and flask filling" "TELEPORT TOOL ROOM\nTAKE FLASK\nTELEPORT MACHINE SHOP\nPUT FLASK UNDER DISPENSER\nPUSH RED BUTTON\nEXAMINE FLASK\nQUIT\nY" "milky white fluid" "Machine shop chemical dispenser and flask work" "MACHINE SHOP"
run_test "Test 54: ROBOT SHOP devices examine" "TELEPORT ROBOT SHOP\nEXAMINE DEVICES\nQUIT\nY" "disassembled robots" "Robot shop devices examine works" "ROBOT SHOP"
run_test "Test 55: FLOYD examine and turn on" "TELEPORT ROBOT SHOP\nEXAMINE FLOYD\nTURN ON FLOYD\nQUIT\nY" "comes to life" "Floyd examine and activation work" "FLOYD ACTIVATION"
run_test "Test 56: ELEVATOR LOBBY call buttons" "TELEPORT ELEVATOR LOBBY\nPUSH BLUE BUTTON\nPUSH RED BUTTON\nQUIT\nY" "whirring noise" "Elevator lobby call buttons work" "ELEVATOR LOBBY BUTTONS"
run_test "Test 57: UPPER ELEVATOR card enable" "TELEPORT SMALL OFFICE\nOPEN DESK\nTAKE UPPER ELEVATOR CARD\nTELEPORT UPPER ELEVATOR\nSLIDE UPPER ELEVATOR CARD THROUGH SLOT\nQUIT\nY" "Elivaatur inebuld" "Upper elevator card enable works" "UPPER ELEVATOR CARD"
run_test "Test 58: UPPER ELEVATOR trip activation" "TELEPORT SMALL OFFICE\nOPEN DESK\nTAKE UPPER ELEVATOR CARD\nTELEPORT UPPER ELEVATOR\nSLIDE UPPER ELEVATOR CARD THROUGH SLOT\nPUSH DOWN BUTTON\nQUIT\nY" "vertical movement" "Upper elevator trip activation works" "UPPER ELEVATOR TRIP"
run_test "Test 59: HELIPAD fence and helicopter boarding" "TELEPORT HELIPAD\nNORTH\nBOARD VEHICLE\nFLY\nOUT\nQUIT\nY" "locked" "Helipad fence and helicopter boarding work" "HELIPAD / HELICOPTER"
run_test "Test 60: COMM ROOM playback and screen" "TELEPORT COMM ROOM\nPUSH PLAYBACK BUTTON\nREAD SCREEN\nEXAMINE CABLES\nQUIT\nY" "Feinstein" "Comm room playback button and screen reading work" "COMM ROOM"
run_test "Test 61: KALAMONTEE PLATFORM description" "TELEPORT WAITING AREA\nEAST\nLOOK\nQUIT\nY" "Kalamontee Staashun" "Kalamontee platform navigation and description work" "KALAMONTEE PLATFORM"

# --- Shared input fragments -------------------------------------------------
# The first blast opens the pod bulkhead around turn 38; the ambassador turns up
# on turn 14 and stays three turns. Both land on fixed turns only because the
# port's rand() is unseeded.
WAIT_12=$(printf 'WAIT\\n%.0s' $(seq 1 12))
WAIT_14=$(printf 'WAIT\\n%.0s' $(seq 1 14))
WAIT_30=$(printf 'WAIT\\n%.0s' $(seq 1 30))
WAIT_38=$(printf 'WAIT\\n%.0s' $(seq 1 38))

# --- V-DIAGNOSE and the hunger timer ----------------------------------------
run_test "Test 111: DIAGNOSE reports all three" "DIAGNOSE\nQUIT\nY" "perfect health" "V-DIAGNOSE health line works" "Diagnose health"
run_test "Test 111a: DIAGNOSE reports rest" "DIAGNOSE\nQUIT\nY" "well-rested" "V-DIAGNOSE rest line works" "Diagnose rest"
# I-HUNGER-WARNINGS is a 2000-unit timer, not a daemon: you do not start hungry.
run_test "Test 111b: You do not start out hungry" "DIAGNOSE\nQUIT\nY" "well-fed" "Hunger starts at zero" "Diagnose hunger"
run_test_absent "Test 111c: No hunger warning early on" "${WAIT_38}QUIT\nY" "getting pretty hungry" "Hunger timer does not fire early" "Premature hunger"

# --- Parser errors (UNKNOWN-WORD / ORPHAN / I beg your pardon) --------------
run_test "Test 106: Unknown words are named" "XYZZY\nQUIT\nY" "don't know the word \"xyzzy\." "UNKNOWN-WORD reports the word" "Unknown word"
run_test "Test 107: A verb with no object asks for one" "TAKE\nQUIT\nY" "What do you want to take" "ORPHAN asks for the missing object" "Orphan"
run_test "Test 108: Empty input" "\nQUIT\nY" "beg your pardon" "Empty input is handled" "Empty input"
run_test "Test 109: Unparseable but known words" "TAKE BRUSH BRUSH BRUSH\nQUIT\nY" "don't understand that sentence|can't see any" "Known words that do not parse still fail cleanly" "Bad sentence"
run_test "Test 110: Not-here message wording" "READ TOWEL\nQUIT\nY" "can't see any towel here!" "Not-here message matches the R39 source" "Not here"

# --- The escape pod (ESCAPE-POD-F, SAFETY-WEB-F, POD-EXIT-F, I-SINK-POD) -----
# The first blast opens the bulkhead around turn 38; boarding the web and riding
# the pod down takes another dozen turns. POD_LANDED leaves the player webbed in
# a pod resting on the water.
POD_BOARDED="${WAIT_38}WEST\n"
POD_LANDED="${POD_BOARDED}ENTER WEB\n${WAIT_12}"

run_test "Test 78: Pod describes its own bulkhead" "${POD_BOARDED}LOOK\nQUIT\nY" "bulkhead leading out is open" "ESCAPE-POD-F reports bulkhead state" "Pod description"
run_test "Test 79: Examining the safety web" "${POD_BOARDED}EXAMINE WEB\nQUIT\nY" "one to, perhaps, twenty people" "Safety web description works" "Safety web examine"
run_test "Test 80: Pod controls are automated" "${POD_BOARDED}EXAMINE CONTROLS\nQUIT\nY" "entirely automated" "CONTROLS-F pod branch works" "Pod controls"
run_test "Test 81: Boarding the web" "${POD_BOARDED}ENTER WEB\nQUIT\nY" "safely cushioned within the web" "Boarding the web works" "Web boarding"
run_test "Test 82: Landing produces provisions" "${POD_LANDED}LOOK\nQUIT\nY" "survival kit" "Landing reveals the survival kit" "Pod provisions"
run_test "Test 83: Cannot walk while webbed" "${POD_LANDED}OUT\nQUIT\nY" "stand up, first" "Walking while webbed is refused" "Webbed movement"
run_test "Test 84: Standing drops the pod into the water" "${POD_LANDED}STAND\nQUIT\nY" "you feel it falling" "Standing starts the pod sinking" "Pod sinking"
run_test "Test 85: The sinking pod crushes you" "${POD_LANDED}STAND\nWAIT\nWAIT\nWAIT\nWAIT\nQUIT\nY" "pod splits open" "Sinking pod kills a sealed-in player" "Sink death"
run_test "Test 86: Escaping the sinking pod" "${POD_LANDED}STAND\nOPEN BULKHEAD\nOUT\nUP\nQUIT\nY" "reached a cleft in the cliff wall" "Player can escape the pod to the Crag" "Pod escape"
# The survival kit is closed on arrival (ZIL gives FOOD-KIT no OPENBIT), so the
# goo is out of scope until you open it.
run_test "Test 87: Goo must be eaten from the kit" "${POD_LANDED}STAND\nOPEN KIT\nTAKE RED GOO\nQUIT\nY" "ooze through your fingers" "GOO-F refuses to be carried" "Goo take"
run_test "Test 87a: Goo is out of scope while the kit is shut" "${POD_LANDED}STAND\nTAKE RED GOO\nQUIT\nY" "can't see any" "Closed kit hides the goo" "Closed kit scope"
run_test "Test 88: Reading the towel" "${POD_LANDED}STAND\nREAD TOWEL\nQUIT\nY" "Don't Panic" "Towel text works" "Towel"

# --- Brig, string exits and shared scenery ----------------------------------
# Blather puts you in the brig after three warnings off-post, which is the only
# way to get there.
BRIGGED="UP\nUP\nWAIT\nWAIT\nWAIT\nWAIT\n"

run_test "Test 89: Brig graffiti" "${BRIGGED}READ GRAFFITI\nQUIT\nY" "fawg-infested tar-pools" "GRAFFITI-PSEUDO works" "Brig graffiti"
run_test "Test 90: Brig cell door is locked" "${BRIGGED}SOUTH\nQUIT\nY" "cell door is locked" "Brig south is a string exit" "Brig south exit"
run_test "Test 91: Brig cell door will not open" "${BRIGGED}OPEN DOOR\nQUIT\nY" "No way, Jose" "DOOR-PSEUDO works" "Brig door"
run_test "Test 92: Deck Eight refusals are Blather's" "UP\nUP\nEAST\nQUIT\nY" "20 push-ups" "Deck Eight string exits work" "Deck Eight exits"
run_test "Test 93: Deck Eight north refusal" "UP\nUP\nNORTH\nQUIT\nY" "extra galley duty" "Deck Eight north string exit works" "Deck Eight north"
run_test "Test 94: Reactor Lobby refusals" "EAST\nSOUTH\nQUIT\nY" "back toward your post" "Reactor Lobby string exits work" "Reactor Lobby exits"
run_test "Test 95: Unhandled directions still fall back" "${BRIGGED}NORTH\nQUIT\nY" "can't go that way" "Stock refusal still applies elsewhere" "Default refusal"

run_test "Test 96: Smelling yourself" "SMELL ME\nQUIT\nY" "Phew" "CRETIN-F smell works" "Cretin smell"
run_test "Test 97: Eating yourself" "EAT ME\nQUIT\nY" "Auto-cannibalism" "CRETIN-F eat works" "Cretin eat"
run_test "Test 98: Taking yourself" "TAKE ME\nQUIT\nY" "How romantic" "CRETIN-F take works" "Cretin take"
run_test "Test 99: Scrubbing yourself" "SCRUB ME\nQUIT\nY" "300 demerits" "CRETIN-F scrub works" "Cretin scrub"
run_test "Test 100: Attacking yourself is fatal" "ATTACK ME\nQUIT\nY" "Poof, you're dead" "CRETIN-F attack kills you" "Cretin attack"
run_test "Test 101: Shaking hands with nobody" "SHAKE HANDS\nQUIT\nY" "no one to shake hands with" "HANDS-F default works" "Hands default"
run_test "Test 102: Shaking hands with the ambassador" "${WAIT_14}SHAKE HANDS\nQUIT\nY" "repulsive idea" "HANDS-F ambassador branch works" "Hands ambassador"

run_test "Test 103: Pod window early in the trip" "${POD_BOARDED}ENTER WEB\nLOOK THROUGH WINDOW\nQUIT\nY" "debris from the exploding Feinstein" "WINDOW-F early trip branch works" "Window early"
run_test "Test 104: Pod window on approach" "${POD_BOARDED}ENTER WEB\n${WAIT_12}LOOK THROUGH WINDOW\nQUIT\nY" "hopefully a hospitable one" "WINDOW-F late trip branch works" "Window late"
run_test "Test 105: The window does not open" "${POD_BOARDED}OPEN WINDOW\nQUIT\nY" "doesn't open" "WINDOW-F open branch works" "Window open"

# --- Blather and the alien ambassador (I-BLATHER / I-AMBASSADOR) -------------
# Blather's off-post branch is unconditional, so leaving Deck Nine always
# summons him; his Deck Nine visit and the ambassador's arrival are probability
# rolls, which land on fixed turns only because rand() is never seeded.

run_test "Test 62: Blather hunts you down off-post" "UP\nUP\nQUIT\nY" "notices you are away" "Blather appears when you leave your post" "Blather off-post"
run_test "Test 63: Blather warns before brigging you" "UP\nUP\nWAIT\nQUIT\nY" "return to your post" "Blather issues warnings" "Blather warning"
run_test "Test 64: Blather brigs you after three warnings" "UP\nUP\nWAIT\nWAIT\nWAIT\nWAIT\nQUIT\nY" "drags you to the Feinstein" "Blather brigs the player" "Blather brig"
run_test "Test 65: Saluting Blather" "UP\nUP\nSALUTE BLATHER\nQUIT\nY" "sneer softens" "Salute is acknowledged" "Blather salute"
run_test "Test 66: Examining Blather" "UP\nUP\nEXAMINE BLATHER\nQUIT\nY" "misshapen nose" "Blather description works" "Blather examine"
run_test "Test 67: Taking Blather" "UP\nUP\nTAKE BLATHER\nQUIT\nY" "suspended shore leave" "Blather rebuffs being taken" "Blather take"
run_test "Test 68: Throwing something at Blather" "UP\nUP\nTHROW BRUSH AT BLATHER\nQUIT\nY" "bulbous nose" "Throwing at Blather works" "Blather throw"
run_test "Test 69: Attacking Blather is fatal" "UP\nUP\nATTACK BLATHER\nQUIT\nY" "appendages and internal organs" "Attacking Blather kills you" "Blather attack"

run_test "Test 70: Ambassador arrives on Deck Nine" "${WAIT_30}QUIT\nY" "Blow'k-bibben-Gordo ambles toward you" "Ambassador arrives" "Ambassador arrival"
run_test "Test 71: Ambassador makes small talk" "${WAIT_30}QUIT\nY" "The ambassador (introduces|asks|inquires|recites|remarks|offers)" "Ambassador quotes fire" "Ambassador quotes"
run_test "Test 72: Ambassador leaves for good" "${WAIT_30}QUIT\nY" "grunts a polite farewell" "Ambassador departs" "Ambassador departure"
run_test "Test 73: Ambassador hands over the brochure" "${WAIT_14}READ BROCHURE\nQUIT\nY" "S. Eric Meretzky" "Brochure text is the full ZIL text" "Brochure text"
run_test "Test 74: Examining the ambassador" "${WAIT_14}EXAMINE AMBASSADOR\nQUIT\nY" "twenty eyes" "Ambassador description works" "Ambassador examine"
run_test "Test 75: Celery cannot be taken" "${WAIT_14}TAKE CELERY\nQUIT\nY" "lack of normal protocol" "Celery refuses to be taken" "Celery take"
run_test "Test 76: Eating the celery is fatal" "${WAIT_14}EAT CELERY\nQUIT\nY" "convulsions" "Eating celery kills you" "Celery eat"
run_test "Test 77: Slime responds to the senses" "${WAIT_14}EXAMINE SLIME\nQUIT\nY" "didn't step in it" "Slime pseudo-object works" "Slime"

# --- Floyd (I-FLOYD / FLOYD-COMES-ALIVE / KLUDGE) ---------------------------
FLOYD_ON="TELEPORT ROBOT SHOP\nTURN ON FLOYD\n"
FLOYD_WAIT=$(printf 'WAIT\\n%.0s' $(seq 1 40))

run_test "Test 112: Floyd comes alive" "${FLOYD_ON}QUIT\nY" "I'm B-19-7" "FLOYD-COMES-ALIVE works" "Floyd activation"
run_test "Test 113: Floyd notices what you carry" "${FLOYD_ON}QUIT\nY" "That's a nice .* you are having there" "Floyd remarks on a carried object" "Floyd carried object"
run_test "Test 114: Turning Floyd on twice" "${FLOYD_ON}TURN ON FLOYD\nQUIT\nY" "already been activated" "Second activation is refused" "Floyd reactivation"
run_test "Test 115: Floyd follows you" "${FLOYD_ON}WEST\nWAIT\nQUIT\nY" "Floyd follows you" "Floyd follows between rooms" "Floyd follow"
run_test "Test 116: Floyd chatters" "${FLOYD_ON}${FLOYD_WAIT}QUIT\nY" "paces impatiently|digits of pi|Dr. Fizpick|bruised his knee|Hucka-Bucka-Beanstalk|signs of rust|totally out of key|batteries failing|whistles tunelessly|looks bored" "FLOYDISMS fire" "Floyd chatter"
run_test "Test 117: Floyd wanders off" "${FLOYD_ON}${FLOYD_WAIT}QUIT\nY" "going exploring" "Floyd leaves FLOYDBIT rooms" "Floyd exploring"
run_test "Test 118: Floyd on Achilles" "${FLOYD_ON}TELEPORT REPAIR ROOM\nWAIT\nWAIT\nQUIT\nY" "they named him Achilles" "KLUDGE fires in the Repair Room" "Floyd Achilles"
run_test "Test 119: Shaking hands with Floyd" "${FLOYD_ON}SHAKE HANDS\nQUIT\nY" "grasping extensions" "HANDS-F recognises an active Floyd" "Floyd handshake"

echo "=== Tests Complete ==="
if [ $FAILED -ne 0 ]; then
    echo "$FAILED test(s) failed!"
    exit 1
fi
exit 0
