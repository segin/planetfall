#include "parser.h"
#include "syntax_gen.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS 32
#define MAX_WORD_LEN 64

typedef struct {
  char word[MAX_WORD_LEN];
  VocabEntry *vocab;
  bool comma_after; // "FLOYD, GO NORTH" -- marks the end of an actor clause
} Token;

Token tokens[MAX_TOKENS];
int num_tokens = 0;
Command current_cmd;

VocabEntry *lookup_vocab(const char *word) {
  for (int i = 0; i < vocab_table_size; i++) {
    if (strcasecmp(word, vocab_table[i].word) == 0) {
      return &vocab_table[i];
    }
  }
  for (int i = 0; i < vocab_table_size; i++) {
    if (strlen(vocab_table[i].word) == 6 &&
        strncasecmp(word, vocab_table[i].word, 6) == 0) {
      return &vocab_table[i];
    }
  }
  return NULL;
}

void tokenize(char *input) {
  num_tokens = 0;

  char buffer[MAX_WORD_LEN];
  int buf_idx = 0;

  for (int i = 0; input[i]; i++) {
    char c = tolower(input[i]);
    if (isspace(c) || c == ',' || c == '.') {
      if (buf_idx > 0) {
        buffer[buf_idx] = '\0';
        if (num_tokens < MAX_TOKENS) {
          strncpy(tokens[num_tokens].word, buffer, MAX_WORD_LEN);
          tokens[num_tokens].vocab = lookup_vocab(buffer);
          tokens[num_tokens].comma_after = false;
          num_tokens++;
        }
        buf_idx = 0;
      }
      // Remember the comma: it is what separates "FLOYD" from the order you
      // are giving him.
      if (c == ',' && num_tokens > 0)
        tokens[num_tokens - 1].comma_after = true;
    } else {
      if (buf_idx < MAX_WORD_LEN - 1) {
        buffer[buf_idx++] = c;
      }
    }
  }
  if (buf_idx > 0) {
    buffer[buf_idx] = '\0';
    if (num_tokens < MAX_TOKENS) {
      strncpy(tokens[num_tokens].word, buffer, MAX_WORD_LEN);
      tokens[num_tokens].vocab = lookup_vocab(buffer);
      tokens[num_tokens].comma_after = false;
      num_tokens++;
    }
  }
}

int snarf_objects(int start, int end, unsigned int search_flags,
                  unsigned int find_flags, ZObjectID *out_list, int max_count);

// "FLOYD, GO NORTH" -- if the input opens with an actor clause followed by a
// comma, pull it off the front and report who is being ordered about. The rest
// of the command then parses exactly as if you had typed it yourself.
static ZObjectID snarf_actor(void) {
  for (int k = 0; k < num_tokens; k++) {
    if (!tokens[k].comma_after)
      continue;

    ZObjectID candidates[4];
    int found = snarf_objects(0, k + 1, 0, 0, candidates, 4);
    if (found > 0 && obj_has_flag(candidates[0], F_ACTORBIT) &&
        candidates[0] != player) {
      ZObjectID actor = candidates[0];
      // Shift the order itself down to the front.
      int shift = k + 1;
      for (int i = shift; i < num_tokens; i++)
        tokens[i - shift] = tokens[i];
      num_tokens -= shift;
      return actor;
    }
    break; // only the first clause can name an actor
  }
  return NOTHING;
}

bool word_matches_object(ZObject *obj, const char *word) {
  for (int i = 0; i < 5; i++) {
    if (obj->synonyms[i] && strcasecmp(word, obj->synonyms[i]) == 0)
      return true;
  }
  for (int i = 0; i < 5; i++) {
    if (obj->adjectives[i] && strcasecmp(word, obj->adjectives[i]) == 0)
      return true;
  }
  return false;
}

// Is this word in any dictionary at all -- the verb/preposition table, an
// object's synonyms or adjectives, or one of the words the parser skips over?
// ZIL's lexer knows every vocabulary word up front, so it can tell an
// unrecognised word from a sentence it merely cannot fit to a syntax line.
static bool word_is_known(const char *word) {
  if (lookup_vocab(word))
    return true;
  if (isdigit((unsigned char)word[0]))
    return true;

  static const char *skipped[] = {"the", "a", "an", "all", "it", "of"};
  for (size_t i = 0; i < sizeof(skipped) / sizeof(skipped[0]); i++) {
    if (strcasecmp(word, skipped[i]) == 0)
      return true;
  }

  for (int i = 0; i < MAX_OBJECTS; i++) {
    if (objects[i].id == NOTHING)
      continue;
    if (word_matches_object(&objects[i], word))
      return true;
  }
  return false;
}

