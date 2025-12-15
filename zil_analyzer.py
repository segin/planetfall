import os
import re

ZIL_DIR = "zil"

# Data structures to hold findings
objects = {}
rooms = {}
routines = {}
verbs = {}
actions = {} # Action things like DECK-NINE-F

def parse_zil_file(filepath):
    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()

    # Simple parsing logic - this might need refinement for nested structures but should catch top-level definitions
    
    # regex for OBJECT
    # <OBJECT NAME ...>
    # We need to capture the name and the content inside
    # Since they can be multi-line and nested, regex is tricky. 
    # A simple token parser might be better, or just finding the start and balancing brackets.
    
    # Let's try to find all occurrences of <OBJECT, <ROOM, <ROUTINE, <SYNTAX
    
    pos = 0
    while pos < len(content):
        match = re.search(r'<((?:OBJECT|ROOM|ROUTINE|SYNTAX|SYNONYM|GLOBAL))\s+([^>\s]+)', content[pos:], re.IGNORECASE)
        if not match:
            break
            
        start_index = pos + match.start()
        definition_type = match.group(1).upper()
        name = match.group(2)
        
        # Find the matching closing > 
        # We need to handle nested < >
        angle_depth = 0
        end_index = start_index
        found_end = False
        
        for i in range(start_index, len(content)):
            if content[i] == '<':
                angle_depth += 1
            elif content[i] == '>':
                angle_depth -= 1
                if angle_depth == 0:
                    end_index = i
                    found_end = True
                    break
        
        if not found_end:
            # Maybe EOF reached
            end_index = len(content)
            
        block_content = content[start_index:end_index+1]
        
        if definition_type == "OBJECT":
            parse_object(name, block_content, filepath)
        elif definition_type == "ROOM":
            parse_room(name, block_content, filepath)
        elif definition_type == "ROUTINE":
            parse_routine(name, block_content, filepath)
        elif definition_type == "SYNTAX":
            parse_syntax(block_content, filepath)
            
        pos = end_index + 1

def extract_property(block, prop_name):
    # Searches for (PROP_NAME value...)
    # This is also simple regex, might fail on complex nested lists
    match = re.search(r'\(\s*' + re.escape(prop_name) + r'\s+([^)]+)\)', block, re.IGNORECASE | re.DOTALL)
    if match:
        return match.group(1).strip()
    return None

def parse_object(name, block, filepath):
    # Check if it has ROOMBIT in flags, sometimes OBJECT is used for rooms
    flags = extract_property(block, "FLAGS")
    is_room = False
    if flags and "ROOMBIT" in flags.upper():
        is_room = True
    
    data = {
        "name": name,
        "desc": extract_property(block, "DESC"),
        "in": extract_property(block, "IN"),
        "flags": flags,
        "action": extract_property(block, "ACTION"),
        "synonyms": extract_property(block, "SYNONYM"),
        "file": os.path.basename(filepath)
    }
    
    if is_room:
        rooms[name] = data
    else:
        objects[name] = data
        
    if data["action"]:
        actions[data["action"]] = {"object": name, "type": "Object Action"}

def parse_room(name, block, filepath):
    data = {
        "name": name,
        "desc": extract_property(block, "DESC"),
        "in": "ROOMS", # Default parent for rooms usually
        "flags": extract_property(block, "FLAGS"),
        "action": extract_property(block, "ACTION"),
        "synonyms": None, # Typically rooms don't map straightforwardly to synonyms like objects, usually via Global objects
        "file": os.path.basename(filepath)
    }
    rooms[name] = data
    if data["action"]:
        actions[data["action"]] = {"object": name, "type": "Room Action"}

def parse_routine(name, block, filepath):
    # Extract args
    args_match = re.search(r'<ROUTINE\s+' + re.escape(name) + r'\s+\(([^)]*)\)', block, re.IGNORECASE)
    args = args_match.group(1) if args_match else ""
    
    routines[name] = {
        "name": name,
        "args": args,
        "file": os.path.basename(filepath),
        "length": len(block.split('\n'))
    }

def parse_syntax(block, filepath):
    # <SYNTAX ... = ROUTINE>
    match = re.search(r'<SYNTAX\s+([^=]+)=\s+([^>]+)>', block, re.IGNORECASE)
    if match:
        syntax_str = match.group(1).strip()
        routine_names = match.group(2).strip().split()
        main_routine = routine_names[0]
        
        # Determine the user-facing command verb (first word of syntax)
        verb_match = re.match(r'^([A-Z0-9$#-]+)', syntax_str)
        verb_word = verb_match.group(1) if verb_match else "UNKNOWN"
        
        if verb_word not in verbs:
            verbs[verb_word] = []
            
        verbs[verb_word].append({
            "syntax": syntax_str,
            "routine": main_routine,
            "extra_routines": routine_names[1:],
            "file": os.path.basename(filepath)
        })

def generate_report():
    print("# ZIL Codebase Analysis Checks\n")
    
    # 1. Objects
    print("## Objects\n")
    print("| Name | Location | Description | Flags | Action | File |")
    print("|---|---|---|---|---|---|")
    for name in sorted(objects.keys()):
        obj = objects[name]
        print(f"| {name} | {obj['in'] or '-'} | {clean(obj['desc'])} | {clean(obj['flags'])} | {obj['action'] or '-'} | {obj['file']} |")
    print("\n")
    
    # 2. Rooms
    print("## Rooms\n")
    print("| Name | Description | Flags | Action | File |")
    print("|---|---|---|---|---|")
    for name in sorted(rooms.keys()):
        rm = rooms[name]
        print(f"| {name} | {clean(rm['desc'])} | {clean(rm['flags'])} | {rm['action'] or '-'} | {rm['file']} |")
    print("\n")

    # 3. Actions
    print("## Action Routines\n")
    print("| Routine Name | Associated Object | Type |")
    print("|---|---|---|")
    for name in sorted(actions.keys()):
        act = actions[name]
        print(f"| {name} | {act['object']} | {act['type']} |")
    print("\n")

    # 4. Verbs
    print("## Verbs and Syntax\n")
    print("| Verb | Syntax | Routine | Pre-Action | File |")
    print("|---|---|---|---|---|")
    for v in sorted(verbs.keys()):
        for entry in verbs[v]:
            pre = entry['extra_routines'][0] if entry['extra_routines'] else "-"
            print(f"| {v} | {clean(entry['syntax'])} | {entry['routine']} | {pre} | {entry['file']} |")
    print("\n")

    # 5. Routines (Systems)
    print("## Routines / Systems\n")
    print("| Routine Name | Args | File | Lines (Approx) |")
    print("|---|---|---|---|")
    for name in sorted(routines.keys()):
        rt = routines[name]
        print(f"| {name} | {rt['args']} | {rt['file']} | {rt['length']} |")

def clean(text):
    if not text:
        return "-"
    # Escape pipes for markdown table
    t = text.replace("|", "\\|").replace("\n", " ")
    if len(t) > 50:
        t = t[:47] + "..."
    return t

if __name__ == "__main__":
    files = [f for f in os.listdir(ZIL_DIR) if f.endswith('.zil')]
    for fname in files:
        parse_zil_file(os.path.join(ZIL_DIR, fname))
    
    generate_report()
