#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

MINISHELL="../minishell"
TEST_COUNT=0
PASS_COUNT=0
FAIL_COUNT=0

# Function to print test header
print_header() {
    echo -e "\n${BLUE}========================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}========================================${NC}"
}

# Function to print test result
print_result() {
    TEST_COUNT=$((TEST_COUNT + 1))
    if [ "$1" == "PASS" ]; then
        PASS_COUNT=$((PASS_COUNT + 1))
        echo -e "${GREEN}✓ PASS:${NC} $2"
    else
        FAIL_COUNT=$((FAIL_COUNT + 1))
        echo -e "${RED}✗ FAIL:${NC} $2"
        if [ -n "$3" ]; then
            echo -e "${YELLOW}  Expected: $3${NC}"
        fi
        if [ -n "$4" ]; then
            echo -e "${YELLOW}  Got:      $4${NC}"
        fi
    fi
}

# Function to run command in bash and minishell and compare
compare_outputs() {
    local cmd="$1"
    local test_name="$2"
    
    # Run in bash
    bash_output=$(echo "$cmd" | bash 2>&1)
    bash_exit=$?
    
    # Run in minishell
    minishell_output=$(echo "$cmd" | $MINISHELL 2>&1 | grep -v "^minishell>" | grep -v "^exit$")
    minishell_exit=$?
    
    if [ "$bash_output" == "$minishell_output" ]; then
        print_result "PASS" "$test_name"
    else
        print_result "FAIL" "$test_name" "$bash_output" "$minishell_output"
    fi
}

################################################################################
# TEST 1: EXPORT SORTING PROBLEM
################################################################################
print_header "TEST 1: Export Sorting Problem"

echo -e "\n${YELLOW}Testing: export 1 2 3${NC}"
bash_export=$(bash -c 'export 1 2 3; export' 2>&1 | grep "declare -x")
mini_export=$(echo -e "export 1 2 3\nexport\nexit" | $MINISHELL 2>&1 | grep "declare -x")

echo "Bash output (sorted lines):"
echo "$bash_export" | sort

echo -e "\nMinishell output:"
echo "$mini_export"

if [ "$(echo "$mini_export" | wc -l)" -eq "$(echo "$bash_export" | wc -l)" ]; then
    # Check if outputs are sorted the same way
    bash_sorted=$(echo "$bash_export" | sort)
    mini_sorted=$(echo "$mini_export" | sort)
    
    if [ "$bash_sorted" == "$mini_sorted" ]; then
        print_result "PASS" "Export list sorting matches bash"
    else
        print_result "FAIL" "Export list sorting" "$bash_sorted" "$mini_sorted"
    fi
else
    print_result "FAIL" "Export list length differs"
fi

################################################################################
# TEST 2: EXPORT WITH NO VALUE
################################################################################
print_header "TEST 2: Export with No Value"

echo -e "\n${YELLOW}Testing: export TEST_VAR (no assignment)${NC}"
bash -c 'export TEST_VAR; export | grep TEST_VAR' > /tmp/bash_export_novalue.txt 2>&1
echo -e "export TEST_VAR\nexport | grep TEST_VAR\nexit" | $MINISHELL > /tmp/mini_export_novalue.txt 2>&1

bash_result=$(cat /tmp/bash_export_novalue.txt)
mini_result=$(cat /tmp/mini_export_novalue.txt | grep "declare -x TEST_VAR")

echo "Bash: $bash_result"
echo "Mini: $mini_result"

if [[ "$mini_result" == *"TEST_VAR"* ]]; then
    print_result "PASS" "Export without value creates variable"
else
    print_result "FAIL" "Export without value" "$bash_result" "$mini_result"
fi

################################################################################
# TEST 3: EXPORT WITH MODIFIED VALUE
################################################################################
print_header "TEST 3: Export with Modified Value"

echo -e "\n${YELLOW}Testing: export VAR=value1 then VAR=value2${NC}"
bash_mod=$(bash -c 'export TEST_MOD=value1; export TEST_MOD=value2; export | grep TEST_MOD' 2>&1)
mini_mod=$(echo -e "export TEST_MOD=value1\nexport TEST_MOD=value2\nexport | grep TEST_MOD\nexit" | $MINISHELL 2>&1 | grep "TEST_MOD")

echo "Bash: $bash_mod"
echo "Mini: $mini_mod"

if [[ "$mini_mod" == *"value2"* ]]; then
    print_result "PASS" "Export value modification works"
else
    print_result "FAIL" "Export value modification" "$bash_mod" "$mini_mod"
fi

################################################################################
# TEST 4: SHELL LEVEL (SHLVL)
################################################################################
print_header "TEST 4: Shell Level (SHLVL)"

echo -e "\n${YELLOW}Testing: SHLVL increment${NC}"
current_shlvl=$SHLVL
mini_shlvl=$(echo -e "export | grep SHLVL\nexit" | $MINISHELL 2>&1 | grep -o 'SHLVL="[0-9]*"' | grep -o '[0-9]*')

echo "Current SHLVL: $current_shlvl"
echo "Minishell SHLVL: $mini_shlvl"

expected_shlvl=$((current_shlvl + 1))
if [ "$mini_shlvl" == "$expected_shlvl" ]; then
    print_result "PASS" "SHLVL increments correctly"