bool phrase_matches_object(ZObject *obj, int start, int end) {
  for (int i = start; i < end; i++) {
    if (word_matches_object(obj, tokens[i].word))
      continue;
    // Skip a leading article -- but only leading. "A" is an article in "a
    // brush" and an adjective in "dorm a", and skipping it everywhere made
    // every dorm answer to "dorm a".
    if (i == start &&
        (strcmp(tokens[i].word, "the") == 0 ||
         strcmp(tokens[i].word, "a") == 0 || strcmp(tokens[i].word, "an") == 0))
      continue;
    return false;
  }
  return true;
}

// WHICH-PRINT (parser.zil). More than one thing answers to what was typed, so
// ask, listing them: "Which door do you mean, the wide bulkhead or the narrow
// bulkhead?" The answer is handled like an orphan reply -- see below.
#define MAX_WHICH 8
static ZObjectID which_candidates[MAX_WHICH];
static int which_count = 0;
static char which_input[256];
static bool which_pending = false;
// Set for the duration of a re-parse, so the ambiguity resolves to the object
// the player picked instead of asking again.
static ZObjectID which_choice = NOTHING;
// Tells parse_command a question was asked, so it stays quiet rather than
// adding "You can't see any ... here!" on top.
static bool asked_which = false;
// The action of the syntax line currently being tried, so TELEPORT can be
// treated as taking a room.
static int parsing_action = 0;

static void which_print(ZObjectID *list, int count, int start, int end) {
  tellf("Which ");
  for (int i = start; i < end; i++) {
    if (i > start)
      tellf(" ");
    tellf("%s", tokens[i].word);
  }
  tellf(" do you mean, ");
  for (int i = 0; i < count; i++) {
    tellf("the %s", objects[list[i]].description);
    int left = count - i;
    if (left == 2) {
      // ZIL puts a comma before the "or" only when there were more than two
      // to begin with.
      if (count != 2)
        tellf(",");
      tellf(" or ");
    } else if (left > 2) {
      tellf(", ");
    }
  }
  tellf("?\n");
}

// Scan the direct children of `parent` (and one level into open containers) for
// objects matching the noun clause spanning tokens [start, end), appending hits
// to out_list. Callers invoke this once per scope (inventory, room, globals),
// so it dedups against what is already in the list.
static void check_list(ZObjectID parent, int start, int end,
                       unsigned int find_flags, bool is_all,
                       ZObjectID *out_list, int max_count, int *count) {
  ZObjectID curr = objects[parent].child;
  while (curr != NOTHING) {
    if (!obj_has_flag(curr, F_INVISIBLE)) {

      bool flags_match = true;
      if (is_all && find_flags != 0) {
        if ((objects[curr].flags & find_flags) == 0) {
          flags_match = false;
        }
      }

      if (flags_match) {
        bool match = false;
        if (is_all) {
          match = true; // "ALL" matches everything that passes flag check
        } else {
          if (phrase_matches_object(&objects[curr], start, end)) {
            match = true;
          }
        }

        if (match) {
          // Check if already in list (dedup if searching multiple scopes)
          bool exists = false;
          for (int k = 0; k < *count; k++)
            if (out_list[k] == curr)
              exists = true;

          if (!exists && *count < max_count) {
            out_list[(*count)++] = curr;
          }
        }
      }
    }

    // Recurse for containers
    if (obj_has_flag(curr, F_CONTBIT) && obj_has_flag(curr, F_OPENBIT)) {
      ZObjectID inner = objects[curr].child;
      while (inner != NOTHING) {
        if (!obj_has_flag(inner, F_INVISIBLE)) {
          bool inner_flags_match = true;
          if (is_all && find_flags != 0) {
            if ((objects[inner].flags & find_flags) == 0)
              inner_flags_match = false;
          }

          bool match = false;
          if (is_all)
            match = true;
          else if (phrase_matches_object(&objects[inner], start, end))
            match = true;

          if (inner_flags_match && match) {
            bool exists = false;
            for (int k = 0; k < *count; k++)
              if (out_list[k] == inner)
                exists = true;
            if (!exists && *count < max_count)
              out_list[(*count)++] = inner;
          }
        }
        inner = objects[inner].sibling;
      }
    }
    curr = objects[curr].sibling;
  }
}

