# Directives

- Only work on one work item at a time! Do not, under any circumstances, batch your work.
- Mark in-progress tasks as in-progress, so that it is clear where you were if interrupted.
- Make a commit and push that commit after each and every completed item. Do not mark the next one as in-progress until after the commit and push are completed.
- Update the current issues to address in this document as you work. Constant updates to this file are required.

# Differential testing against the real game

`zil/planetfall.beta` is a genuine Z-machine v3 build of Planetfall (Release 1,
serial 830517). With `dfrotz` installed (`pacman -S frotz-dumb`), `make compare`
drives the same input through it and through `./planetfall` and diffs the
replies. It reports findings rather than gating, and is deliberately *not* part
of `make test`, because the binary is Release 1 while the ZIL being ported is
Release 39 -- wording drifted between them, and the sources win. Confirmed
release differences are listed in `KNOWN_DIFFS` in `tests/compare_original.py`
so they do not drown the real signal.

Current: 16/30 probes match, 14 known release differences, 0 real findings.
Add probes as chapters land -- everything the existing set covers now agrees
with the original, modulo catalogued Release 1 wording.

# Current issues to address:

- `dispatch_action` now follows ZIL's PERFORM chain for actor, room `M-BEG`,
  PRSI, PRSO and the verb default. Still missing: the `PREACTIONS` table (the
  port calls its `pre_*` routines ad hoc from inside the `perform_*` functions
  instead) and the container `CONTFCN` step.
- Floyd outstanding: `FLOYD-INTO-LAB` and the Bio Lab clauses of
  `FLOYDS-FAMOUS-DOOR-ROUTINE`, `FLUSH` (which drops the rest of a multi-clause
  order when he gets confused), `I-FLOYD`'s Alfie/Betty shuttle-cabin clauses
  (those rooms are not built), `DEAD-FLOYD-F`, `I-FLOYD-FORAY` and
  `I-CHASE-SCENE` -- the Bio Lab sequence he is famous for.
- `TELEPORT <room>` cannot reach rooms whose only name is their description,
  e.g. "computer room" -- rooms mostly lack synonyms. It is a debug verb, but it
  is the only practical way to test the planet chapters, so it is worth giving
  rooms their ZIL synonyms as each chapter lands.

- Fully implement `zil/planetfall.zil` -- that is, the whole game, since that file
  just includes SYNTAX, MISC, GLOBALS, PARSER, VERBS, COMPONE and COMPTWO. Run
  `python3 mark_progress.py` for the current state; it derives the checklist in
  `zil_analysis.md` from the C sources, so it does not go stale. As of the
  Blather/ambassador work:

  | Section            | Done     |
  |--------------------|----------|
  | Rooms              |  96/105  |
  | Objects            | 120/150  |
  | Action Routines    | 103/136  |
  | Verbs and Syntax   | 128/251  |
  | Routines / Systems | 183/505  |
  | **Total**          | **630/1147** |

  The weakest areas by far are Verbs/Syntax and Routines/Systems -- the world is
  largely built but the verb layer behind it is thin.

  **Agreed strategy: chapter by chapter.** Finish the Feinstein completely, then
  `compone.zil`, then `comptwo.zil`, so each chapter becomes fully playable in
  turn. Remaining Feinstein work, in order:

  1. Player status daemons. `I-SLEEP-WARNINGS`, `I-FALL-ASLEEP`, `DREAMING`,
     `WAKING-UP` and `I-SICKNESS-WARNINGS` do not exist; `I-HUNGER-WARNINGS` is a
     stub on the wrong schedule (ZIL escalates 450/150/100/50 rather than counting
     to 600). All are queued by GO on turn one, but at 3600/2000/1000 ticks they
     only fire well into the planet chapters, and `WAKING-UP` depends on Floyd,
     the canteen and the flask -- so this lands after `compone.zil` is further
     along, not during the Feinstein.
- Parser gaps in `src/parser.c`: "IT" is a stub that always fails to resolve
  (`snarf_objects` bails early) though ZIL tracks P-IT-OBJECT from GO onward;
  disambiguation prints a debug line and silently takes the first match; container
  recursion only descends one level.
- `tools/gen_syntax.py` regenerates `src/syntax_gen.c` and `include/syntax_gen.h`
  from `zil/syntax.zil`, but is not wired into the Makefile, so the generated files
  can drift. It has also leaked a parse artifact into the vocabulary table:
  `{"<synonym", VOCAB_BUZZ, NULL}`.
- Some action routines are written but never attached to their objects, so they are
  dead code. `ground_f` and `patrol_uniform_f` in `src/feinstein_actions.c` are
  currently unattached; `deck_nine_f` and `gangway_f` were too until the bulkhead
  work wired them up. Worth auditing the other `*_actions.c` files for the same.
- `src/main.c` still carries a `legacy_dispatch` switch that `dispatch_action`
  falls through to. It duplicates verb logic with invented text and shadows real
  handlers -- the pod bulkhead bug lived there. The Blather/ambassador work removed
  its ATTACK/KICK/TALK/celery branches; the rest should follow as the verbs get
  proper handlers.
- Status bar has time and no score. The time half is now correct (it shows Galactic
  Standard Time from the MOVES global, matching ZIL). The score field is still
  missing: Planetfall is a `<VERSION ZIP>` game with no TIME flag, so the authentic
  interpreter status line renders SCORE *and* MOVES side by side. Adding it means
  putting `Score: %d` from `game_state.score` into `update_status_bar` in
  `src/main.c`.
- `rand()` is never seeded -- there is no `srand()` call anywhere in `src/`. Every
  playthrough therefore draws exactly the same "random" numbers, so the explosion
  delay, `number_needed`, `chemical_required`, and the Blather/ambassador
  appearance rolls are all fixed constants in practice. Seeding fixes that but
  makes the 61 scripted tests non-deterministic, so the test harness likely needs
  a fixed-seed switch at the same time. 

