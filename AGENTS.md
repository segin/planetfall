# Directives

- Only work on one work item at a time! Do not, under any circumstances, batch your work.
- Mark in-progress tasks as in-progress, so that it is clear where you were if interrupted.
- Make a commit and push that commit after each and every completed item. Do not mark the next one as in-progress until after the commit and push are completed.
- Update the current issues to address in this document as you work. Constant updates to this file are required.

# Current issues to address:

- Fully implement `zil/planetfall.zil` -- that is, the whole game, since that file
  just includes SYNTAX, MISC, GLOBALS, PARSER, VERBS, COMPONE and COMPTWO. The
  checklist in `zil_analysis.md` tracks this at 79 of 1171 items done. Needs to be
  broken into per-file or per-region chunks and worked one at a time.
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