// Returns count found
int snarf_objects(int start, int end, unsigned int search_flags,
                  unsigned int find_flags, ZObjectID *out_list, int max_count) {
  if (start >= end)
    return 0;

  int count = 0;
  if (end - start == 1 && isdigit((unsigned char)tokens[start].word[0])) {
    current_cmd.parsed_number = atoi(tokens[start].word);
    out_list[0] = O_INTNUM;
    return 1;
  }

  bool is_all =
      (strcasecmp(tokens[start].word, "all") == 0 && (end - start == 1));
  bool is_it =
      (strcasecmp(tokens[start].word, "it") == 0 && (end - start == 1));

  // "IT" stands for whatever was last referred to, provided it is still within
  // reach. MAIN-LOOP substitutes P-IT-OBJECT before the command is performed;
  // doing it here comes to the same thing and keeps it out of the game loop.
  if (is_it) {
    if (game_state.it_object != NOTHING && is_here(game_state.it_object)) {
      out_list[0] = game_state.it_object;
      return 1;
    }
    tellf("I don't see what you are referring to.\n");
    game_state.it_object = NOTHING;
    asked_which = true; // suppress the follow-on "You can't see any it here!"
    return 0;
  }

  if (search_flags & SEARCH_ALL) {
    // Iterate ALL objects
    // Assuming we have access to total objects count or MAX_OBJECTS
    // objects array is global.
    for (int i = 0; i < MAX_OBJECTS;
         i++) { // Need MAX_OBJECTS access or sentinel
      // If MAX_OBJECTS not available, use loop until some sentinel?
      // objects is defined in planetfall.h as extern ZObject objects[];
      // MAX_OBJECTS is likely in planetfall.h or ids.h
      if (objects[i].id != NOTHING) { // Simple check
        if (phrase_matches_object(&objects[i], start, end)) {
          bool exists = false;
          for (int k = 0; k < count; k++)
            if (out_list[k] == i)
              exists = true;
          if (!exists && count < max_count)
            out_list[count++] = i;
        }
      }
    }
  } else {
    bool explicit_scope = (search_flags & (SEARCH_HELD | SEARCH_ROOM | SEARCH_GROUND)) != 0;
    // A syntax line's scope flags bias where ZIL looks; they do not put what
    // you are carrying out of reach. "TAKE CARD" is declared (ON-GROUND) yet
    // the real game happily takes the ID card out of your own uniform pocket,
    // so the held scope is always searched.
    bool want_held = true;
    // <DO-SL ,WINNER ...> -- when you are ordering someone about, what they are
    // carrying comes into scope too, so "FLOYD, DROP THE BRUSH" finds it in his
    // compartments. It is an extra scope rather than a replacement: FLOYD-F
    // needs to resolve the object even when he hasn't got it, so that it can
    // answer "Floyd does not one of those have!".
    if (want_held && current_cmd.winner != NOTHING &&
        current_cmd.winner != player)
      check_list(current_cmd.winner, start, end, find_flags, is_all, out_list,
                 max_count, &count);
    if (want_held)
      check_list(player, start, end, find_flags, is_all, out_list, max_count, &count);
    if (!explicit_scope || (search_flags & (SEARCH_ROOM | SEARCH_GROUND)) || (search_flags & PARSE_TRY_TAKE))
      check_list(current_room, start, end, find_flags, is_all, out_list, max_count, &count);
  }

  check_list(OBJ_GLOBAL_OBJECTS, start, end, find_flags, is_all, out_list, max_count, &count);
  for (int g = 0; g < 10; g++) {
    ZObjectID gobj = objects[current_room].globals[g];
    if (gobj != NOTHING && gobj > 0 && gobj < MAX_OBJECTS) {
      if (!obj_has_flag(gobj, F_INVISIBLE)) {
        bool flags_match = true;
        if (is_all && find_flags != 0 && (objects[gobj].flags & find_flags) == 0) {
          flags_match = false;
        }
        if (flags_match) {
          bool match = is_all || phrase_matches_object(&objects[gobj], start, end);
          if (match) {
            bool exists = false;
            for (int k = 0; k < count; k++) {
              if (out_list[k] == gobj)
                exists = true;
            }
            if (!exists && count < max_count) {
              out_list[count++] = gobj;
            }
          }
        }
      }
    }
  }

  // Disambiguation. Silently taking the first match is how you end up scrubbing
  // the wrong thing without being told.
  if (!is_all && count > 1 && parsing_action == V_TELEPORT) {
    // TELEPORT is the port's own debug verb and it goes to rooms, so a door or
    // a keycard sharing the room's name is not a real ambiguity. Every room
    // carries RLANDBIT or RWATERBIT.
    int rooms = 0;
    for (int k = 0; k < count; k++) {
      if (obj_has_flag(out_list[k], F_RLANDBIT) ||
          obj_has_flag(out_list[k], F_RWATERBIT))
        out_list[rooms++] = out_list[k];
    }
    if (rooms > 0)
      count = rooms;
  }

  if (!is_all && count > 1) {
    // Re-parsing after the player answered: take the object they picked.
    if (which_choice != NOTHING) {
      for (int k = 0; k < count; k++) {
        if (out_list[k] == which_choice) {
          out_list[0] = which_choice;
          return 1;
        }
      }
    }
    which_print(out_list, count, start, end);
    which_count = count < MAX_WHICH ? count : MAX_WHICH;
    for (int k = 0; k < which_count; k++)
      which_candidates[k] = out_list[k];
    which_pending = true;
    asked_which = true;
    return 0;
  }

  return count;
}