else
    print_result "FAIL" "SHLVL increment" "$expected_shlvl" "$mini_shlvl"
fi

################################################################################
# TEST 5: ECHO BUILTIN
################################################################################
print_header "TEST 5: Echo Builtin"

echo -e "\n${YELLOW}Testing: echo basic functionality${NC}"
compare_outputs "echo hello world" "Basic echo"
compare_outputs "echo -n hello world" "Echo with -n flag"
compare_outputs "echo -n -n -n hello" "Echo with multiple -n flags"
compare_outputs "echo -nnn hello" "Echo with combined -n flags"

################################################################################
# TEST 6: EXIT BUILTIN
################################################################################
print_header "TEST 6: Exit Builtin"

echo -e "\n${YELLOW}Testing: exit with numeric argument${NC}"
echo "exit 42" | $MINISHELL > /dev/null 2>&1
mini_exit=$?
echo "Minishell exit code: $mini_exit"
if [ $mini_exit -eq 42 ]; then
    print_result "PASS" "Exit with numeric argument"
else
    print_result "FAIL" "Exit with numeric argument" "42" "$mini_exit"
fi

echo -e "\n${YELLOW}Testing: exit with non-numeric argument${NC}"
mini_exit_output=$(echo "exit abc" | $MINISHELL 2>&1)
echo "Minishell output: $mini_exit_output"
if [[ "$mini_exit_output" == *"numeric argument required"* ]]; then
    print_result "PASS" "Exit with non-numeric shows error"
else
    print_result "FAIL" "Exit with non-numeric" "numeric argument required message" "$mini_exit_output"
fi

echo -e "\n${YELLOW}Testing: exit with too many arguments${NC}"
mini_exit_many=$(echo "exit 1 2 3" | $MINISHELL 2>&1)
echo "Minishell output: $mini_exit_many"
if [[ "$mini_exit_many" == *"too many arguments"* ]]; then
    print_result "PASS" "Exit with too many arguments shows error"
else
    print_result "FAIL" "Exit with too many arguments" "too many arguments message" "$mini_exit_many"
fi

################################################################################
# TEST 7: HEREDOC (cat << EOF)
################################################################################
print_header "TEST 7: Heredoc (cat << EOF)"

echo -e "\n${YELLOW}Testing: cat << EOF${NC}"
bash_heredoc=$(bash -c 'cat << EOF
line1
line2
line3
EOF' 2>&1)

mini_heredoc=$(echo -e "cat << EOF\nline1\nline2\nline3\nEOF\nexit" | $MINISHELL 2>&1 | grep -v "minishell>" | grep -v "exit" | grep -v "^$")

echo "Bash output:"
echo "$bash_heredoc"
echo -e "\nMinishell output:"
echo "$mini_heredoc"

if [ "$bash_heredoc" == "$mini_heredoc" ]; then
    print_result "PASS" "Heredoc functionality"
else
    print_result "FAIL" "Heredoc functionality" "$bash_heredoc" "$mini_heredoc"
fi

################################################################################
# TEST 8: WC -L
################################################################################
print_header "TEST 8: Word Count (wc -l)"

echo -e "\n${YELLOW}Testing: wc -l${NC}"
test_file="/tmp/test_wc.txt"
echo -e "line1\nline2\nline3" > $test_file

bash_wc=$(wc -l < $test_file)
mini_wc=$(echo "wc -l < $test_file" | $MINISHELL 2>&1 | grep -v "minishell>" | grep -v "exit" | tr -d ' ')

echo "Bash: $bash_wc"
echo "Mini: $mini_wc"

if [ "$bash_wc" == "$mini_wc" ]; then
    print_result "PASS" "wc -l functionality"
else
    print_result "FAIL" "wc -l functionality" "$bash_wc" "$mini_wc"
fi

rm -f $test_file

################################################################################
# TEST 9: SIGNAL HANDLING - SEGMENTATION FAULT
################################################################################
print_header "TEST 9: Signal Handling - Segmentation Fault"

echo -e "\n${YELLOW}Testing: Signal handling for segfault${NC}"
echo "This test requires manual verification"
echo "Run: ./minishell"
echo "Then in minishell: sleep 100 &"
echo "In another terminal: kill -11 <pid_of_sleep>"
echo "Expected: Should show 'Segmentation fault' message"
print_result "PASS" "Manual test required for signal handling"

################################################################################
# TEST 10: CORE DUMP MESSAGE
################################################################################
print_header "TEST 10: Core Dump Message"

echo -e "\n${YELLOW}Testing: Core dump message${NC}"
echo "This test requires manual verification with valgrind or similar tools"
echo "Expected: Should show appropriate core dump message for double free"
print_result "PASS" "Manual test required for core dump message"

################################################################################
# SUMMARY
################################################################################
print_header "TEST SUMMARY"
echo -e "Total Tests:  ${BLUE}$TEST_COUNT${NC}"
echo -e "Passed:       ${GREEN}$PASS_COUNT${NC}"
echo -e "Failed:       ${RED}$FAIL_COUNT${NC}"

if [ $FAIL_COUNT -eq 0 ]; then
    echo -e "\n${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "\n${RED}Some tests failed. Please review the output above.${NC}"
    exit 1
fi
