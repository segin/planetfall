import re
import os

SYNTAX_FILE = 'zil/syntax.zil'
HEADER_OUT = 'include/syntax_gen.h'
SOURCE_OUT = 'src/syntax_gen.c'

# Mappings for Flags (ZIL to C)
FLAG_MAP = {
    'ACTORBIT': 'F_ACTORBIT',
    'VEHBIT': 'F_VEHBIT',
    'CONTBIT': 'F_CONTBIT',
    'OPENBIT': 'F_OPENBIT',
    'TAKEBIT': 'F_TAKEBIT',
    'READBIT': 'F_READBIT',
    'FOODBIT': 'F_FOODBIT',
    'LIGHTBIT': 'F_LIGHTBIT',
    'RMUNGBIT': 'F_MUNGBIT', 
    'CLIMBBIT': 'F_CLIMBBIT',
    'DOORBIT': 'F_DOORBIT',
    'TOOLBIT': 'F_TOOLBIT',
    'WEARBIT': 'F_WEARBIT',
    'ON-GROUND': 'SEARCH_GROUND',
    'IN-ROOM': 'SEARCH_ROOM',
    'HELD': 'SEARCH_HELD',
    'CARRIED': 'SEARCH_HELD',
    'HAVE': 'SEARCH_HELD',
    'MANY': 'PARSE_MANY',
    'TAKE': 'PARSE_TRY_TAKE',
}

# Prepositions
prepositions = {} 
next_prep_id = 1

# Actions
actions = set()

# Vocab
vocab = {} 

# Syntax Entries
syntax_table = []

def parse_zil():
    global next_prep_id
    
    with open(SYNTAX_FILE, 'r') as f:
        content = f.read()

    content = re.sub(r';.*', '', content)
    content = re.sub(r'"[^"]*"', '', content)

    blocks = re.findall(r'<([^>]+)>', content)
    
    for block in blocks:
        parts = block.split()
        if not parts: continue
        
        cmd = parts[0]
        
        if cmd == 'BUZZ':
            for word in parts[1:]:
                vocab[word] = {'type': 'BUZZ'}
                
        elif cmd == 'SYNONYM':
            if len(parts) >= 2:
                orig = parts[1]
                for syn in parts[2:]:
                     vocab[syn] = {'type': 'SYNONYM', 'target': orig}
                     
        elif cmd == 'SYNTAX':
            body = block[7:].strip()
            
            if '=' not in body:
                print(f"Skipping malformed SYNTAX: {block}")
                continue
                
            lhs, rhs = body.split('=')
            
            rhs_parts = rhs.split()
            action = rhs_parts[0]
            actions.add(action)
            pre_action = rhs_parts[1] if len(rhs_parts) > 1 else "NULL"
            
            lhs_tokens = []
            current_token = ""
            depth = 0
            for char in lhs:
                if char == '(':
                    depth += 1
                    current_token += char
                elif char == ')':
                    depth -= 1
                    current_token += char
                elif char.isspace() and depth == 0:
                    if current_token:
                        lhs_tokens.append(current_token)
                    current_token = ""
                else:
                    current_token += char
            if current_token:
                lhs_tokens.append(current_token)
            
            verb = lhs_tokens[0]
            if verb not in vocab:
                vocab[verb] = {'type': 'VERB'}
            
            entry = {
                'verb': verb,
                'prep1': 'NULL',
                'prep2': 'NULL',
                'obj1_present': False,
                'obj2_present': False,
                'obj1_flags': [],
                'obj2_flags': [],
                'action': action,
                'pre_action': pre_action,
                'prep1_loc': 0, # 0=None, 1=Before O1, 2=After O1, 3=After O2
                'prep2_loc': 0
            }
            
            obj_idx = 1
            last_obj_idx = 0
            
            i = 1
            while i < len(lhs_tokens):
                tok = lhs_tokens[i]
                
                if tok == 'OBJECT':
                    if obj_idx == 1:
                        entry['obj1_present'] = True
                        last_obj_idx = 1
                        obj_idx = 2
                    elif obj_idx == 2:
                        entry['obj2_present'] = True
                        last_obj_idx = 2
                        obj_idx = 3
                    
                    while i+1 < len(lhs_tokens) and lhs_tokens[i+1].startswith('('):
                        flag_spec = lhs_tokens[i+1][1:-1] 
                        flags = flag_spec.split()
                        if last_obj_idx == 1: 
                            entry['obj1_flags'].extend(flags)
                        elif last_obj_idx == 2:
                            entry['obj2_flags'].extend(flags)
                        i += 1
                        
                elif tok.startswith('('):
                     print(f"Orphaned flags {tok} in {block}")
                    
                else:
                    # Preposition
                    if tok not in vocab:
                         vocab[tok] = {'type': 'PREP'}
                    
                    loc = 0
                    if obj_idx == 1: loc = 1 # Before Obj1
                    elif obj_idx == 2: loc = 2 # After Obj1 (Before Obj2)
                    elif obj_idx == 3: loc = 3 # After Obj2
                    
                    if entry['prep1'] == 'NULL':
                         entry['prep1'] = tok
                         entry['prep1_loc'] = loc
                    else:
                         entry['prep2'] = tok
                         entry['prep2_loc'] = loc
                        
                i += 1
            
            syntax_table.append(entry)

