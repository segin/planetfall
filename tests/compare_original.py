#!/usr/bin/env python3
"""Differential test: run the same commands through the port and the real game.

zil/planetfall.beta is a genuine Z-machine v3 build of Planetfall (Release 1,
serial 830517), so with a dumb-terminal interpreter we can drive it with the
same input as ./planetfall and compare what comes back.

Two caveats worth keeping in mind before treating a difference as a bug:

  * The binary is Release 1. The ZIL in zil/ is Release 39 (see
    zil/planetfall.errors). Wording drifted between them -- the beta says
    "total of 80 points" where the source says "out of 80 points" -- and the
    ZIL sources are what this project is translating, so the source wins.
  * Several things are random (Blather and the ambassador turning up, the
    explosion delay, the dream you get). Probes should avoid depending on them,
    and the port's rand() is unseeded besides.

So this reports differences rather than asserting; it is a research tool for
finding wording and behaviour drift, not a pass/fail gate. Exit status is 0
unless something actually failed to run.

Usage:
    python3 tests/compare_original.py            # run every probe
    python3 tests/compare_original.py bulkhead   # only probes matching a name
    python3 tests/compare_original.py -v         # show matching probes too
"""

import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PORT = os.path.join(ROOT, 'planetfall')
STORY = os.path.join(ROOT, 'zil', 'planetfall.beta')
DFROTZ = 'dfrotz'

# Each probe is (name, [commands]). Keep them short and away from the random
# events; every probe implicitly ends with QUIT / Y.
PROBES = [
    ('opening-room',      ['look']),
    ('bulkhead-walk',     ['west']),
    ('bulkhead-in',       ['in']),
    ('bulkhead-open',     ['open bulkhead']),
    ('bulkhead-close',    ['close bulkhead']),
    ('examine-me',        ['examine me']),
    ('smell-me',          ['smell me']),
    ('eat-me',            ['eat me']),
    ('take-me',           ['take me']),
    ('scrub-me',          ['scrub me']),
    ('follow-me',         ['follow me']),
    ('shake-hands',       ['shake hands']),
    ('inventory',         ['inventory']),
    ('read-id-card',      ['read id card']),
    ('examine-brush',     ['examine brush']),
    ('examine-uniform',   ['examine uniform']),
    ('read-towel-absent', ['read towel']),
    ('gangway-up',        ['up']),
    ('deck-eight',        ['up', 'up']),
    ('deck-eight-east',   ['up', 'up', 'east']),
    ('deck-eight-north',  ['up', 'up', 'north']),
    ('reactor-lobby',     ['east']),
    ('reactor-south',     ['east', 'south']),
    ('reactor-east',      ['east', 'east']),
    ('walk-to-pod',       ['walk to pod']),
    ('bad-verb',          ['xyzzy']),
    ('empty-input',       ['take']),
    ('diagnose',          ['diagnose']),
    ('version',           ['version']),
    ('wait',              ['wait']),
]

# Probes where the beta and the Release 39 sources genuinely say different
# things. The sources are what we are translating, so these are expected and are
# reported separately rather than as findings. Verified against zil/globals.zil.
KNOWN_DIFFS = {
    'bulkhead-close': 'IS-CLOSED reads "It is closed!" in the R39 source',
    'smell-me':       'R39 CRETIN-F gained a SMELL branch ("Phew!")',
    'scrub-me':       'R39 CRETIN-F shortened the scrub response',
    'follow-me':      'R39 CRETIN-F gained a FOLLOW branch',
    'shake-hands':    'R39 HANDS-F gained the SHAKE branch',
    'gangway-up':     'R39 LDESC capitalises "Deck Eight"/"Deck Nine"',
    'deck-eight':     'R39 LDESC capitalises "Deck Eight"/"Deck Nine"',
    'deck-eight-east': 'R39 LDESC capitalises "Deck Eight"/"Deck Nine"',
    'deck-eight-north': 'R39 LDESC capitalises "Deck Eight"/"Deck Nine"',
    'version':        'the port reports its own release, not the beta\'s',
    'inventory':      'R1 calls it a "Patrol ID card"; R39 DESC is "ID card"',
    'examine-uniform': 'R1 says "radiation and mosquitoes"; R39 says "mild '
                       'radiation, repels all insects"',
    'read-towel-absent': 'R39 ends the not-here message "here!"; R1 "here."',
    'bad-verb':       'R39 UNKNOWN-WORD quotes the word "like this." and R1 '
                      "'like this'.",
}

STATUS_RE = re.compile(r'Score:\s*-?\d+\s+Moves:\s*\d+')
ANSI_RE = re.compile(r'\x1b\[[0-9;?]*[A-Za-z]')

