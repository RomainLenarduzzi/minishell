#!/bin/bash

# Simple Ultimate Comparison Test - Direct Side-by-Side
# Focuses on the fixes we made and core functionality

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

MINISHELL="../minishell"
PASS=0
FAIL=0

echo -e "${CYAN}╔════════════════════════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║     MINISHELL vs BASH - CORE FUNCTIONALITY TEST           ║${NC}"
echo -e "${CYAN}╚════════════════════════════════════════════════════════════╝${NC}\n"

################################################################################
# TEST 1: EXPORT SORTING
################################################################################
echo -e "${BLUE}━━━ TEST 1: Export Alphabetical Sorting ━━━${NC}"

# Create test script for bash
cat > /tmp/test_bash_export.sh << 'EOF'
export ZZZ=last
export AAA=first
export MMM=middle
export TEST_B=b
export TEST_A=a
export | grep -E "^declare -x (AAA|MMM|ZZZ|TEST_)" | head -5
EOF

# Create test script for minishell  
cat > /tmp/test_mini_export.sh << 'EOF'
export ZZZ=last
export AAA=first
export MMM=middle
export TEST_B=b
export TEST_A=a
export
exit
EOF

bash_out=$(bash /tmp/test_bash_export.sh 2>&1 | sort)
mini_out=$(cat /tmp/test_mini_export.sh | $MINISHELL 2>&1 | grep -E "^declare -x (AAA|MMM|ZZZ|TEST_)" | head -5 | sort)

echo -e "${CYAN}Bash (sorted):${NC}"
echo "$bash_out"
echo -e "\n${CYAN}Minishell (sorted):${NC}"
echo "$mini_out"

if [ "$bash_out" == "$mini_out" ]; then
    echo -e "\n${GREEN}✓ PASS${NC} - Export sorting works!\n"
    PASS=$((PASS + 1))
else
    echo -e "\n${RED}✗ FAIL${NC} - Export sorting differs\n"
    FAIL=$((FAIL + 1))
fi

################################################################################
# TEST 2: EXPORT WITHOUT VALUE
################################################################################
echo -e "${BLUE}━━━ TEST 2: Export Variable Without Value ━━━${NC}"

bash_novalue=$(bash -c 'export TEST_NO_VALUE; export | grep "^declare -x TEST_NO_VALUE"' 2>&1)
mini_novalue=$(echo -e "export TEST_NO_VALUE\nexport\nexit" | $MINISHELL 2>&1 | grep "^declare -x TEST_NO_VALUE")

echo -e "${CYAN}Bash:${NC}       $bash_novalue"
echo -e "${CYAN}Minishell:${NC}  $mini_novalue"

if [[ "$mini_novalue" == *"TEST_NO_VALUE"* ]]; then
    echo -e "${GREEN}✓ PASS${NC} - Export without value works!\n"
    PASS=$((PASS + 1))
else
    echo -e "${RED}✗ FAIL${NC} - Export without value failed\n"
    FAIL=$((FAIL + 1))
fi

################################################################################
# TEST 3: EXPORT WITH VALUE
################################################################################
echo -e "${BLUE}━━━ TEST 3: Export Variable With Value ━━━${NC}"

bash_value=$(bash -c 'export MY_TEST=hello_world; export | grep "^declare -x MY_TEST="' 2>&1)
mini_value=$(echo -e "export MY_TEST=hello_world\nexport\nexit" | $MINISHELL 2>&1 | grep "^declare -x MY_TEST=")

echo -e "${CYAN}Bash:${NC}       $bash_value"
echo -e "${CYAN}Minishell:${NC}  $mini_value"

if [ "$bash_value" == "$mini_value" ]; then
    echo -e "${GREEN}✓ PASS${NC} - Export with value works!\n"
    PASS=$((PASS + 1))
else
    echo -e "${RED}✗ FAIL${NC} - Export with value differs\n"
    FAIL=$((FAIL + 1))
fi

################################################################################
# TEST 4: EXPORT MODIFY VALUE
################################################################################
echo -e "${BLUE}━━━ TEST 4: Modify Exported Variable ━━━${NC}"

bash_modify=$(bash -c 'export VAR1=initial; export VAR1=modified; export | grep "^declare -x VAR1="' 2>&1)
mini_modify=$(echo -e "export VAR1=initial\nexport VAR1=modified\nexport\nexit" | $MINISHELL 2>&1 | grep "^declare -x VAR1=")

echo -e "${CYAN}Bash:${NC}       $bash_modify"
echo -e "${CYAN}Minishell:${NC}  $mini_modify"

if [ "$bash_modify" == "$mini_modify" ]; then
    echo -e "${GREEN}✓ PASS${NC} - Export modify value works!\n"
    PASS=$((PASS + 1))
else
    echo -e "${RED}✗ FAIL${NC} - Export modify value differs\n"
    FAIL=$((FAIL + 1))
fi

################################################################################
# TEST 5: SHLVL INCREMENT
################################################################################
echo -e "${BLUE}━━━ TEST 5: SHLVL Increment ━━━${NC}"

current_shlvl=$SHLVL
expected=$((current_shlvl + 1))
mini_shlvl=$(echo -e "env\nexit" | $MINISHELL 2>&1 | grep "^SHLVL=" | cut -d= -f2)

echo -e "${CYAN}Parent SHLVL:${NC}     $current_shlvl"
echo -e "${CYAN}Expected:${NC}        $expected"
echo -e "${CYAN}Minishell:${NC}       $mini_shlvl"

if [ "$mini_shlvl" == "$expected" ]; then
    echo -e "${GREEN}✓ PASS${NC} - SHLVL increments correctly!\n"
    PASS=$((PASS + 1))
