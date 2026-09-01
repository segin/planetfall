CC = gcc
# Pin the *language* to ISO C23 (no GNU extensions). GCC only learned the name
# -std=c23 in 14 and clang in 18; before that the same language is spelled
# -std=c2x. Distro packaging still has to build on RHEL 9's GCC 11, so ask the
# compiler which name it answers to rather than assuming.
STD := $(shell $(CC) -std=c23 -E -x c /dev/null >/dev/null 2>&1 && echo c23 || echo c2x)

# _DEFAULT_SOURCE keeps the POSIX/BSD library surface we rely on visible
# (strcasecmp, sigaction, TIOCGWINSZ), which a strict ISO mode would otherwise
# hide.
CFLAGS = -std=$(STD) -D_DEFAULT_SOURCE -Wall -Werror -Iinclude -g

VERSION = 0.1.0

# Install layout. Distro packaging overrides these: Debian puts games in
# /usr/games, the RPM distributions use /usr/bin. DESTDIR is the staging root
# every package builder expects to be honoured.
PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
DATADIR ?= $(PREFIX)/share
MANDIR ?= $(DATADIR)/man
INSTALL ?= install

SRC = src/engine_core.c src/parser.c src/main.c src/events.c src/feinstein.c src/complexone.c src/complextwo.c src/feinstein_actions.c src/complexone_actions.c src/complextwo_actions.c src/global_objects.c src/syntax_gen.c src/actions.c src/output.c src/savegame.c
OBJ = $(SRC:.c=.o)
TEST_ENGINE_SRC = tests/test_engine.c
TEST_PARSER_SRC = tests/test_parser.c
TEST_EVENTS_SRC = tests/test_events.c

TEST_ENGINE_BIN = tests/run_test_engine
TEST_PARSER_BIN = tests/run_test_parser
TEST_EVENTS_BIN = tests/run_test_events

BIN = planetfall

.PHONY: all test check compare install uninstall dist clean

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ)

test: $(TEST_ENGINE_BIN) $(TEST_PARSER_BIN) $(TEST_EVENTS_BIN) $(BIN)
	$(TEST_ENGINE_BIN)
	$(TEST_PARSER_BIN)
	$(TEST_EVENTS_BIN)
	bash tests/test_feinstein.sh

check: test

# Differential comparison against the real game. zil/planetfall.beta is a
# Z-machine v3 build of Planetfall, so with dfrotz (pacman -S frotz-dumb) the
# same input can be driven through both and the replies compared. Reports
# findings rather than gating, because the binary is Release 1 while the ZIL we
# are porting is Release 39. Not part of `make test`.
compare: $(BIN)
	python3 tests/compare_original.py

$(TEST_ENGINE_BIN): src/engine_core.o $(TEST_ENGINE_SRC)
	$(CC) $(CFLAGS) -o $(TEST_ENGINE_BIN) $(TEST_ENGINE_SRC) src/engine_core.o

$(TEST_PARSER_BIN): src/engine_core.o src/parser.o src/syntax_gen.o src/output.o $(TEST_PARSER_SRC)
	$(CC) $(CFLAGS) -o $(TEST_PARSER_BIN) $(TEST_PARSER_SRC) src/engine_core.o src/parser.o src/syntax_gen.o src/output.o

$(TEST_EVENTS_BIN): src/events.o $(TEST_EVENTS_SRC)
	$(CC) $(CFLAGS) -o $(TEST_EVENTS_BIN) $(TEST_EVENTS_SRC) src/events.o

install: $(BIN)
	$(INSTALL) -d $(DESTDIR)$(BINDIR)
	$(INSTALL) -m 755 $(BIN) $(DESTDIR)$(BINDIR)/$(BIN)
	$(INSTALL) -d $(DESTDIR)$(MANDIR)/man6
	$(INSTALL) -m 644 doc/planetfall.6 $(DESTDIR)$(MANDIR)/man6/planetfall.6

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(BIN)
	rm -f $(DESTDIR)$(MANDIR)/man6/planetfall.6

# Source tarball for the packaging to consume. zil/ is deliberately excluded:
# it holds Infocom's original sources and a Z-machine build of the game, which
# zil/README.md states are not under an open licence. They are reference
# material for developing the port, not something to redistribute.
dist: clean
	tar --transform 's,^,planetfall-$(VERSION)/,' \
	    --exclude-vcs --exclude=zil --exclude='*.sav' --exclude='*.o' \
	    -czf planetfall-$(VERSION).tar.gz \
	    src include tools tests doc docs Makefile AGENTS.md

clean:
	rm -f src/*.o $(TEST_ENGINE_BIN) $(TEST_PARSER_BIN) $(TEST_EVENTS_BIN) $(BIN)
	rm -f planetfall-$(VERSION).tar.gz
