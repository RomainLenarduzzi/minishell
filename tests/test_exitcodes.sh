#!/bin/bash

# Test exit codes for cd and exit with too many arguments

echo "Testing cd with too many arguments..."
echo "cd /tmp /usr" | ../minishell > /dev/null 2>&1
mini_cd=$?
bash -c "cd /tmp /usr" > /dev/null 2>&1
bash_cd=$?
echo "Minishell: $mini_cd | Bash: $bash_cd"
if [ $mini_cd -eq $bash_cd ]; then
    echo "✓ PASS - cd exit codes match"
else
    echo "✗ FAIL - cd exit codes differ"
fi

echo ""
echo "Testing exit with too many arguments..."
echo -e "exit 42 world\necho \$?\nexit" | ../minishell > /dev/null 2>&1
mini_exit=$?
echo "Minishell exit status: $mini_exit (should NOT exit shell, just return error code 2)"

echo ""
echo "Testing cd \$PWD hi (from test 40)..."
echo "cd \$PWD hi" | ../minishell > /dev/null 2>&1
mini_pwd=$?
bash -c "cd \$PWD hi" > /dev/null 2>&1
bash_pwd=$?
echo "Minishell: $mini_pwd | Bash: $bash_pwd"
if [ $mini_pwd -eq $bash_pwd ]; then
    echo "✓ PASS - cd \$PWD hi exit codes match"
else
    echo "✗ FAIL - cd \$PWD hi exit codes differ"
fi
