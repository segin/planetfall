CC = gcc
CFLAGS = -Wall -Iinclude -g

SRC = src/engine_core.c src/parser.c src/main.c src/events.c src/feinstein.c src/complexone.c
OBJ = $(SRC:.c=.o)
TEST_ENGINE_SRC = tests/test_engine.c
TEST_PARSER_SRC = tests/test_parser.c
TEST_EVENTS_SRC = tests/test_events.c

TEST_ENGINE_BIN = tests/run_test_engine
TEST_PARSER_BIN = tests/run_test_parser
TEST_EVENTS_BIN = tests/run_test_events

BIN = planetfall

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ)

test: $(TEST_ENGINE_BIN) $(TEST_PARSER_BIN) $(TEST_EVENTS_BIN)
	$(TEST_ENGINE_BIN)
	$(TEST_PARSER_BIN)
	$(TEST_EVENTS_BIN)

$(TEST_ENGINE_BIN): src/engine_core.o $(TEST_ENGINE_SRC)
	$(CC) $(CFLAGS) -o $(TEST_ENGINE_BIN) $(TEST_ENGINE_SRC) src/engine_core.o

$(TEST_PARSER_BIN): src/engine_core.o src/parser.o $(TEST_PARSER_SRC)
	$(CC) $(CFLAGS) -o $(TEST_PARSER_BIN) $(TEST_PARSER_SRC) src/engine_core.o src/parser.o

$(TEST_EVENTS_BIN): src/events.o src/feinstein.o src/engine_core.o $(TEST_EVENTS_SRC)
	$(CC) $(CFLAGS) -o $(TEST_EVENTS_BIN) $(TEST_EVENTS_SRC) src/events.o src/feinstein.o src/engine_core.o

clean:
	rm -f src/*.o tests/run_tests $(BIN)
