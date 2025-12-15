#!/bin/bash
# Feinstein Chapter Unit Tests

echo "=== Feinstein Chapter Verification ==="

cd /home/segin/planetfall

# Helper to strip ANSI codes
strip_ansi() {
    sed 's/\x1b\[[0-9;]*m//g' | sed 's/\x1b\[[0-9]*[A-Za-z]//g' | sed 's/\x1b\[[^m]*m//g' | tr -d '[:cntrl:]'
}

# Test 1: Basic room navigation
echo "Test 1: Room navigation (UP from Deck Nine)"
OUTPUT=$(echo -e "UP\nQUIT\nY" | ./planetfall 2>/dev/null | strip_ansi)
echo "$OUTPUT" | grep -qi "Gangway" && echo "PASS: UP leads to Gangway" || echo "FAIL: UP navigation"

# Test 2: Port/Starboard directions  
echo "Test 2: Port direction (should go west to pod if open)"
OUTPUT=$(echo -e "PORT\nQUIT\nY" | ./planetfall 2>/dev/null | strip_ansi)
echo "$OUTPUT" | grep -qiE "pod|closed|can't" && echo "PASS: Port direction works" || echo "FAIL: Port direction"

# Test 3: Examine ME
echo "Test 3: Examine ME"
OUTPUT=$(echo -e "EXAMINE ME\nQUIT\nY" | ./planetfall 2>/dev/null | strip_ansi)
echo "$OUTPUT" | grep -qiE "cretin|nothing special|special" && echo "PASS: Examine ME works" || echo "FAIL: Examine ME"

# Test 4: Look command
echo "Test 4: Look command"
OUTPUT=$(echo -e "LOOK\nQUIT\nY" | ./planetfall 2>/dev/null | strip_ansi)
echo "$OUTPUT" | grep -qi "Deck Nine" && echo "PASS: Look works" || echo "FAIL: Look"

# Test 5: Inventory
echo "Test 5: Inventory"
OUTPUT=$(echo -e "INVENTORY\nQUIT\nY" | ./planetfall 2>/dev/null | strip_ansi)
echo "$OUTPUT" | grep -qiE "carrying|uniform|chronometer|brush" && echo "PASS: Inventory works" || echo "FAIL: Inventory"

# Test 6: Wait for Ambassador (several turns)
echo "Test 6: Wait for events"
OUTPUT=$(echo -e "WAIT\nWAIT\nWAIT\nWAIT\nWAIT\nQUIT\nY" | ./planetfall 2>/dev/null | strip_ansi)
echo "$OUTPUT" | grep -qiE "ambassador|Blather|explosion" && echo "PASS: Events trigger" || echo "FAIL: Events"

# Test 7: Scrub brush in inventory
echo "Test 7: Scrub brush exists"
OUTPUT=$(echo -e "I\nQUIT\nY" | ./planetfall 2>/dev/null | strip_ansi)
echo "$OUTPUT" | grep -qi "brush" && echo "PASS: Scrub brush in inventory" || echo "FAIL: Scrub brush missing"

# Test 8: Chronometer examine
echo "Test 8: Examine chronometer"
OUTPUT=$(echo -e "EXAMINE CHRONOMETER\nQUIT\nY" | ./planetfall 2>/dev/null | strip_ansi)
echo "$OUTPUT" | grep -qiE "wrist|digital|time|chronometer" && echo "PASS: Chronometer examine works" || echo "FAIL: Chronometer"

echo "=== Tests Complete ==="
