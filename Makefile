CC = gcc
CFLAGS = -Wall -Iinclude -g

SRC = src/engine_core.c src/parser.c src/main.c src/events.c src/feinstein.c src/complexone.c src/complextwo.c src/feinstein_actions.c src/complexone_actions.c src/complextwo_actions.c src/global_objects.c src/syntax_gen.c src/actions.c src/output.c src/savegame.c
OBJ = $(SRC:.c=.o)
TEST_ENGINE_SRC = tests/test_engine.c
TEST_PARSER_SRC = tests/test_parser.c
TEST_EVENTS_SRC = tests/test_events.c

TEST_ENGINE_BIN = tests/run_test_engine
TEST_PARSER_BIN = tests/run_test_parser
TEST_EVENTS_BIN = tests/run_test_events

BIN = planetfall

.PHONY: all test check clean

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ)

test: $(TEST_ENGINE_BIN) $(TEST_PARSER_BIN) $(TEST_EVENTS_BIN) $(BIN)
	$(TEST_ENGINE_BIN)
	$(TEST_PARSER_BIN)
	$(TEST_EVENTS_BIN)
	bash tests/test_feinstein.sh

check: test

$(TEST_ENGINE_BIN): src/engine_core.o $(TEST_ENGINE_SRC)
	$(CC) $(CFLAGS) -o $(TEST_ENGINE_BIN) $(TEST_ENGINE_SRC) src/engine_core.o

$(TEST_PARSER_BIN): src/engine_core.o src/parser.o src/syntax_gen.o $(TEST_PARSER_SRC)
	$(CC) $(CFLAGS) -o $(TEST_PARSER_BIN) $(TEST_PARSER_SRC) src/engine_core.o src/parser.o src/syntax_gen.o

$(TEST_EVENTS_BIN): src/events.o $(TEST_EVENTS_SRC)
	$(CC) $(CFLAGS) -o $(TEST_EVENTS_BIN) $(TEST_EVENTS_SRC) src/events.o

clean:
	rm -f src/*.o $(TEST_ENGINE_BIN) $(TEST_PARSER_BIN) $(TEST_EVENTS_BIN) $(BIN)
