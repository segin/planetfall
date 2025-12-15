# Planetfall ZIL Systems Documentation

> Comprehensive analysis of the core game systems in Planetfall's ZIL codebase.

---

## Table of Contents
1. [Core Game Loop](#core-game-loop)
2. [Clock & Interrupt System](#clock--interrupt-system)
3. [Player Status Systems](#player-status-systems)
4. [NPC Systems](#npc-systems)
5. [Plot Event Systems](#plot-event-systems)
6. [Verb Handler Architecture](#verb-handler-architecture)
7. [Object/Room Action System](#objectroom-action-system)

---

## Core Game Loop

**File:** `misc.zil`

### GO Routine
Entry point for the game. Initializes:
- Queues all interrupts onto clock chain
- Sets starting location (`DECK-NINE`)
- Sets initial game time (~4450-4630)
- Displays version info and intro text
- Calls `MAIN-LOOP`

### MAIN-LOOP Routine
The central game loop that processes player input each turn.

**Flow:**
1. Reset `C-ELAPSED` to default (7)
2. Call `PARSER` to get player command
3. Resolve "IT" pronoun references
4. For multi-object commands, iterate through objects
5. Call `PERFORM` to execute the action
6. Update `INTERNAL-MOVES` timer by `C-ELAPSED`
7. Call `CLOCKER` to run scheduled interrupts
8. Loop

### PERFORM Routine
Dispatches actions through the handler chain:
1. Actor handler (`WINNER.ACTION`)
2. Room begin handler (`LOC.ACTION` with `M-BEG`)
3. Pre-action handler (`PREACTIONS` table)
4. Indirect object handler (`PRSI.ACTION`)
5. Direct object handler (`PRSO.ACTION`)
6. Default verb handler (`ACTIONS` table)
7. Room end handler (`LOC.ACTION` with `M-END`)

### Time System
- `INTERNAL-MOVES`: Master game clock
- `C-ELAPSED`: Time consumed by current action (default 7)
- `MOVES`: Displayed time (synced with chronometer)
- Timeless verbs (SAVE, RESTORE, etc.) don't advance time

---

## Clock & Interrupt System

**File:** `misc.zil`

### Key Routines

| Routine | Purpose |
|---------|---------|
| `CLOCKER` | Runs each turn, processes enabled interrupts |
| `QUEUE` | Schedules an interrupt with tick count |
| `INT` | Creates/finds interrupt structure for a routine |
| `ENABLE` | Activates an interrupt |
| `DISABLE` | Deactivates an interrupt |

### Interrupt Structure
Each interrupt has:
- `C-ENABLED?`: 0 = disabled, 1 = enabled
- `C-TICK`: Countdown value (-1 = every turn, 0 = run now, >0 = countdown)
- `C-RTN`: The routine to call

### Interrupt Types
- **Demons** (`TICK = -1`): Run every turn while enabled
- **Timed Events** (`TICK > 0`): Count down by `C-ELAPSED`, fire when ≤0

### Initial Interrupts (set in GO)
| Interrupt | Initial Tick | Purpose |
|-----------|--------------|---------|
| `I-BLATHER` | -1 (demon) | Ensign Blather encounters |
| `I-AMBASSADOR` | -1 (demon) | Alien ambassador encounters |
| `I-RANDOM-INTERRUPTS` | 1 | One-time setup |
| `I-SLEEP-WARNINGS` | 3600 | Sleep deprivation |
| `I-HUNGER-WARNINGS` | 2000 | Hunger |
| `I-SICKNESS-WARNINGS` | 1000 | Radiation sickness |

---

## Player Status Systems

### Hunger System
**Interrupt:** `I-HUNGER-WARNINGS`

**Global:** `HUNGER-LEVEL`
- 0 = Not hungry
- Increases over time
- Eating resets to 0 and requeues interrupt

**Food Sources:**
- Survival kit goo (RED/GREEN/BROWN-GOO)
- High-protein liquid (canteen)

### Sleep System
**Interrupt:** `I-SLEEP-WARNINGS`

**Global:** `SLEEP-LEVEL`
- Escalating warnings as player stays awake
- Eventually leads to forced sleep

**Sleep Locations:**
- `BED` object (various dorms)
- Dream sequences possible

### Sickness System
**Interrupt:** `I-SICKNESS-WARNINGS`

Tracks radiation/disease exposure. Requires medicine to cure.

---

## NPC Systems

### Ensign Blather (`I-BLATHER`)
**File:** `globals.zil`

**Behavior:**
- Appears on Deck 8/9 and Reactor Lobby
- Gives demerits for infractions
- Sends player to brig after 3+ encounters
- Leaves Deck Nine after 3 turns

**Globals:**
- `BLATHER-LEAVE`: Counter for leaving
- `BRIGS-UP`: Counter for brig sentences

### Alien Ambassador (`I-AMBASSADOR`)
**File:** `globals.zil`

**Behavior:**
- Randomly appears on Deck Nine (15% chance)
- Gives player a brochure
- Has celery (don't eat it!)
- Leaves after 3 turns

**Globals:**
- `AMBASSADOR-LEAVE`: Counter
- `AMBASSADOR-QUOTES`: Random dialog table

### Floyd the Robot
**File:** `compone.zil`

The most complex NPC in the game with extensive behavior:
- Follows player between rooms
- Has autonomous actions and dialog
- Can be given commands
- Has emotional states
- Critical to game plot

---

## Plot Event Systems

### Feinstein Explosion (`I-BLOWUP-FEINSTEIN`)
**File:** `globals.zil`

**Global:** `BLOWUP-COUNTER`

**Sequence:**
1. Counter 1-2: Warning signs
2. Counter 3: Pod door closes automatically
3. Counter 4: Pod begins ejection, disables NPC interrupts
4. Counter 5: Ship explodes
   - If on Deck Nine: Death
   - If in pod: Escape sequence begins

### Pod Trip (`I-POD-TRIP`)
**File:** `globals.zil`

**Global:** `TRIP-COUNTER`

Handles escape pod journey and landing sequence:
- Pod tumbles through space
- Window polarization changes
- Planet approach
- Crash landing and aftermath

---

## Verb Handler Architecture

**Files:** `syntax.zil` (definitions), `verbs.zil` (handlers)

### Syntax Definition Format
```zil
<SYNTAX VERB-WORD [OBJECT] [PREPOSITION OBJECT] = V-HANDLER [PRE-HANDLER]>
```

### Handler Naming Conventions
| Prefix | Purpose |
|--------|---------|
| `V-` | Main verb handler |
| `PRE-` | Pre-action validations |

### Common Verb Categories

**Movement:**
- `V-WALK`, `V-CLIMB-UP`, `V-CLIMB-DOWN`
- `V-BOARD`, `V-DISEMBARK`, `V-THROUGH`

**Object Manipulation:**
- `V-TAKE`, `V-DROP`, `V-PUT`, `V-PUT-ON`
- `V-OPEN`, `V-CLOSE`, `V-UNLOCK`

**Examination:**
- `V-EXAMINE`, `V-LOOK-INSIDE`, `V-READ`
- `V-LOOK-UNDER`, `V-SEARCH`

**Combat/Interaction:**
- `V-ATTACK`, `V-KICK`, `V-ZAP` (laser)

**Communication:**
- `V-TELL`, `V-TALK`, `V-HELLO`
- `V-ASK-FOR`

**Meta:**
- `V-SAVE`, `V-RESTORE`, `V-QUIT`
- `V-SCORE`, `V-INVENTORY`

---

## Object/Room Action System

### Action Handler Protocol
Handlers receive `RARG` (Room ARGument) for context:
- `M-OBJECT` or `<>`: Direct object action
- `M-BEG`: Room entry/action start
- `M-END`: Post-action processing
- `M-ENTER`: Room entry
- `M-LOOK`: Room description
- `M-FLASH`: Brief room description
- `M-OBJDESC`: Object description override

### Return Values
- `<>` or false: Not handled, continue chain
- `T` or true: Handled, stop processing
- `M-FATAL` (2): Fatal error, abort turn

### Object Flags

| Flag | Meaning |
|------|---------|
| `TAKEBIT` | Can be picked up |
| `CONTBIT` | Is a container |
| `OPENBIT` | Is currently open |
| `DOORBIT` | Is a door |
| `LIGHTBIT` | Can be turned on/off |
| `ONBIT` | Is currently on |
| `READBIT` | Can be read |
| `FOODBIT` | Can be eaten |
| `VEHBIT` | Is a vehicle |
| `ACTORBIT` | Is an NPC |
| `NDESCBIT` | Don't auto-describe |
| `INVISIBLE` | Hidden from player |
| `TOUCHBIT` | Player has interacted |
| `WEARBIT` | Can be worn |
| `WORNBIT` | Is being worn |

---

## Global Variables Summary

### Game State
| Global | Purpose |
|--------|---------|
| `HERE` | Current room |
| `WINNER` | Current actor (usually ADVENTURER) |
| `LIT` | Is current location lit? |
| `SCORE` | Player score |
| `DAY` | Current day |

### Parser State
| Global | Purpose |
|--------|---------|
| `PRSA` | Current action verb |
| `PRSO` | Direct object |
| `PRSI` | Indirect object |
| `P-IT-OBJECT` | Last referenced object |

### Saved State (for AGAIN)
| Global | Purpose |
|--------|---------|
| `L-PRSA` | Last action |
| `L-PRSO` | Last direct object |
| `L-PRSI` | Last indirect object |

---

## Key Constants

| Constant | Value | Purpose |
|----------|-------|---------|
| `M-FATAL` | 2 | Fatal return code |
| `M-HANDLED` | 1 | Handled return code |
| `M-BEG` | 1 | Room begin context |
| `M-ENTER` | 2 | Room enter context |
| `M-LOOK` | 3 | Room look context |
| `M-FLASH` | 4 | Brief description context |
| `M-OBJDESC` | 5 | Object description context |
| `M-END` | 6 | Room end context |
| `DEFAULT-MOVE` | 20 | Default time for movement |
| `C-ELAPSED-DEFAULT` | 7 | Default action time |

---

## Complete Interrupt Reference

### All I-* Interrupt Routines

**Startup & NPCs:**
- `I-RANDOM-INTERRUPTS` - One-time game initialization
- `I-BLATHER` - Ensign Blather demon
- `I-AMBASSADOR` - Alien ambassador demon

**Feinstein & Pod:**
- `I-BLOWUP-FEINSTEIN` - Ship explosion sequence
- `I-POD-TRIP` - Escape pod journey
- `I-SINK-POD` - Pod sinking after landing

**Player Status:**
- `I-SLEEP-WARNINGS` - Tiredness warnings
- `I-FALL-ASLEEP` - Forced sleep
- `I-HUNGER-WARNINGS` - Starvation warnings
- `I-SICKNESS-WARNINGS` - Disease/radiation

**Elevators:**
- `I-UPPER-ELEVATOR-ARRIVE/TRIP/TURNOFF` - Upper elevator
- `I-LOWER-ELEVATOR-ARRIVE/TRIP/TURNOFF` - Lower elevator
- `I-CRYO-ELEVATOR-ARRIVE` - Cryo elevator

**Transportation:**
- `I-SHUTTLE` - Shuttle car movement
- `I-TURNOFF-SHUTTLE` - Shuttle timeout
- `I-TURNOFF-TELEPORTATION` - Teleporter timeout

**Doors:**
- `I-KITCHEN-DOOR-CLOSES` - Kitchen auto-close
- `I-REACTOR-DOOR-CLOSE` - Reactor auto-close
- `I-BIO-EAST-CLOSES`, `I-BIO-WEST-CLOSES` - Bio lab doors

**Floyd:**
- `I-FLOYD` - Main Floyd behavior
- `I-CLEAR-FLOYD-PEER` - Clear peering state
- `I-FLOYD-FORAY` - Bio lab mission
- `I-CHASE-SCENE` - Chase sequence

**Combat & Hazards:**
- `I-MAGNET` - Magnet effects
- `I-WARMTH`, `I-FRY` - Laser overheating
- `I-NUKED-BLUE` - Radiation damage
- `I-UNFLOOD` - Flood drain
- `I-MICROBE` - Microbe behavior
- `I-TURNOFF-MINI` - Miniaturization timeout
- `I-ANNOUNCEMENT` - Computer announcements

---

## State Counters

| Counter | Purpose |
|---------|---------|
| `BLOWUP-COUNTER` | Feinstein explosion phases |
| `TRIP-COUNTER` | Pod journey phases |
| `SINK-COUNTER` | Pod sinking phases |
| `SHUTTLE-COUNTER` | Shuttle trip progress |
| `WAITING-COUNTER` | Bio lab wait time |
| `FORAY-COUNTER` | Floyd bio mission phases |
| `NUKED-COUNTER` | Radiation exposure |
| `MARKSMANSHIP-COUNTER` | Laser accuracy |
| `MICROBE-COUNTER` | Microbe sequence |
