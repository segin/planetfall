# Directives

- Only work on one work item at a time! Do not, under any circumstances, batch your work.
- Mark in-progress tasks as in-progress, so that it is clear where you were if interrupted.
- Make a commit and push that commit after each and every completed item. Do not mark the next one as in-progress until after the commit and push are completed.
- Update the current issues to address in this document as you work. Constant updates to this file are required.

# Current issues to address:

- You can enter the escape pod before the explosions open the escape pod door.
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

