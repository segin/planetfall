
import re

implemented_objects = [
    "STRUCTURE-PSEUDO", "CLEFT-PSEUDO", "PLAQUE-PSEUDO", "CASTLE-PSEUDO",
    "RUBBLE-PSEUDO", "CONFERENCE-DOOR", "COMBINATION-DIAL", "STORAGE-WEST-DOOR",
    "PADLOCK", "KITCHEN-DOOR", "DISPENSER", "CANTEEN", "HIGH-PROTEIN",
    "CAN", "LADDER", "CREVICE", "KEY",
    "CHEMICAL-DISPENSER", "RED-BUTTON", "BLUE-BUTTON", "GREEN-BUTTON",
    "YELLOW-BUTTON", "GRAY-BUTTON", "BROWN-BUTTON", "BLACK-BUTTON",
    "ROUND-WHITE-BUTTON", "SQUARE-WHITE-BUTTON", "FLOYD",
    "UPPER-ELEVATOR-DOOR", "LOWER-ELEVATOR-DOOR", "BLUE-ELEVATOR-BUTTON",
    "RED-ELEVATOR-BUTTON", "ELEVATOR-BUTTON", "HELICOPTER-OBJECT",
    "RECEIVE-CONSOLE", "PLAYBACK-BUTTON", "SEND-CONSOLE", "COMM-SCREEN",
    "FUNNEL-HOLE", "CHEMICAL-FLUID"
]

implemented_rooms = [
    "UNDERWATER", "CRAG", "BALCONY", "WINDING-STAIR", "COURTYARD", "WEST-WING",
    "PLAIN-HALL", "REC-AREA", "CONFERENCE-ROOM", "BOOTH-1", "REC-CORRIDOR",
    "DORM-A", "SANFAC-A", "DORM-B", "SANFAC-B", "DORM-C", "SANFAC-C",
    "DORM-D", "SANFAC-D", "MESS-CORRIDOR", "MESS-HALL", "KITCHEN",
    "STORAGE-WEST", "DORM-CORRIDOR", "CORRIDOR-JUNCTION", "ADMIN-CORRIDOR-S",
    "MACHINE-SHOP", "ROBOT-SHOP", "ELEVATOR-LOBBY", "UPPER-ELEVATOR", 
    "LOWER-ELEVATOR", "BOOTH-2", "TOWER-CORE", "HELIPAD", "HELICOPTER", 
    "COMM-ROOM", "OBSERVATION-DECK", "WAITING-AREA", "KALAMONTEE-PLATFORM"
]

def main():
    with open('zil_analysis.md', 'r') as f:
        lines = f.readlines()

    with open('zil_analysis.md', 'w') as f:
        for line in lines:
            updated = False
            if line.startswith('- [ ]'):
                # Check if object or room name is in the line
                # Format: - [ ] **NAME** ...
                match = re.search(r'\*\*([A-Z0-9-]+)\*\*', line)
                if match:
                    name = match.group(1)
                    if name in implemented_objects or name in implemented_rooms:
                        line = line.replace('- [ ]', '- [x]')
                        updated = True
            f.write(line)

if __name__ == "__main__":
    main()