// Does any syntax line start with this word? Used to tell an answer to an
// orphan question ("FLOOR") from a fresh command ("LOOK"), which is what ZIL's
// P-OFLAG handling comes down to.
static bool is_verb_word(const char *word) {
  for (int i = 0; i < syntax_table_size; i++) {
    if (strcasecmp(word, syntax_table[i].verb_word) == 0)
      return true;
  }
  VocabEntry *v = lookup_vocab(word);
  if (v && v->type == VOCAB_SYNONYM && v->target) {
    for (int i = 0; i < syntax_table_size; i++) {
      if (strcasecmp(v->target, syntax_table[i].verb_word) == 0)
        return true;
    }
  }
  return false;
}

// ORPHAN (parser.zil). When a verb needs an object and none was given we ask
// for one; the player's reply is a bare noun clause, not a sentence, so it is
// rejoined to the verb still waiting for it. Cleared as soon as anything else
// is typed, exactly as P-OFLAG is.
static char orphan_verb[MAX_WORD_LEN];
static bool orphan_pending = false;

// Put the remembered verb back on the front of the reply and let the ordinary
// syntax machinery deal with the result.
static void adopt_orphan(void) {
  if (num_tokens >= MAX_TOKENS)
    return;
  for (int i = num_tokens; i > 0; i--)
    tokens[i] = tokens[i - 1];
  num_tokens++;
  snprintf(tokens[0].word, MAX_WORD_LEN, "%s", orphan_verb);
  tokens[0].vocab = lookup_vocab(tokens[0].word);
  tokens[0].comma_after = false;
}

// Name the noun rather than the article when reporting that we cannot see it.
static const char *noun_of(int start, int end) {
  for (int i = start; i < end; i++) {
    if (strcasecmp(tokens[i].word, "the") && strcasecmp(tokens[i].word, "a") &&
        strcasecmp(tokens[i].word, "an"))
      return tokens[i].word;
  }
  return tokens[start].word;
}