def clean_word(s):
    if s == 'NULL': return 'NULL'
    s = s.lower()
    s = s.replace(r'\#', '#') 
    return f'"{s}"'

def generate_c():
    with open(HEADER_OUT, 'w') as f:
        f.write("#ifndef SYNTAX_GEN_H\n#define SYNTAX_GEN_H\n\n")
        f.write("#include \"planetfall.h\"\n\n")
        
        f.write("// Actions\n")
        f.write("typedef enum {\n")
        f.write("    V_NULL = 0,\n")
        sorted_actions = sorted(list(actions))
        for act in sorted_actions:
            f.write(f"    {act.replace('-', '_')},\n")
        f.write("} ActionID;\n\n")
        
        f.write("// Search Flags\n")
        f.write("#define SEARCH_HELD   (1<<0)\n")
        f.write("#define SEARCH_ROOM   (1<<1)\n")
        f.write("#define SEARCH_GROUND (1<<2)\n")
        f.write("#define PARSE_MANY    (1<<3)\n")
        f.write("#define PARSE_TRY_TAKE (1<<4)\n\n")
        
        f.write("// Preposition Locations\n")
        f.write("#define PREP_LOC_NONE 0\n")
        f.write("#define PREP_LOC_BEFORE_OBJ1 1\n")
        f.write("#define PREP_LOC_AFTER_OBJ1 2\n")
        f.write("#define PREP_LOC_AFTER_OBJ2 3\n\n")

        f.write("typedef enum { VOCAB_UNKNOWN, VOCAB_VERB, VOCAB_PREP, VOCAB_BUZZ, VOCAB_SYNONYM } VocabType;\n\n")

        f.write("typedef struct {\n")
        f.write("    const char* word;\n")
        f.write("    VocabType type;\n")
        f.write("    const char* target; // For synonyms\n")
        f.write("} VocabEntry;\n\n")

        f.write("typedef struct {\n")
        f.write("    const char* verb_word;\n")
        f.write("    const char* prep1;\n")
        f.write("    const char* prep2;\n")
        f.write("    int obj1_present;\n")
        f.write("    int obj2_present;\n")
        f.write("    int prep1_loc;\n")
        f.write("    int prep2_loc;\n")
        f.write("    unsigned int obj1_search;\n")
        f.write("    unsigned int obj2_search;\n")
        f.write("    unsigned int obj1_find;\n")
        f.write("    unsigned int obj2_find;\n")
        f.write("    ActionID action_id;\n")
        f.write("} SyntaxEntry;\n\n")
        
        f.write("extern VocabEntry vocab_table[];\n")
        f.write("extern int vocab_table_size;\n")
        f.write("extern SyntaxEntry syntax_table[];\n")
        f.write("extern int syntax_table_size;\n\n")
        
        f.write("#endif\n")

    with open(SOURCE_OUT, 'w') as f:
        f.write("#include \"syntax_gen.h\"\n\n")
        
        f.write("VocabEntry vocab_table[] = {\n")
        for word, data in vocab.items():
             w_type = data['type']
             c_type = "VOCAB_UNKNOWN"
             target = "NULL"
             
             cw = clean_word(word)
             
             if w_type == 'VERB': c_type = "VOCAB_VERB"
             elif w_type == 'PREP': c_type = "VOCAB_PREP"
             elif w_type == 'BUZZ': c_type = "VOCAB_BUZZ"
             elif w_type == 'SYNONYM': 
                 c_type = "VOCAB_SYNONYM"
                 target = clean_word(data["target"])
             
             f.write(f"    {{ {cw}, {c_type}, {target} }},\n")
        f.write("};\n")
        f.write(f"int vocab_table_size = {len(vocab)};\n\n")

        f.write("SyntaxEntry syntax_table[] = {\n")
        
        for entry in syntax_table:
            def process_flags(flag_list):
                search = []
                find = []
                for fl in flag_list:
                    if fl == 'FIND': continue 
                    if fl in FLAG_MAP:
                        mapped = FLAG_MAP[fl]
                        if mapped.startswith('F_'):
                            find.append(mapped)
                        else:
                            search.append(mapped)
                return " | ".join(search) if search else "0", " | ".join(find) if find else "0"

            o1_search, o1_find = process_flags(entry['obj1_flags'])
            o2_search, o2_find = process_flags(entry['obj2_flags'])
            
            p1 = clean_word(entry["prep1"])
            p2 = clean_word(entry["prep2"])
            if entry["prep1"] == 'NULL': p1 = 'NULL'
            if entry["prep2"] == 'NULL': p2 = 'NULL'
            
            act = entry['action'].replace('-', '_')
            
            o1_pres = 1 if entry['obj1_present'] else 0
            o2_pres = 1 if entry['obj2_present'] else 0
            
            p1_loc = entry['prep1_loc']
            p2_loc = entry['prep2_loc']
            
            v_word = clean_word(entry['verb'])
            
            f.write(f"    {{ {v_word}, {p1}, {p2}, {o1_pres}, {o2_pres}, {p1_loc}, {p2_loc}, {o1_search}, {o2_search}, {o1_find}, {o2_find}, {act} }},\n")
            
        f.write("};\n")
        f.write(f"int syntax_table_size = {len(syntax_table)};\n")

if __name__ == '__main__':
    parse_zil()
    generate_c()
