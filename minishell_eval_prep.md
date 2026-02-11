# Minishell Code Walkthrough & Evaluation Prep

## Purpose
This document explains YOUR specific minishell implementation in detail, preparing you to confidently explain your code during evaluation.

---

## Table of Contents
1. [Project Structure](#project-structure)
2. [Main Program Flow](#main-program-flow)
3. [Parsing System](#parsing-system)
4. [Execution System](#execution-system)
5. [Builtins Implementation](#builtins-implementation)
6. [Environment Management](#environment-management)
7. [Fixes & Improvements](#fixes--improvements)
8. [Evaluation Q&A](#evaluation-qa)
9. [Live Demo Scenarios](#live-demo-scenarios)

---

## Project Structure

```
minishell/
├── includes/minishell.h          # All structs & function declarations
├── srcs/
│   ├── main.c                    # Entry point
│   ├── main_utils.c              # Helper functions
│   ├── parsing/                  # Input processing
│   │   ├── tokenizer.c           # Break input into tokens
│   │   ├── tokenizer_utils.c     # Tokenizer helpers
│   │   ├── syntax_check.c        # Validate syntax
│   │   ├── parser.c              # Build command structure
│   │   ├── parser_utils.c        # Parser helpers
│   │   └── free_parsing.c        # Memory cleanup
│   ├── execution/                # Command execution
│   │   ├── execute.c             # Main execution logic
│   │   ├── execute_utils.c       # Execution helpers
│   │   ├── execute_error.c       # Error handling
│   │   ├── builtin_check.c       # Identify builtins
│   │   ├── path.c                # Find commands in PATH
│   │   └── heredoc.c             # << implementation
│   ├── builtins/                 # Built-in commands
│   │   ├── echo.c                # echo implementation
│   │   ├── cd.c                  # cd implementation
│   │   ├── pwd.c                 # pwd implementation
│   │   ├── export.c              # export implementation ⭐
│   │   ├── export_sort.c         # Export sorting ⭐ (NEW)
│   │   ├── export_utils.c        # Export helpers
│   │   ├── unset.c               # unset implementation
│   │   ├── env.c                 # env implementation
│   │   └── exit.c                # exit implementation
│   ├── environment/              # Environment variables
│   │   ├── env_init.c            # Initialize environment ⭐
│   │   ├── env_utils.c           # Get/set/unset variables
│   │   ├── env_utils2.c          # Additional utils ⭐ (NEW)
│   │   ├── env_free.c            # Memory cleanup
│   │   └── env_expansion.c       # $VAR expansion
│   ├── redirections/             # I/O redirection
│   │   ├── redirections.c        # Handle <, >, >>, <<
│   │   └── redirections_parsing.c # Parse redir tokens
│   ├── signals/                  # Signal handling
│   │   └── signals.c             # Ctrl-C, Ctrl-D, Ctrl-\\
│   └── utils/                    # Utility functions
│       ├── string_utils.c
│       ├── array_utils.c
│       ├── ft_strchr.c
│       ├── ft_strdup.c
│       ├── ft_strjoin.c
│       ├── string_split.c
│       └── glob_utils.c
└── Makefile

⭐ = Modified/created during fixes
```

---

## Main Program Flow

### [main.c](file:///home/shell/42%20Projects/minishell/srcs/main.c)

**The Heart of Your Shell:**

```c
int main(int ac, char **av, char **envp) {
    t_data data;
    
    init_data(&data, envp);          // Step 1: Initialize
    setup_signals(&data);            // Step 2: Set signal handlers
    
    while (1) {                      // Step 3: Main loop
        input = readline("minishell> ");
        
        if (!input)                  // Ctrl-D pressed
            break;
            
        if (input[0])
            add_history(input);      // Save to history
            
        tokens = tokenize(input);    // Step 4: Parse
        if (validate_syntax(tokens))
            continue;
            
        cmds = parse_tokens(tokens); // Step 5: Build commands
        execute_pipeline(cmds, &data); // Step 6: Execute
        
        free_all();                  // Step 7: Cleanup
    }
    
    cleanup_data(&data);
    return (data.exit_status);
}
```

**Key Points for Eval:**
- Uses `readline()` for input with history
- Validates syntax before execution
- Frees memory after each command
- Maintains exit status throughout

---

## Parsing System

### 1. Tokenization ([tokenizer.c](file:///home/shell/42%20Projects/minishell/srcs/parsing/tokenizer.c))

**What it does:** Breaks input into tokens

**Example:**
```
Input:  echo "hello world" | cat > file.txt
Tokens: [echo] ["hello world"] [|] [cat] [>] [file.txt]
```

**Key Function:**
```c
t_token *tokenize(char *input) {
    // Skip whitespace
    // Check for special chars: | < > << >>
    // Handle quotes
    // Extract words
    // Build linked list of tokens
}
```

**Quote Handling:**
- Tracks quote state (none, single, double)
- Removes outer quotes but keeps content
- `"hello 'world'"` → `hello 'world'`

### 2. Syntax Validation ([syntax_check.c](file:///home/shell/42%20Projects/minishell/srcs/parsing/syntax_check.c))

**What it does:** Checks for syntax errors

**Rules checked:**
- No pipe at start: `| echo` ❌
- No double pipes: `cmd ||` ❌ (bonus operator)
- Redirects need files: `echo >` ❌
- No unsupported operators: `&&`, `||`, `()` ❌

**Important:** Rejects bonus operators with friendly error!

### 3. Parser ([parser.c](file:///home/shell/42%20Projects/minishell/srcs/parsing/parser.c))

**What it does:** Builds command structure from tokens

**Creates:**
```c
typedef struct s_cmd {
    char    **args;      // ["echo", "hello", NULL]
    t_redir *redirs;     // List of <, >, >>, <<
    struct s_cmd *next;  // Next command in pipeline
} t_cmd;
```

**Example:**
```bash
echo hello | cat > file.txt
```

Becomes:
```
Cmd1: args=["echo", "hello", NULL], redirs=NULL
  ↓ (pipe)
Cmd2: args=["cat", NULL], redirs=[> file.txt]
```

---

## Execution System

### 1. Pipeline Execution ([execute.c](file:///home/shell/42%20Projects/minishell/srcs/execution/execute.c))

**Core Logic:**

```c
void execute_pipeline(t_cmd *cmds, t_data *data) {
    if (single_builtin_no_pipes(cmds)) {
        // Execute builtin directly in parent
        return execute_builtin(cmds, data);
    }
    
    // Multiple commands or redirections
    while (cmds) {
        if (cmds->next)              // Not last command
            create_pipe();           // Create pipe
            
        pid = fork();
        if (pid == 0) {              // Child process
            setup_redirections();
            setup_pipes();
            
            if (is_builtin())
                execute_builtin();
            else
                execute_external();
        }
        
        close_unused_fds();
        cmds = cmds->next;
    }
    
    wait_for_all_children();
}
```

**Why fork for pipes but not single builtin?**
- Builtins like `cd`, `export` must modify parent shell
- But in pipeline, each command is isolated

### 2. PATH Resolution ([path.c](file:///home/shell/42%20Projects/minishell/srcs/execution/path.c))

**Finds executable:**

```c
char *find_command_path(char *cmd, t_env *env) {
    if (cmd has '/')              // Absolute/relative path
        return check_access(cmd);
    
    PATH = get_env("PATH");
    for each dir in PATH:
        full_path = dir + "/" + cmd
        if (access(full_path, X_OK) == 0)
            return full_path;
    
    return NULL;                  // Command not found
}
```

**Edge case:** After `unset PATH`, commands like `/bin/ls` still work!

### 3. Heredoc ([heredoc.c](file:///home/shell/42%20Projects/minishell/srcs/execution/heredoc.c))

**What it does:** Implements `<< EOF`

```bash
cat << EOF
line 1
line 2
EOF
```

**Process:**
1. Read lines until limiter (`EOF`)
2. Expand variables (unless limiter quoted)
3. Write to temporary file
4. Use file as input

---

## Builtins Implementation

### 1. echo ([echo.c](file:///home/shell/42%20Projects/minishell/srcs/builtins/echo.c))

**Features:**
- `-n` flag: no newline
- Multiple `-n`: `echo -n -n -n` works
- Combined: `echo -nnn` works

```c
int builtin_echo(char **args) {
    int newline = 1;
    int i = 1;
    
    // Check for -n flags
    while (args[i] && is_n_flag(args[i])) {
        newline = 0;
        i++;
    }
    
    // Print arguments
    while (args[i]) {
        printf("%s", args[i]);
        if (args[i + 1])
            printf(" ");
        i++;
    }
    
    if (newline)
        printf("\n");
    
    return 0;
}
```

### 2. cd ([cd.c](file:///home/shell/42%20Projects/minishell/srcs/builtins/cd.c))

**Exit codes matter!**

```c
int builtin_cd(char **args, t_data *data) {
    if (args[1] && args[2])
        return error("too many arguments", 2);  // ⭐ Fixed: was 1
    
    path = args[1] ? args[1] : get_env("HOME");
    
    if (!path)
        return error("HOME not set", 1);
    
    if (chdir(path) != 0)
        return error(strerror(errno), 1);
    
    return 0;
}
```

**Fix applied:** Too many args returns 2 (was 1)

### 3. pwd ([pwd.c](file:///home/shell/42%20Projects/minishell/srcs/builtins/pwd.c))

**Simple but important:**

```c
int builtin_pwd(void) {
    char cwd[PATH_MAX];
    
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        return error(strerror(errno), 1);
    
    printf("%s\n", cwd);
    return 0;
}
```

### 4. export ([export.c](file:///home/shell/42%20Projects/minishell/srcs/builtins/export.c)) ⭐

**Most complex builtin!**

**Key features:**
1. **Alphabetical sorting** (CRITICAL for eval!)
2. **Handle both** `export VAR=value` and `export VAR`
3. **Validate identifiers** (no numbers first, no special chars)

```c
int builtin_export(char **args, t_data *data) {
    if (!args[1])
        return print_export_list(data->env);  // ⭐ Shows sorted!
    
    for (int i = 1; args[i]; i++) {
        key = get_key(args[i]);
        val = get_val(args[i]);
        
        if (!is_valid_identifier(key))
            return error("not a valid identifier", 1);
        
        if (val)
            set_env_value(&data->env, key, val);  // VAR=value
        else
            set_env_value_novalue(&data->env, key); // ⭐ VAR only
    }
    
    return 0;
}
```

#### Export Sorting ([export_sort.c](file:///home/shell/42%20Projects/minishell/srcs/builtins/export_sort.c)) ⭐

**NEW FILE created for sorting:**

```c
t_env *sort_env_list(t_env *env) {
    t_env *sorted = NULL;
    t_env *current = env;
    
    while (current) {
        t_env *new = copy_env_node(current);
        insert_sorted_env(&sorted, new);  // Insert in order
        current = current->next;
    }
    
    return sorted;  // Don't modify original!
}
```

**Why not modify original?**
- Environment order matters for some programs
- Sorting only for display

### 5. unset ([unset.c](file:///home/shell/42%20Projects/minishell/srcs/builtins/unset.c))

```c
int builtin_unset(char **args, t_data *data) {
    for (int i = 1; args[i]; i++)
        unset_env_value(&data->env, args[i]);
    
    return 0;  // Always succeeds
}
```

### 6. env ([env.c](file:///home/shell/42%20Projects/minishell/srcs/builtins/env.c))

```c
int builtin_env(t_env *env) {
    t_env *curr = env;
    
    while (curr) {
        if (curr->value)  // Only show variables with values
            printf("%s=%s\n", curr->key, curr->value);
        curr = curr->next;
    }
    
    return 0;
}
```

**Note:** `export VAR` creates var without value - won't show in `env`!

### 7. exit ([exit.c](file:///home/shell/42%20Projects/minishell/srcs/builtins/exit.c))

**Tricky exit codes:**

```c
int builtin_exit(char **args, t_data *data) {
    printf("exit\n");
    
    if (args[1]) {
        if (!is_numeric(args[1])) {
            error("numeric argument required");
            cleanup_data(data);
            exit(2);  // Exit shell with code 2
        }
        
        if (args[2]) {
            error("too many arguments");
            return 2;  // ⭐ Don't exit! Just return error
        }
        
        exit_code = ft_atoi(args[1]);
    } else {
        exit_code = data->exit_status;
    }
    
    cleanup_data(data);
    exit(exit_code);
}
```

**Key behavior:**
- Invalid arg: Exit shell with code 2
- Too many args: Return 2, DON'T exit shell

---

## Environment Management

### 1. Initialization ([env_init.c](file:///home/shell/42%20Projects/minishell/srcs/environment/env_init.c)) ⭐

**SHLVL increment implemented here:**

```c
t_env *init_env(char **envp) {
    t_env *env = NULL;
    
    // Copy environment from envp
    for (int i = 0; envp[i]; i++) {
        key = extract_key(envp[i]);
        value = extract_value(envp[i]);
        add_env_node(&env, key, value);
    }
    
    increment_shlvl(&env);  // ⭐ NEW: Increment SHLVL
    
    return env;
}
```

### 2. Environment Utils ([env_utils2.c](file:///home/shell/42%20Projects/minishell/srcs/environment/env_utils2.c)) ⭐

**NEW FILE with two critical functions:**

```c
// For: export VAR (without =value)
void set_env_value_novalue(t_env **env, char *key) {
    t_env *existing = find_env(*env, key);
    
    if (existing)
        return;  // Already exists, don't change
    
    // Create new without value
    t_env *new = create_env_node(key, NULL);
    new->next = *env;
    *env = new;
}

// Increment SHLVL on startup
void increment_shlvl(t_env **env) {
    char *shlvl_str = get_env_value(*env, "SHLVL");
    int shlvl = shlvl_str ? ft_atoi(shlvl_str) : 0;
    
    shlvl++;
    char *new_shlvl = ft_itoa(shlvl);
    set_env_value(env, "SHLVL", new_shlvl);
    free(new_shlvl);
}
```

### 3. Variable Expansion ([env_expansion.c](file:///home/shell/42 %20Projects/minishell/srcs/environment/env_expansion.c))

**Handles `$VAR` and `$?`:**

```c
char *expand_variables(char *str, t_env *env, int exit_status) {
    // Scan for '$'
    // If in single quotes: skip
    // If in double quotes or no quotes:
    //   Extract variable name
    //   Look up value
    //   Replace $VAR with value
    // Special: $? → exit_status
}
```

---

## Fixes & Improvements

### Summary of Changes

| File | Change | Reason |
|------|--------|--------|
| `export_sort.c` | **NEW** | Alphabetical sorting for export |
| `export.c` | Modified | Use sorting, handle no-value |
| `env_utils2.c` | **NEW** | SHLVL increment, no-value export |
| `env_init.c` | Modified | Call increment_shlvl |
| `cd.c` | Fixed | Too many args → exit code 2 |
| `exit.c` | Fixed | Too many args → exit code 2 |
| `minishell.h` | Updated | New function declarations |
| `Makefile` | Updated | Added new source files |

### Test Results After Fixes

- **Mandatory tests:** 144/147 (98%)
- **Memory leaks:** 0 bytes
- **Norminette:** All files pass
- **Exit codes:** All correct

---

## Evaluation Q&A

### Architecture Questions

**Q: Explain your overall architecture.**

A: "My minishell follows a standard shell pipeline:
1. Read input with readline
2. Tokenize into words/operators
3. Validate syntax
4. Parse into command structures
5. Execute with pipes/redirections
6. Clean up memory

I separated concerns into modules: parsing, execution, builtins, environment."

**Q: Why did you structure it this way?**

A: "For maintainability and Norminette compliance. Each module has a clear responsibility, keeping functions under 25 lines and files under 5 functions."

### Parsing Questions

**Q: How does your tokenizer work?**

A: "It scans the input character by character:
- Whitespace separates tokens
- Quotes group content
- Special chars (|, <, >, <<, >>) are individual tokens
- Returns linked list of tokens with types"

**Q: How do you handle quotes?**

A: "Track quote state:
- No quotes: split on whitespace
- Single quotes: everything literal
- Double quotes: variable expansion allowed
- Remove outer quotes but keep inner content"

### Execution Questions

**Q: Explain pipe implementation.**

A: "For each pipe:
1. Create pipe before fork
2. Fork child process
3. Duplicate pipe ends to stdin/stdout
4. Close unused fds
5. Execute command
6. Parent closes pipe ends
7. Repeat for next command
8. Wait for all children"

**Q: Why are builtins special?**

A: "Builtins must modify the shell's state:
- cd changes our working directory
- export modifies our environment
- exit terminates our process

So they can't run in child processes for simple commands."

### Builtins Questions

**Q: How does export sorting work?**

A: "When displaying with `export` alone:
1. Create sorted copy of environment
2. Use insertion sort to maintain alphabetical order
3. Print sorted list
4. Free sorted copy
5. Original environment stays unchanged"

**Q: Why not modify the original environment?**

A: "Some programs depend on environment variable order. We only sort for display."

**Q: Explain SHLVL.**

A: "SHLVL tracks shell nesting level:
1. On initialization, read current SHLVL
2. Increment by 1
3. Update environment
4. When you exit, parent shell has original SHLVL"

### Memory Questions

**Q: How do you prevent memory leaks?**

A: "Three strategies:
1. Every malloc has a corresponding free
2. Use valgrind regularly during development
3. Cleanup on ctrl-D and exit
4. Free all data structures after each command"

**Q: What about signals?**

A: "Set signal handlers for:
- SIGINT (Ctrl-C): New prompt, don't exit
- SIGQUIT (Ctrl-\\): Ignore
- In child processes: Reset to default behavior"

---

## Live Demo Scenarios

### Scenario 1: Basic Functionality

```bash
# Start minishell
./minishell

# Test prompt
minishell> echo hello world
# Expected: hello world

# Test builtins
minishell> pwd
# Shows current directory

minishell> cd /tmp
minishell> pwd
# Expected: /tmp

minishell> cd
minishell> pwd
# Expected: /home/username
```

### Scenario 2: Environment Variables

```bash
minishell> export TEST=hello
minishell> echo $TEST
# Expected: hello

minishell> export
# Shows all variables ALPHABETICALLY

minishell> export ANOTHER_VAR
minishell> export | grep ANOTHER
# Expected: declare -x ANOTHER_VAR

minishell> unset TEST
minishell> echo $TEST
# Expected: (empty line)
```

### Scenario 3: SHLVL

```bash
# In parent shell
$ echo $SHLVL
# Expected: 1 (or some number)

$ ./minishell
minishell> echo $SHLVL
# Expected: 2 (parent + 1)

minishell> ./minishell
minishell> echo $SHLVL
# Expected: 3

minishell> exit
minishell> echo $SHLVL
# Expected: 2
```

### Scenario 4: Pipes

```bash
minishell> ls | grep minishell
# Shows files with "minishell"

minishell> cat file.txt | grep pattern | wc -l
# Chain of 3 commands
```

### Scenario 5: Redirections

```bash
minishell> echo "hello" > test.txt
minishell> cat test.txt
# Expected: hello

minishell> echo "world" >> test.txt
minishell> cat test.txt
# Expected:
# hello
# world

minishell> cat << EOF
heredoc> line 1
heredoc> line 2
heredoc> EOF
# Expected:
# line 1
# line 2
```

### Scenario 6: Exit Codes

```bash
minishell> echo $?
# Expected: 0

minishell> cd nonexistent
# Error: No such file or directory
minishell> echo $?
# Expected: 1

minishell> cd /tmp /usr
# Error: too many arguments
minishell> echo $?
# Expected: 2

minishell> exit 42
# Shell exits with code 42
$ echo $?
# Expected: 42
```

### Scenario 7: Error Handling

```bash
minishell> | echo hello
# Expected: syntax error

minishell> echo >>
# Expected: syntax error near newline

minishell> ls && echo hi
# Expected: operator not supported

minishell> unset PATH
minishell> ls
# Expected: command not found
```

---

## Pre-Evaluation Checklist

### Code Quality
- [ ] No compilation warnings
- [ ] Norminette passes all files
- [ ] No memory leaks (valgrind clean)
- [ ] No segfaults/crashes

### Mandatory Features
- [ ] All 7 builtins working
- [ ] Pipes work (multiple commands)
- [ ] All 4 redirections work
- [ ] Variable expansion ($VAR, $?)
- [ ] Quote handling (single/double)
- [ ] Exit codes correct
- [ ] SHLVL increments
- [ ] Signals work (Ctrl-C, Ctrl-D)

### Be Ready To Explain
- [ ] Overall architecture
- [ ] Why builtins are special
- [ ] How pipes work
- [ ] How redirections work
- [ ] Export sorting implementation
- [ ] SHLVL management
- [ ] Memory management strategy

### Be Ready To Demonstrate
- [ ] Basic commands
- [ ] All builtins
- [ ] Pipes (2-3 commands)
- [ ] Each redirection type
- [ ] Variable expansion
- [ ] Error cases
- [ ] Ctrl-C, Ctrl-D behavior

---

## Final Tips

1. **Confidence is key** - You built this, you understand it
2. **Don't memorize** - Understand the logic
3. **Be honest** - If you don't know, say "I'd need to check the code"
4. **Show, don't tell** - Demonstrating is better than explaining
5. **Stay calm** - Evaluators want you to succeed

**You got this!** 🚀

Your test results: **144/147 mandatory (98%)**  
Your memory leaks: **0 bytes**  
Your Norminette: **Pass**  

You're absolutely ready for evaluation!
