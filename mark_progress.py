#!/usr/bin/env python3
"""Refresh the checkboxes in zil_analysis.md from the actual C sources.

This used to carry hand-maintained lists of implemented names, which meant the
checklist drifted out of date the moment anyone forgot to append to them -- it
was reporting 79 items done when the port had already built well over twice
that. Derive the state from the source instead so it cannot go stale.

A ZIL name maps to a C identifier by upcasing and swapping hyphens for
underscores, prefixed R_ for rooms and O_ for objects (DECK-NINE -> R_DECK_NINE,
STORAGE-WEST-DOOR -> O_STORAGE_WEST_DOOR). An entity counts as implemented once
something in src/ actually constructs it, and a routine counts once a function
with the corresponding name is defined.
"""

import os
import re
import sys

SRC_DIR = 'src'
ANALYSIS = 'zil_analysis.md'


def read_sources():
    blob = []
    for name in sorted(os.listdir(SRC_DIR)):
        if name.endswith('.c'):
            with open(os.path.join(SRC_DIR, name)) as f:
                blob.append(f.read())
    return '\n'.join(blob)


def zil_to_c(name):
    return name.upper().replace('-', '_')


def constructed_ids(src):
    """IDs the port actually builds, via either construction idiom."""
    found = set()
    # ZObject *o = &objects[O_FOO];
    found.update(re.findall(r'&objects\[([A-Z0-9_]+)\]', src))
    # objects[O_FOO].id = O_FOO;
    found.update(re.findall(r'objects\[([A-Z0-9_]+)\]\s*\.\s*id', src))
    return found


def defined_routines(src):
    """Function names defined in the port, e.g. 'pod_door_f'."""
    return set(re.findall(r'^(?:bool|void|int)\s+([a-z_0-9]+)\s*\(', src,
                          re.MULTILINE))


def handled_verbs(src):
    """Verb ActionIDs the port actually dispatches on, e.g. 'V_TAKE'."""
    return set(re.findall(r'case\s+(V_[A-Z0-9_]+)\s*:', src))


def routine_candidates(zil_name):
    """C function names a ZIL routine might reasonably have been ported to.

    ZIL leans on a -F suffix for action routines and an I- prefix for
    interrupts; the port renders those as _f and routine_/i_ respectively.
    """
    base = zil_name.lower().replace('-', '_')
    names = {base}
    if base.startswith('i_'):
        stem = base[2:]
        names.update({stem, 'routine_' + stem, 'i_' + stem})
    if base.startswith('v_'):
        names.add('perform_' + base[2:])
    if base.startswith('pre_'):
        names.add('pre_' + base[4:])
    return names


def main():
    if not os.path.isdir(SRC_DIR) or not os.path.exists(ANALYSIS):
        sys.exit('run this from the repository root')

    src = read_sources()
    built = constructed_ids(src)
    routines = defined_routines(src)
    verbs = handled_verbs(src)

    with open(ANALYSIS) as f:
        lines = f.readlines()

    section = None
    counts = {}
    out = []

    for line in lines:
        if line.startswith('## '):
            section = line[3:].strip()
            counts.setdefault(section, [0, 0])

        match = re.match(r'^- \[([ xX])\] \*\*([A-Z0-9_-]+)\*\*', line)
        if match and section:
            name = match.group(2)
            c_name = zil_to_c(name)

            if section == 'Rooms':
                done = ('R_' + c_name) in built
            elif section == 'Objects':
                done = ('O_' + c_name) in built or ('R_' + c_name) in built
            elif section == 'Verbs and Syntax':
                # Keyed by verb word, but what matters is the action the syntax
                # line resolves to: "ATTACK OBJECT ... -> V-ATTACK".
                target = re.search(r'->\s*(V-[A-Z0-9$-]+)', line)
                done = False
                if target:
                    action = zil_to_c(target.group(1)).replace('$', '')
                    done = action.replace('V_', 'V_', 1) in verbs
            else:
                done = bool(routine_candidates(name) & routines)

            line = re.sub(r'^- \[[ xX]\]', '- [x]' if done else '- [ ]', line)
            counts[section][0] += 1 if done else 0
            counts[section][1] += 1

        out.append(line)

    with open(ANALYSIS, 'w') as f:
        f.writelines(out)

    total_done = total = 0
    for name, (done, seen) in counts.items():
        if seen:
            print(f'{name:20} {done:4}/{seen:<4} '
                  f'{100.0 * done / seen:5.1f}%')
            total_done += done
            total += seen
    if total:
        print(f'{"TOTAL":20} {total_done:4}/{total:<4} '
              f'{100.0 * total_done / total:5.1f}%')


if __name__ == '__main__':
    main()