bool parse_command(char *input, Command *cmd) {
  // Kept so a "Which do you mean...?" answer can re-run the original command.
  // Held aside until we know this line is not itself that answer -- committing
  // it up here would overwrite the very command we need to run again.
  char this_input[sizeof(which_input)];
  snprintf(this_input, sizeof(this_input), "%s", input);

  asked_which = false;
  tokenize(input);
  if (num_tokens == 0) {
    tellf("I beg your pardon?\n");
    return false;
  }

  // UNKNOWN-WORD (parser.zil): report a word that is in no dictionary before
  // trying to fit the sentence to anything.
  for (int i = 0; i < num_tokens; i++) {
    if (!word_is_known(tokens[i].word)) {
      tellf("I don't know the word \"%s.\"\n", tokens[i].word);
      return false;
    }
  }

  // "Which do you mean...?" is outstanding: narrow the candidates by what was
  // just typed, then run the original command again with the choice pinned.
  bool had_which = which_pending;
  which_pending = false;
  if (had_which && !is_verb_word(tokens[0].word)) {
    ZObjectID pick = NOTHING;
    int matches = 0;
    for (int k = 0; k < which_count; k++) {
      if (phrase_matches_object(&objects[which_candidates[k]], 0, num_tokens)) {
        pick = which_candidates[k];
        matches++;
      }
    }
    if (matches == 1) {
      char again[sizeof(which_input)];
      snprintf(again, sizeof(again), "%s", which_input);
      which_choice = pick;
      bool ok = parse_command(again, cmd);
      which_choice = NOTHING;
      return ok;
    }
    // Still ambiguous, or nothing matched: fall through and read it as an
    // ordinary command, which will complain in its own way.
  }

  // Not an answer to anything: this is the line to re-run if it turns out to
  // be ambiguous itself.
  if (which_choice == NOTHING)
    snprintf(which_input, sizeof(which_input), "%s", this_input);

  // A question is outstanding, and this does not look like a fresh command:
  // treat it as the answer and rejoin it to the verb that asked.
  bool had_orphan = orphan_pending;
  orphan_pending = false;
  if (had_orphan && !is_verb_word(tokens[0].word))
    adopt_orphan();

  // Set when some syntax line for this verb wanted a direct object and the
  // player did not supply one, so we can ask for it the way ORPHAN does.
  bool wanted_object = false;

  cmd->prso_count = 0;
  cmd->prsi = NOTHING;
  cmd->winner = snarf_actor();
  if (cmd->winner != NOTHING && num_tokens == 0) {
    tellf("\"I don't understand! What are you referring to?\"\n");
    return false;
  }

  // Implicit Verb: Direction
  for (int dir = O_NORTH; dir <= O_OUT; dir++) {
    if (word_matches_object(&objects[dir], tokens[0].word)) {
      if (num_tokens == 1) {
        cmd->verb = V_WALK;
        cmd->prso_list[0] = dir;
        cmd->prso_count = 1;
        cmd->prsi = NOTHING;
        return true;
      }
    }
  }

  for (int i = 0; i < syntax_table_size; i++) {
    SyntaxEntry *se = &syntax_table[i];

    const char *verb_word = tokens[0].word;
    if (tokens[0].vocab) {
      if (tokens[0].vocab->type == VOCAB_SYNONYM)
        verb_word = tokens[0].vocab->target;
      else if (tokens[0].vocab->type == VOCAB_VERB)
        verb_word = tokens[0].vocab->word;
    }

    if (strcasecmp(tokens[0].word, se->verb_word) != 0 &&
        strcasecmp(verb_word, se->verb_word) != 0) {
      continue;
    }

    int input_prep1_idx = -1;
    int input_prep2_idx = -1;

    for (int k = 1; k < num_tokens; k++) {
      if (tokens[k].vocab) {
        if (tokens[k].vocab->type == VOCAB_PREP ||
            (tokens[k].vocab->type == VOCAB_SYNONYM &&
             lookup_vocab(tokens[k].vocab->target) &&
             lookup_vocab(tokens[k].vocab->target)->type == VOCAB_PREP)) {
          if (input_prep1_idx == -1)
            input_prep1_idx = k;
          else if (input_prep2_idx == -1)
            input_prep2_idx = k;
        }
      }
    }

    bool p1_match = false;
    if (se->prep1 == NULL) {
      if (input_prep1_idx == -1)
        p1_match = true;
    } else {
      if (input_prep1_idx != -1) {
        const char *prep1_word = tokens[input_prep1_idx].word;
        if (tokens[input_prep1_idx].vocab &&
            tokens[input_prep1_idx].vocab->type == VOCAB_SYNONYM) {
          prep1_word = tokens[input_prep1_idx].vocab->target;
        }
        if (strcasecmp(tokens[input_prep1_idx].word, se->prep1) == 0 ||
            strcasecmp(prep1_word, se->prep1) == 0) {
          p1_match = true;
        }
      }
    }

    bool p2_match = false;
    if (se->prep2 == NULL) {
      if (input_prep2_idx == -1)
        p2_match = true;
    } else {
      if (input_prep2_idx != -1) {
        const char *prep2_word = tokens[input_prep2_idx].word;
        if (tokens[input_prep2_idx].vocab &&
            tokens[input_prep2_idx].vocab->type == VOCAB_SYNONYM) {
          prep2_word = tokens[input_prep2_idx].vocab->target;
        }
        if (strcasecmp(tokens[input_prep2_idx].word, se->prep2) == 0 ||
            strcasecmp(prep2_word, se->prep2) == 0) {
          p2_match = true;
        }
      }
    }

    if (!p1_match || !p2_match)
      continue;

    int nc1_start = -1, nc1_end = -1;
    int nc2_start = -1, nc2_end = -1;
    int ptr = 1;

    if (!se->obj1_present && !se->obj2_present) {
      int expected_tokens = 1;
      if (se->prep1) expected_tokens++;
      if (se->prep2) expected_tokens++;
      if (num_tokens != expected_tokens)
        goto next_syntax;
    }

    if (se->obj1_present) {
      if (se->prep1_loc == PREP_LOC_BEFORE_OBJ1) {
        if (input_prep1_idx != ptr)
          goto next_syntax;
        ptr++;
        nc1_start = ptr;
        if (se->prep2 != NULL) {
          if (input_prep2_idx == -1 || input_prep2_idx <= ptr)
            goto next_syntax;
          nc1_end = input_prep2_idx;
          ptr = input_prep2_idx + 1;
        } else {
          nc1_end = num_tokens;
          ptr = num_tokens;
        }
      } else if (se->prep1_loc == PREP_LOC_AFTER_OBJ1) {
        nc1_start = ptr;
        if (input_prep1_idx == -1 || input_prep1_idx <= ptr)
          goto next_syntax;
        nc1_end = input_prep1_idx;
        ptr = input_prep1_idx + 1;
      } else {
        nc1_start = ptr;
        if (se->prep2 != NULL) {
          if (input_prep1_idx != -1) {
            nc1_end = input_prep1_idx;
            ptr = input_prep1_idx;
          }
        } else {
          nc1_end = num_tokens;
          ptr = num_tokens;
        }
      }

      if (nc1_start >= nc1_end) {
        // The verb fits, but nothing was named to apply it to.
        wanted_object = true;
        goto next_syntax;
      }
    }

    if (se->obj2_present) {
      nc2_start = ptr;
      nc2_end = num_tokens;
      if (nc2_start >= nc2_end)
        goto next_syntax;
    }

    // Snarf Objects
    int count1 = 0;

    parsing_action = se->action_id;
    if (se->obj1_present) {
      count1 = snarf_objects(nc1_start, nc1_end, se->obj1_search, se->obj1_find,
                             cmd->prso_list, MAX_OBJECTS_PER_CMD);
      if (count1 == 0) {
        if (!asked_which)
          tellf("You can't see any %s here!\n", noun_of(nc1_start, nc1_end));
        return false;
      }
      cmd->prso_count = count1;
    }

    if (se->obj2_present) {
      // Prsi
      ZObjectID prsi_list[2];
      int count2 = snarf_objects(nc2_start, nc2_end, se->obj2_search,
                                 se->obj2_find, prsi_list, 2);
      if (count2 == 0) {
        if (!asked_which)
          tellf("You can't see any %s here!\n", noun_of(nc2_start, nc2_end));
        return false;
      }
      cmd->prsi = prsi_list[0];
    }

    cmd->verb = se->action_id;
    return true;

  next_syntax:
    continue;
  }

  if (wanted_object) {
    // ORPHAN (parser.zil): "What do you want to take?" -- and remember the
    // verb, so the reply can be joined to it.
    tellf("What do you want to %s?\n", tokens[0].word);
    snprintf(orphan_verb, sizeof(orphan_verb), "%s", tokens[0].word);
    orphan_pending = true;
    return false;
  }

  // parser.zil distinguishes a sentence it cannot fit to any syntax line from
  // one that never had a verb in it. The second is the more useful complaint:
  // it tells the player what is actually missing.
  for (int i = 0; i < num_tokens; i++) {
    if (is_verb_word(tokens[i].word)) {
      tellf("I don't understand that sentence.\n");
      return false;
    }
  }
  tellf("I can't find a verb in that sentence!\n");
  return false;
}