# Blather and the ambassador wander in on probability rolls, and the two sides
# roll differently, so their interjections would swamp every probe. Strip them
# from both transcripts and compare what is left.
RANDOM_RE = [re.compile(p) for p in (
    r'Ensign First Class Blather swaggers in\..*?arms crossed\.',
    r'Ensign Blather, his uniform immaculate,.*?negligence\.',
    r'"I said to return to your post.*?crimson\.',
    r'Blather, adding fifty more demerits.*?terrorize\.',
    r'Blather, foaming slightly at the mouth.*?decks\.',
    r'Blather enters, looking confused.*?you\.',
    r"The alien ambassador from the planet Blow'k-bibben-Gordo.*?exports\.",
    r'The ambassador (?:introduces|asks|inquires|recites|remarks|offers)'
    r'[^.]*\.',
    r'The ambassador grunts a polite farewell,.*?slime\.',
    r'You hear a distant bellowing.*?danger\.',
)]


def run(cmd, script, timeout=25):
    try:
        out = subprocess.run(cmd, input=script, capture_output=True, text=True,
                             timeout=timeout, cwd=ROOT)
        return out.stdout
    except subprocess.TimeoutExpired:
        return None
    except FileNotFoundError:
        return None


def responses(raw, drop_leading):
    """Split a transcript on '>' prompts into one normalized chunk per command.

    Both interpreters echo a prompt before reading, so the text between prompts
    is the reply to the command before it. drop_leading discards the banner and
    opening room description that precede the first command.
    """
    if raw is None:
        return None

    text = ANSI_RE.sub('', raw)
    kept = []
    for line in text.split('\n'):
        if STATUS_RE.search(line):
            # dfrotz redraws the status bar on the prompt line, so the line
            # reads "> Deck Nine ... Score: 0  Moves: 4466". Drop the bar but
            # keep the prompt, which is what delimits one reply from the next.
            kept.append('>' if '>' in line else '')
            continue
        if line.startswith(('Using normal formatting', 'Loading ')):
            continue
        kept.append(line)
    text = '\n'.join(kept)

    # Chunks are separated by the prompt; a prompt can share a line with output.
    chunks = re.split(r'(?m)^\s*>\s?|(?<=\n)>\s?|>\s', text)
    chunks = [normalize(c) for c in chunks]
    chunks = [c for c in chunks if c]
    return chunks[drop_leading:]


def normalize(chunk):
    """Collapse wrapping differences: one line of space-separated words."""
    chunk = chunk.replace('\r', '')
    # The two interpreters hard-wrap at different widths, so reflow entirely.
    words = chunk.split()
    text = ' '.join(words)
    # dfrotz renders the vertical bar directive as a paragraph break; both sides
    # already lost that in the reflow. Normalize curly punctuation too.
    text = text.replace('’', "'").replace('“', '"').replace('”', '"')
    for pattern in RANDOM_RE:
        text = pattern.sub('', text)
    return ' '.join(text.split()).strip()


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('-')]
    verbose = '-v' in sys.argv[1:]

    if not os.path.exists(PORT):
        sys.exit('build the port first: make')
    if not os.path.exists(STORY):
        sys.exit(f'missing story file: {STORY}')
    if subprocess.run(['which', DFROTZ], capture_output=True).returncode != 0:
        sys.exit(f'{DFROTZ} not found -- install frotz-dumb')

    probes = PROBES
    if args:
        probes = [p for p in PROBES if any(a in p[0] for a in args)]
        if not probes:
            sys.exit(f'no probes match {args}')

    same = diff = broke = known = 0

    for name, commands in probes:
        script = '\n'.join(commands + ['quit', 'y']) + '\n'

        port_raw = run([PORT, '--no-status'], script)
        orig_raw = run([DFROTZ, '-w', '200', '-p', STORY], script)

        # The port prints a banner then the first room; dfrotz prints a status
        # line, banner, intro and first room. Neither is preceded by a prompt,
        # so exactly one leading chunk precedes the first command on each side.
        port = responses(port_raw, 1)
        orig = responses(orig_raw, 1)

        if port is None or orig is None:
            print(f'BROKE {name}: one side failed to run')
            broke += 1
            continue

        mismatches = []
        for i, cmd in enumerate(commands):
            p = port[i] if i < len(port) else '<no output>'
            o = orig[i] if i < len(orig) else '<no output>'
            if p != o:
                mismatches.append((cmd, p, o))

        if not mismatches:
            same += 1
            if verbose:
                print(f'MATCH {name}')
        elif name in KNOWN_DIFFS:
            known += 1
            if verbose:
                print(f'KNOWN {name}: {KNOWN_DIFFS[name]}')
        else:
            diff += 1
            print(f'DIFF  {name}')
            for cmd, p, o in mismatches:
                print(f'      > {cmd}')
                print(f'        port: {p}')
                print(f'        orig: {o}')
            print()

    total = same + diff + broke + known
    print(f'\n{same}/{total} match, {known} known release differences, '
          f'{diff} to look at'
          f'{f", {broke} failed to run" if broke else ""}')
    return 1 if broke else 0


if __name__ == '__main__':
    sys.exit(main())