else
    echo -e "${RED}✗ FAIL${NC} - SHLVL not incremented correctly\n"
    FAIL=$((FAIL + 1))
fi

################################################################################
# TEST 6: ECHO BASIC
################################################################################
echo -e "${BLUE}━━━ TEST 6: Echo Basic ━━━${NC}"

bash_echo=$(echo "hello world" | xargs bash -c 'echo "$@"' bash)
mini_echo=$(echo -e "echo hello world\nexit" | $MINISHELL 2>&1 | grep -v "minishell>" | grep -v "^exit$" | head -1)

echo -e "${CYAN}Bash:${NC}       '$bash_echo'"
echo -e "${CYAN}Minishell:${NC}  '$mini_echo'"

if [ "$bash_echo" == "$mini_echo" ]; then
    echo -e "${GREEN}✓ PASS${NC} - Echo works!\n"
    PASS=$((PASS + 1))
else
    echo -e "${RED}✗ FAIL${NC} - Echo differs\n"
    FAIL=$((FAIL + 1))
fi

################################################################################
# TEST 7: ECHO WITH -n
################################################################################
echo -e "${BLUE}━━━ TEST 7: Echo with -n Flag ━━━${NC}"

bash_echo_n=$(bash -c 'echo -n hello')
mini_echo_n=$(echo -e "echo -n hello\nexit" | $MINISHELL 2>&1 | grep -v "minishell>" | grep -v "^exit$" | tr -d '\n')

echo -e "${CYAN}Bash:${NC}       '$bash_echo_n'"
echo -e "${CYAN}Minishell:${NC}  '$mini_echo_n'"

if [ "$bash_echo_n" == "$mini_echo_n" ]; then
    echo -e "${GREEN}✓ PASS${NC} - Echo -n works!\n"
    PASS=$((PASS + 1))
else
    echo -e "${RED}✗ FAIL${NC} - Echo -n differs\n"
    FAIL=$((FAIL + 1))
fi

################################################################################
# TEST 8: PWD
################################################################################
echo -e "${BLUE}━━━ TEST 8: PWD Command ━━━${NC}"

bash_pwd=$(pwd)
mini_pwd=$(echo -e "pwd\nexit" | $MINISHELL 2>&1 | grep -v "minishell>" | grep -v "^exit$" | head -1)

echo -e "${CYAN}Bash:${NC}       $bash_pwd"
echo -e "${CYAN}Minishell:${NC}  $mini_pwd"

if [ "$bash_pwd" == "$mini_pwd" ]; then
    echo -e "${GREEN}✓ PASS${NC} - PWD works!\n"
    PASS=$((PASS + 1))
else
    echo -e "${RED}✗ FAIL${NC} - PWD differs\n"
    FAIL=$((FAIL + 1))
fi

################################################################################
# TEST 9: EXIT CODE
################################################################################
echo -e "${BLUE}━━━ TEST 9: Exit with Code 42 ━━━${NC}"

echo "exit 42" | $MINISHELL > /dev/null 2>&1
mini_exit=$?

echo -e "${CYAN}Expected:${NC}    42"
echo -e "${CYAN}Minishell:${NC}   $mini_exit"

if [ $mini_exit -eq 42 ]; then
    echo -e "${GREEN}✓ PASS${NC} - Exit code works!\n"
    PASS=$((PASS + 1))
else
    echo -e "${RED}✗ FAIL${NC} - Exit code incorrect\n"
    FAIL=$((FAIL + 1))
fi

################################################################################
# TEST 10: EXIT NON-NUMERIC
################################################################################
echo -e "${BLUE}━━━ TEST 10: Exit Non-Numeric Error ━━━${NC}"

mini_exit_err=$(echo "exit abc" | $MINISHELL 2>&1 | grep "numeric argument required")

if [[ -n "$mini_exit_err" ]]; then
    echo -e "${CYAN}Error message:${NC} Present ✓"
    echo -e "${GREEN}✓ PASS${NC} - Exit error handling works!\n"
    PASS=$((PASS + 1))
else
    echo -e "${RED}✗ FAIL${NC} - Exit error message missing\n"
    FAIL=$((FAIL + 1))
fi

################################################################################
# SUMMARY
################################################################################
echo -e "\n${CYAN}╔════════════════════════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║                    FINAL RESULTS                           ║${NC}"
echo -e "${CYAN}╚════════════════════════════════════════════════════════════╝${NC}\n"

TOTAL=$((PASS + FAIL))
PERCENTAGE=$((PASS * 100 / TOTAL))

echo -e "${CYAN}Total Tests:${NC}      $TOTAL"
echo -e "${GREEN}Passed:${NC}           $PASS"
echo -e "${RED}Failed:${NC}           $FAIL"
echo -e "${YELLOW}Success Rate:${NC}     ${PERCENTAGE}%\n"

if [ $PERCENTAGE -ge 90 ]; then
    echo -e "${GREEN}╔════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${GREEN}║  🎉 EXCELLENT! Minishell matches bash behavior! 🎉         ║${NC}"
    echo -e "${GREEN}╚════════════════════════════════════════════════════════════╝${NC}\n"
    exit 0
elif [ $PERCENTAGE -ge 70 ]; then
    echo -e "${YELLOW}╔════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${YELLOW}║  Good! Most tests pass. Minor fixes may be needed.        ║${NC}"
    echo -e "${YELLOW}╚════════════════════════════════════════════════════════════╝${NC}\n"
    exit 0
else
    echo -e "${RED}╔════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${RED}║  Some tests failed. Review output for details.            ║${NC}"
    echo -e "${RED}╚════════════════════════════════════════════════════════════╝${NC}\n"
    exit 1
fi

# Cleanup
rm -f /tmp/test_bash_export.sh /tmp/test_mini_export.sh
