#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "planetfall.h"
#include "parser.h"

// Mock objects for testing
void setup_parser_test() {
    init_game();
    player = 10;
    current_room = 20;
    
    objects[player].id = 10;
    objects[player].child = 30; // Has item 30
    
    objects[20].id = 20; // Room
    objects[20].child = 40; // Has item 40
    
    objects[30].id = 30;
    objects[30].synonyms[0] = "item";
    objects[30].flags = 0;
    objects[30].sibling = NOTHING;
    objects[30].parent = player;
    
    objects[40].id = 40;
    objects[40].synonyms[0] = "flooritem";
    objects[40].flags = 0;
    objects[40].sibling = NOTHING;
    objects[40].parent = 20;
}

void test_tokenize() {
    printf("Testing Tokenizer...\n");
    Command cmd;
    // We can't access static 'tokens' directly if not exposed, 
    // but we can test via parse_command result.
    // Assuming parse_command calls tokenize.
    
    char input[] = "Look at item";
    bool res = parse_command(input, &cmd);
    // Since we haven't implemented "at" skipping fully or "look at" mapping, 
    // this depends on current parser.c implementation.
    // Current parser: verbs={"look": V_LOOK}, prepositions={...}
    
    // Test basic verb
    char input1[] = "look";
    res = parse_command(input1, &cmd);
    assert(res);
    assert(cmd.verb == V_LOOK);
    
    // Test verb + object
    // "take item"
    char input2[] = "take item";
    res = parse_command(input2, &cmd);
    assert(res);
    assert(cmd.verb == V_TAKE);
    assert(cmd.direct_object == 30);
    
    printf("Tokenizer/Parser Basic Passed.\n");
}

int main() {
    setup_parser_test();
    test_tokenize();
    return 0;
}
