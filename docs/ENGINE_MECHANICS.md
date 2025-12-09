# Planetfall Engine Mechanics

## Overview
The Planetfall engine is a custom implementation of the Z-Machine architecture, tailored specifically for porting the ZIL source code to C. It manages the game world, object hierarchy, parser, and event loop.

## Object Model
The core of the game world is the **Object Tree**. Every entity in the game (Rooms, Items, Player, Scenery) is an `Object`.

### Data Structure
An `Object` is defined by:
- **ID**: Unique identifier.
- **Hierarchy Links**:
  - `parent`: The object containing this object.
  - `child`: The first object contained within this object.
  - `sibling`: The next object sharing the same parent.
- **Flags**: A 32-bit (or 64-bit) bitmask representing boolean states (e.g., `TAKEBIT`, `OPENBIT`, `LITBIT`).
- **Properties**: Key-value pairs defining attributes like `DESC`, `SIZE`, `CAPACITY`, `TEXT`.
- **Exits** (Rooms only): Definitions for movement (NORTH, SOUTH, etc.). These can be direct links to other rooms or function callbacks (NEXIT).
- **Action Routine**: A pointer to a C function that handles specific events for this object.

### Hierarchy Logic
- **Containment**: Objects are "in" a room or "in" a container/inventory if their `parent` points to it.
- **Movement**: Moving an object involves:
  1. Detaching it from its current `parent` (updating `child`/`sibling` links).
  2. Attaching it to the new `parent`.

## Game Loop
1. **Input**: Read a line of text from the user.
2. **Parser**: Tokenize and Analyze.
   - Match words against the Vocabulary.
   - Identify Grammar patterns (VERB, VERB NOUN, VERB NOUN PREP NOUN).
   - Resolve objects (Disambiguation).
3. **Perform**: Execute the action.
   - `Perform(Verb, DirectObject, IndirectObject)`
4. **Update**:
   - Run Daemons and Timers (e.g., `I-BLATHER`, `I-HUNGER`).
   - Increment turn counter.

## Action Processing Order
When a command is executed (e.g., `TAKE BRUSH`):
1. **Parser** identifies `PRSO` (Direct Object) as `SCRUB-BRUSH`.
2. **Pre-Action**: Check if the verb has a `PRE-` routine (e.g., `PRE-TAKE`).
3. **Object Specific**: Call the `ACTION` routine of the object (if any). If it handles the verb, stop.
4. **Verb Routine**: Call the generic `V-TAKE` routine.

## Flags & Attributes
Common flags derived from `globals.zil`:
- `TAKEBIT`: Can be picked up.
- `OPENBIT`: Is open.
- `DOORBIT`: Is a door.
- `CONTBIT`: Is a container.
- `TRANSBIT`: Is transparent (can see inside).
- `WEARBIT`: Can be worn.
- `WORNBIT`: Is currently worn.
- `LIGHTBIT`: Emits light.
- `ONBIT`: Is turned on.
