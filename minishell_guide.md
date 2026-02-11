# Complete Guide to Minishell - 42 Project

## Table of Contents
1. [What is Minishell?](#what-is-minishell)
2. [Core Concepts](#core-concepts)
3. [Architecture Overview](#architecture-overview)
4. [Mandatory Features](#mandatory-features)
5. [Implementation Deep Dive](#implementation-deep-dive)
6. [Common Pitfalls & Solutions](#common-pitfalls--solutions)
7. [Testing Strategy](#testing-strategy)
8. [Evaluation Tips](#evaluation-tips)

---

## What is Minishell?

Minishell is a simplified version of bash that teaches you how shells work internally. When you type commands in a terminal, the shell:
1. Reads your input
2. Parses it into tokens
3. Expands variables and wildcards
4. Executes commands
5. Handles input/output redirection
6. Returns exit codes

---

## Core Concepts

### 1. The Shell Loop

Every shell follows this basic loop:

```
while (running):
    1. Display prompt
    2. Read input from user
    3. Parse input into tokens
    4. Execute commands
    5. Wait for completion
    6. Repeat
```

### 2. Tokenization

Breaking input into meaningful pieces:

```bash
echo "hello world" > file.txt
```

Becomes tokens:
- `echo` (WORD)
- `"hello world"` (WORD with quotes)
- `>` (REDIRECT_OUT)
- `file.txt` (WORD)

### 3. Quote Handling

**Single Quotes (`'`)**: Everything is literal
```bash
echo '$USER'  # Prints: $USER
```

**Double Quotes (`"`)**: Variables expand
```bash
echo "$USER"  # Prints: your username
```

### 4. Environment Variables

Shells maintain environment (name=value pairs):
```bash
export MY_VAR="hello"
echo $MY_VAR  # Prints: hello
```

Special variables:
- `$?` - Exit code of last command
- `$HOME` - Home directory path
- `$PATH` - Command search paths
- `$PWD` - Current directory

### 5. Pipes

Connect output of one command to input of another:
```bash
cat file.txt | grep "pattern" | wc -l
```

Creates 3 processes connected by 2 pipes.

### 6. Redirections

**Input (`<`)**: Read from file instead of keyboard
```bash
cat < input.txt
```

**Output (`>`)**: Write to file instead of screen
```bash
echo "hello" > output.txt
```

**Append (`>>`)**: Add to end of file
```bash
echo "more" >> output.txt
```

**Here-document (`<<`)**: Multi-line input
```bash
cat << EOF
line 1
line 2
EOF
```

---

## Architecture Overview

### Directory Structure
```
minishell/
├── includes/
│   └── minishell.h          # All structures and function declarations
├── srcs/
│   ├── main.c               # Entry point, main loop
│   ├── parsing/             # Tokenization and syntax checking
│   │   ├── tokenizer.c
│   │   ├── parser.c
│   │   └── syntax_check.c
│   ├── execution/           # Command execution, pipes
│   │   ├── execute.c
│   │   ├── path.c
│   │   └── heredoc.c
│   ├── builtins/            # Built-in commands
│   │   ├── echo.c
│   │   ├── cd.c
│   │   ├── pwd.c
│   │   ├── export.c
│   │   ├── unset.c
│   │   ├── env.c
│   │   └── exit.c
│   ├── environment/         # Environment variable management
│   │   ├── env_init.c
│   │   ├── env_utils.c
│   │   └── env_expansion.c
│   ├── redirections/        # I/O redirection handling
│   │   └── redirections.c
│   └── signals/             # Signal handling
│       └── signals.c
└── Makefile
```

### Data Structures

**Token Structure:**
```c
typedef struct s_token {
    char            *value;      // Token text
    int             type;        // TOKEN_WORD, TOKEN_PIPE, etc.
    struct s_token  *next;       // Linked list
} t_token;
```

**Command Structure:**
```c
typedef struct s_cmd {
    char            **args;      // Command arguments
    t_redir         *redirs;     // Redirections list
    struct s_cmd    *next;       // Next command in pipeline
} t_cmd;
```

**Environment Structure:**
```c
typedef struct s_env {
    char            *key;        // Variable name
    char            *value;      // Variable value
    struct s_env    *next;       // Linked list
} t_env;
```

---

## Mandatory Features

### Builtins (Must Implement)

#### 1. echo
```bash
echo hello world              # Print arguments
echo -n hello                 # No newline at end
echo "hello $USER"            # Variable expansion
```

#### 2. cd
```bash
cd /tmp                       # Change to absolute path
cd ..                         # Relative path
cd                            # Go to HOME
```

**Exit codes:**
- 0: Success
- 1: Directory not found, HOME not set
- 2: Too many arguments

#### 3. pwd
```bash
pwd                           # Print working directory
```

#### 4. export
```bash
export VAR=value              # Create/update variable
export VAR                    # Create without value
export                        # Display all in alphabetical order
```

**Key requirement:** Must display in sorted order!

#### 5. unset
```bash
unset VAR1 VAR2               # Remove variables
```

#### 6. env
```bash
env                           # Display all environment variables
```

#### 7. exit
```bash
exit                          # Exit with last exit code
exit 42                       # Exit with specific code
exit hello                    # Error: numeric argument required
exit 1 2                      # Error: too many arguments
```

**Exit codes:**
- Invalid arg: 2
- Too many args: 2 (but don't exit shell)

### Pipes
```bash
cmd1 | cmd2 | cmd3
```

Each command runs in separate process, connected by pipes.

### Redirections

All 4 types must work:
- `<` input
- `>` output (truncate)
- `>>` append
- `<<` heredoc

### Environment Variables

```bash
$VAR                          # Expand variable
$?                            # Last exit code
```

### Signal Handling

- **Ctrl-C**: Interrupt (new prompt)
- **Ctrl-D**: EOF (exit shell)
- **Ctrl-\\**: Quit (ignore)

---

## Implementation Deep Dive

### 1. Parsing Pipeline

```
User Input → Tokenize → Syntax Check → Build AST → Execute
```

**Example:**
```bash
echo hello | cat > file.txt
```

**Tokenization:**
1. `echo` (WORD)
2. `hello` (WORD)
3. `|` (PIPE)
4. `cat` (WORD)
5. `>` (REDIRECT_OUT)
6. `file.txt` (WORD)

**Syntax validation:**
- Pipe not at start
- Redirect followed by filename
- No unclosed quotes

**Build commands:**
```
Cmd1: echo hello
Cmd2: cat > file.txt
```

### 2. Builtins vs External Commands

**Check if builtin:**
```c
if (cmd is cd, echo, pwd, export, unset, env, exit)
    execute_builtin(cmd);
else
    fork_and_exec(cmd);
```

**Why builtins can't be external:**
- `cd` must change shell's directory
- `export` must modify shell's environment
- `exit` must exit the shell itself

### 3. Pipe Implementation

**For:** `cmd1 | cmd2 | cmd3`

```c
1. Create pipe between cmd1 and cmd2
2. Fork for cmd1
   - Redirect stdout to pipe write end
   - Close unused pipe ends
   - Execute cmd1
3. Create pipe between cmd2 and cmd3
4. Fork for cmd2
   - Redirect stdin from first pipe read end
   - Redirect stdout to second pipe write end
   - Execute cmd2
5. Fork for cmd3
   - Redirect stdin from second pipe read end
   - Execute cmd3
6. Close all pipe ends in parent
7. Wait for all children
```

### 4. Environment Variable Expansion

```bash
echo "$USER $HOME"
```

**Process:**
1. Find `$` in double quotes
2. Extract variable name
3. Look up in environment
4. Replace with value
5. Continue scanning

**Edge cases:**
- `$?` expands to exit code
- Single quotes: no expansion
- `$$` not required (bonus)

### 5. SHLVL Management

Every shell must increment SHLVL:

```c
void increment_shlvl(t_env **env) {
    char *shlvl_str = get_env_value(*env, "SHLVL");
    int shlvl = shlvl_str ? ft_atoi(shlvl_str) : 0;
    shlvl++;
    char *new = ft_itoa(shlvl);
    set_env_value(env, "SHLVL", new);
    free(new);
}
```

---

## Common Pitfalls & Solutions

### 1. Memory Leaks

**Problem:** Not freeing allocated memory

**Solution:** 
- Free tokens after parsing
- Free commands after execution
- Free environment on exit
- Use valgrind regularly

```bash
valgrind --leak-check=full ./minishell
```

### 2. Zombie Processes

**Problem:** Not waiting for child processes

**Solution:**
```c
while (waitpid(-1, &status, 0) > 0)
    ; // Wait for all children
```

### 3. File Descriptor Leaks

**Problem:** Not closing pipes and files

**Solution:**
- Close unused pipe ends immediately
- Close files after use
- Close all fds before execve

### 4. Export Sorting

**Problem:** Not showing variables alphabetically

**Solution:**
- Create sorted copy before printing
- Don't modify original environment order

### 5. Quote Handling

**Problem:** Not removing quotes correctly

**Solution:**
```c
// Remove outer quotes but keep inner content
"hello 'world'"  →  hello 'world'
'hello "world"'  →  hello "world"
```

### 6. PATH Handling

**Problem:** Commands not found after `unset PATH`

**Solution:**
- Check if PATH exists
- Return "command not found" error
- Builtins still work (no PATH needed)

---

## Testing Strategy

### 1. Manual Testing

Test each feature individually:
```bash
# Builtins
echo hello
cd /tmp && pwd
export TEST=value && env | grep TEST

# Pipes
ls | grep minishell
cat file | wc -l
```

### 2. Comparison Testing

Compare with bash:
```bash
# Your shell
echo $?

# Bash
bash -c "echo \$?"
```

### 3. Edge Cases

```bash
# Empty input
<Enter>

# Only spaces
"   "

# Invalid syntax
| echo
echo >

# Missing files
cat < nonexistent
```

### 4. Memory Testing

```bash
valgrind --leak-check=full --show-leak-kinds=all ./minishell
```

Target: **0 bytes definitely lost**

### 5. Stress Testing

```bash
# Many pipes
cat file | grep a | grep b | grep c | wc -l

# Complex redirections
< in1 < in2 cat > out1 > out2

# Variable expansion
echo "$USER $HOME $PWD $?"
```

---

## Evaluation Tips

### What Evaluators Check

1. **Norminette** - Must pass 100%
2. **Compilation** - No warnings with `-Wall -Wextra -Werror`
3. **Memory** - No leaks, no segfaults
4. **Builtins** - All 7 working correctly
5. **Pipes** - Multiple pipes working
6. **Redirections** - All 4 types working
7. **Signals** - Ctrl-C, Ctrl-D, Ctrl-\\ handling

### Common Eval Questions

**Q: How does your parser work?**
A: "First tokenize input, then validate syntax, then build command structure from tokens."

**Q: Why are builtins not external programs?**
A: "Because they need to modify the shell's own state - cd changes our directory, export changes our environment."

**Q: How do pipes work?**
A: "Create pipe, fork, redirect stdout/stdin to pipe ends, execute, close unused fds."

**Q: What happens with Ctrl-C?**
A: "Signal handler catches SIGINT, interrupts current command, displays new prompt."

**Q: How do you handle SHLVL?**
A: "On initialization, get current SHLVL, increment by 1, update environment."

### Live Demo Examples

Be ready to demonstrate:
```bash
# Basic commands
ls -la
pwd
echo "hello world"

# Pipes
ls -l | grep minishell | wc -l

# Redirections
echo "test" > file.txt
cat < file.txt
cat << EOF
line1
line2
EOF

# Builtins
cd /tmp
export TEST=hello
env | grep TEST
unset TEST
exit 0

# Variables
echo $USER $HOME $?

# Error handling
cd nonexistent
exit hello
```

---

## Key Differences: Mandatory vs Bonus

### Mandatory
- Single pipes: `cmd1 | cmd2`
- Basic redirections: `<`, `>`, `>>`, `<<`
- 7 builtins
- Variable expansion
- Quote handling
- Signals

### Bonus (Not Required)
- `&&` and `||` operators
- `()` subshells  
- `*` wildcards
- More complex features

**Important:** Evaluator won't fail you for missing bonus!

---

## Resources

### Testing Tools
- [Minishell Tester by Lucas Kuhn](https://github.com/LucasKuhn/minishell_tester)
- Valgrind for memory leaks
- Norminette for code style

### Learning Resources
- `man bash` - Bash manual
- `man pipe` - Pipe system call
- `man fork` - Process creation
- `man execve` - Execute program
- `man waitpid` - Wait for child

### Key System Calls
- `readline()` - Read user input
- `fork()` - Create child process
- `pipe()` - Create pipe
- `dup2()` - Duplicate file descriptor
- `execve()` - Execute program
- `waitpid()` - Wait for process
- `access()` - Check file permissions
- `chdir()` - Change directory

---

## Success Checklist

Before evaluation, verify:

- [ ] Compiles without warnings
- [ ] Norminette passes all files
- [ ] No memory leaks (valgrind clean)
- [ ] All 7 builtins working
- [ ] Pipes work (at least 2 commands)
- [ ] All 4 redirections work
- [ ] Variable expansion works
- [ ] Quote handling correct
- [ ] Exit codes correct
- [ ] SHLVL increments
- [ ] Signals handled (Ctrl-C, Ctrl-D, Ctrl-\\)
- [ ] Can handle errors gracefully

**If all checked** → You're ready! 🚀

Good luck with your evaluation!
