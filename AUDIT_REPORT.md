# Codebase Audit Report: Planetfall Remake

## 1. Executive Summary

The codebase represents a text adventure game engine written in C, inspired by Infocom's Planetfall. It features a custom parser, an event system, and a basic object model. While functional, the codebase exhibits several security vulnerabilities, implementation shortcomings, and areas for modernization. The most critical issue is the unsafe deserialization of binary data during game loading, which poses a significant security risk. Additionally, the reliance on global state and non-portable binary formats limits the engine's robustness and cross-platform compatibility.

## 2. Security Audit

### 2.1. Critical Vulnerabilities

#### Binary Deserialization (CWE-502)
**Location:** `src/savegame.c` (`restore_game` function)
**Description:** The `restore_game` function uses `fread` to read raw binary data directly into game structures (`ZGameState`, `SavedObject`). It trusts the file content implicitly.
**Impact:** A malicious save file could corrupt memory, overwrite critical game state variables (e.g., array indices, pointers), and potentially lead to arbitrary code execution or application crashes.
**Recommendation:**
- Implement a serialization format that validates data upon loading (e.g., JSON, or a TLV binary format with checksums).
- Sanitize all loaded values (e.g., ensure `current_room` is a valid ID).

### 2.2. Medium Risks

#### File I/O Handling
**Location:** `src/savegame.c`
**Description:** The save and restore functions use a hardcoded filename `"planetfall.sav"`. While this prevents directory traversal attacks from user input (since the user cannot specify the filename), it is inflexible. If the filename were ever to be taken from user input without validation, it would be a high risk.
**Recommendation:** If user input is added for filenames, ensure strict validation (allowlist of characters, no directory separators). For now, consider allowing save slots (e.g., `planetfall_01.sav`).

#### Predictable Random Number Generation
**Location:** `src/actions.c` (`itake` function uses `rand()`)
**Description:** The game uses `rand()` for probability checks (e.g., fumbling an item). There is no visible call to `srand()` with a unique seed (like `time(NULL)`).
**Impact:** The random number sequence will be identical every time the game is run, making "random" events deterministic and predictable.
**Recommendation:** Call `srand(time(NULL))` (or a better seed source) in `main.c` at startup.

### 2.3. Low Risks

#### Buffer Handling
**Location:** `src/parser.c`
**Description:** The parser uses fixed-size buffers (`MAX_WORD_LEN`, `MAX_TOKENS`). While `strncpy` and manual null-termination logic seem correct, the reliance on fixed buffers can be brittle.
**Impact:** Input longer than the buffer is truncated. This is generally handled safely here but could lead to unexpected parsing behavior for very long words.
**Recommendation:** Use dynamic allocation or rigorous bounds checking (which seems mostly present).

## 3. Implementation Audit

### 3.1. Portability Issues
**Struct Padding and Endianness:** The game saves raw C structs (`fwrite`). This makes save files incompatible between different architectures (e.g., x86 vs ARM, 32-bit vs 64-bit) due to differences in struct packing and byte order.
**Terminal Control:** The game uses hardcoded ANSI escape codes in `main.c` and `output.c` (implied). This limits portability to non-ANSI terminals (e.g., Windows Command Prompt without VT processing enabled).

### 3.2. Code Structure & Design
**Global State:** The engine relies heavily on global variables (`objects`, `game_state`, `player`, `current_room`). This makes unit testing difficult and increases the risk of side effects.
**Hardcoded Logic:** `main.c` contains game logic for specific verbs (`V_QUIT`, `V_BOARD`, `V_OPEN`) mixed with the input loop. This violates the separation of concerns. These should be moved to `actions.c` or specific verb handlers.
**Error Handling:** Return values from `malloc` (if used in future) or file operations are not consistently checked or handled gracefully beyond printing a message.

### 3.3. Specific Logic Flaws (from AGENTS.md)
- **Escape Pod Logic:** The player can enter the escape pod before the intended event (explosions/door opening). This is a logic error in `V_BOARD` in `main.c`.
- **Time Limits:** The game timer might be too strict compared to the original, making exploration difficult.
- **Quit Confirmation:** `V_QUIT` exits immediately without asking "Are you sure?".
- **Status Bar:** The status bar shows time but lacks the score, which is standard for Z-machine games.

## 4. Feature Suggestions

### 4.1. Save/Load System
- **JSON/Text Format:** Switch to a text-based save format. This fixes the portability issue and makes debugging save states easier.
- **Multiple Slots:** Allow the user to specify a save slot or filename.

### 4.2. User Experience (UX)
- **Command History:** Implement a command history feature (using `readline` or `linenoise`) so users can press Up/Down to recall commands.
- **Tab Completion:** Auto-complete object names.
- **Better Status Bar:** Use `ncurses` or a more robust terminal library to render the status bar reliably across different terminal sizes.
- **Color Output:** Use colors to distinguish between narration, dialogue, and system messages.

### 4.3. Scripting & Testing
- **Scripting Interface:** The `is_scripting` check suggests some automation. Expose more internal state to scripts to allow for comprehensive regression testing.
- **Unit Tests:** Expand `tests/` to cover specific game logic (puzzles), not just engine core functions.

## 5. Recommended Next Steps (Action Plan)

1.  **Refactor `main.c`:** Move verb logic (`V_BOARD`, `V_QUIT`, etc.) to `actions.c`.
2.  **Fix Critical Logic:** Update `V_BOARD` to check the `F_OPENBIT` of `O_POD_DOOR` or the `EVT_POD_TRIP` status before allowing entry.
3.  **Implement Safe Save/Load:** Replace `fwrite`/`fread` of structs with a field-by-field serialization or a JSON library.
4.  **Enhance UX:** Add a confirmation prompt to `V_QUIT` and update `update_status_bar` to verify terminal dimensions and include score.
