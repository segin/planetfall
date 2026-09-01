#!/usr/bin/env python3
"""Compare every ported object's flags against the ZIL that defines it.

A missing flag is a quiet bug: the chronometer was built without MUNGBIT, so it
could not be broken and the whole MUNGED-TIME mechanic was unreachable even
though it was implemented. Nothing fails loudly when a bit is absent -- the
behaviour it gates simply never happens -- so it is worth checking wholesale.

Reads the (FLAGS ...) list off each <OBJECT>/<ROOM> in zil/, finds where the C
constructs the corresponding O_/R_ id, and diffs the two sets.

Usage:
    python3 tools/check_flags.py           # objects the port has built
    python3 tools/check_flags.py --all     # include ones it has not
"""

import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ZIL_DIR = os.path.join(ROOT, 'zil')
SRC_DIR = os.path.join(ROOT, 'src')

# ZIL flags the port deliberately spells differently, or does not model.
ALIASES = {'RMUNGBIT': 'MUNGBIT'}
# Flags with no C equivalent; not worth reporting as missing.
IGNORED = {'SCRAMBLEDBIT'}


def zil_flags():
    """{NAME: (kind, {FLAG, ...})} for every object and room in the sources."""
    out = {}
    for name in sorted(os.listdir(ZIL_DIR)):
        if not name.endswith('.zil'):
            continue
        text = open(os.path.join(ZIL_DIR, name), encoding='latin-1').read()
        # Each definition runs to the start of the next top-level form.
        for m in re.finditer(r'^<(OBJECT|ROOM)\s+([A-Z0-9?-]+)(.*?)(?=^<)',
                             text, re.S | re.M):
            kind, obj, body = m.group(1), m.group(2), m.group(3)
            fm = re.search(r'\(FLAGS([^)]*)\)', body)
            flags = set(fm.group(1).split()) if fm else set()
            flags = {ALIASES.get(f, f) for f in flags} - IGNORED
            out[obj] = (kind, flags)
    return out


def c_flags():
    """{C_ID: {FLAG, ...}} for everything the port constructs."""
    blob = []
    for name in sorted(os.listdir(SRC_DIR)):
        if name.endswith('.c'):
            blob.append(open(os.path.join(SRC_DIR, name)).read())
    text = '\n'.join(blob)

    out = {}
    # Construction runs from "&objects[ID]" to the next one; the flags
    # assignment inside it is what we want. Also catch objects[ID].flags = ...
    for m in re.finditer(r'objects\[([A-Z0-9_]+)\]\s*\.\s*flags\s*=\s*([^;]+);',
                         text):
        out.setdefault(m.group(1), set()).update(
            re.findall(r'F_([A-Z0-9_]+)', m.group(2)))

    blocks = re.split(r'&objects\[([A-Z0-9_]+)\]', text)
    for i in range(1, len(blocks) - 1, 2):
        obj, body = blocks[i], blocks[i + 1]
        fm = re.search(r'->flags\s*=\s*([^;]+);', body)
        if fm:
            out.setdefault(obj, set()).update(
                re.findall(r'F_([A-Z0-9_]+)', fm.group(1)))
    return out


def main():
    show_all = '--all' in sys.argv
    zil, built = zil_flags(), c_flags()

    missing_obj = mismatches = checked = 0
    for name in sorted(zil):
        kind, want = zil[name]
        c_id = ('R_' if kind == 'ROOM' else 'O_') + name.replace('-', '_')
        have = built.get(c_id)
        if have is None and kind == 'OBJECT':
            have = built.get('R_' + name.replace('-', '_'))
        if have is None:
            missing_obj += 1
            if show_all:
                print(f'not built  {name}')
            continue

        checked += 1
        absent = want - have
        extra = have - want
        if absent or extra:
            mismatches += 1
            print(f'{name}')
            if absent:
                print(f'    missing in C: {" ".join(sorted(absent))}')
            if extra:
                print(f'    extra in C:   {" ".join(sorted(extra))}')

    print(f'\n{checked} built, {mismatches} with flag differences, '
          f'{missing_obj} not built yet')
    return 0


if __name__ == '__main__':
    sys.exit(main())
