
import re

def convert_line(line, section):
    if not line.strip().startswith('|') or '---' in line:
        return line
        
    parts = [p.strip() for p in line.split('|')[1:-1]]
    if not parts:
        return line
        
    # Check if header
    if parts[0] == 'Name' or parts[0] == 'Routine Name' or parts[0] == 'Verb':
        return "" 

    if section == 'Objects':
        # Name | Location | Description | Flags | Action | File
        if len(parts) >= 6:
            name, loc, desc, flags, action, file = parts[:6]
            return f"- [ ] **{name}** ({file}): {desc} [Loc: {loc}, Flags: {flags}, Action: {action}]\n"
    elif section == 'Rooms':
        # Name | Description | Flags | Action | File
        if len(parts) >= 5:
            name, desc, flags, action, file = parts[:5]
            return f"- [ ] **{name}** ({file}): {desc} [Flags: {flags}, Action: {action}]\n"
    elif section == 'Action Routines':
        # Routine Name | Associated Object | Type
        if len(parts) >= 3:
            name, obj, type_ = parts[:3]
            return f"- [ ] **{name}** ({type_}) [Obj: {obj}]\n"
    elif section == 'Verbs and Syntax':
         # Verb | Syntax | Routine | Pre-Action | File
         if len(parts) >= 5:
            verb, syntax, routine, pre, file = parts[:5]
            return f"- [ ] **{verb}**: {syntax} -> {routine} (Pre: {pre}) [{file}]\n"
    elif section == 'Routines / Systems':
        # Routine Name | Args | File | Lines
        if len(parts) >= 3:
            name, args, file = parts[:3]
            return f"- [ ] **{name}** ({file}): Args: {args}\n"

    return line

def main():
    with open('zil_analysis.md', 'r') as f:
        lines = f.readlines()

    with open('zil_analysis_checklist.md', 'w') as f:
        f.write("# Planetfall Implementation Checklist\n\n")
        
        current_section = ""
        for line in lines:
            if line.startswith('## '):
                current_section = line.strip()[3:]
                f.write(f"\n## {current_section}\n")
                continue
            
            if line.strip().startswith('|'):
                converted = convert_line(line, current_section)
                if converted:
                    f.write(converted)
            # Skip empty lines or text lines within tables for now, 
            # but keep main headers. 
            # actually we can ignore other lines or print them?
            # The prompt says "Turn zil_analysis.md into a checklist", so we replace content.
            
    print("Done")

if __name__ == "__main__":
    main()
