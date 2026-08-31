#include <stdio.h>
#include <assert.h>
#include "planetfall.h"

void test_object_linking() {
    printf("Testing Object Linking...\n");
    
    // Reset objects
    init_game();
    
    ZObjectID parent = 10;
    ZObjectID child1 = 11;
    ZObjectID child2 = 12;
    
    objects[parent].id = parent;
    objects[child1].id = child1;
    objects[child2].id = child2;
    
    // Move child1 to parent
    obj_move(child1, parent);
    assert(objects[child1].parent == parent);
    assert(objects[parent].child == child1);
    assert(objects[child1].sibling == NOTHING);
    
    // Move child2 to parent
    obj_move(child2, parent);
    assert(objects[child2].parent == parent);
    assert(objects[parent].child == child2);     // Should be first
    assert(objects[child2].sibling == child1);   // child2 -> child1
    assert(objects[child1].sibling == NOTHING);
    
    // Remove child2
    obj_remove(child2);
    assert(objects[child2].parent == NOTHING);
    assert(objects[parent].child == child1);
    
    printf("Object Linking Passed.\n");
}

void test_flags() {
    printf("Testing Flags...\n");
    ZObjectID obj = 20;
    objects[obj].id = obj;
    
    assert(!obj_has_flag(obj, F_TAKEBIT));
    obj_set_flag(obj, F_TAKEBIT);
    assert(obj_has_flag(obj, F_TAKEBIT));
    obj_set_flag(obj, F_OPENBIT);
    assert(obj_has_flag(obj, F_TAKEBIT));
    assert(obj_has_flag(obj, F_OPENBIT));
    obj_clear_flag(obj, F_TAKEBIT);
    assert(!obj_has_flag(obj, F_TAKEBIT));
    assert(obj_has_flag(obj, F_OPENBIT));
    
    printf("Flags Passed.\n");
}

void test_score_obj() {
    printf("Testing Score Obj...\n");
    init_game();
    assert(game_state.score == 0);
    assert(game_state.day == 1);
    assert(game_state.internal_moves == 0);

    ZObjectID item = 50;
    objects[item].id = item;
    objects[item].value = 5;
    objects[item].flags = 0;

    score_obj(item);
    assert(game_state.score == 5);
    assert(objects[item].value == 0);
    assert(obj_has_flag(item, F_TOUCHBIT));

    // Scoring it again shouldn't increase score
    score_obj(item);
    assert(game_state.score == 5);

    printf("Score Obj Passed.\n");
}

void init_objects() {
    // Dummy init for test
}

int main() {
    test_object_linking();
    test_flags();
    test_score_obj();
    return 0;
}
