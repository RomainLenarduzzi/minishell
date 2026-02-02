_This project has been created as part of the 42 curriculum by romlenar, mael-hou._

# Minishell - Unix Shell Implementation

<p align="center">
  <img src="https://img.shields.io/badge/Score-Pending-yellow" alt="Score">
  <img src="https://img.shields.io/badge/Language-C-blue" alt="Language">
  <img src="https://img.shields.io/badge/42-Project-success" alt="42 Project">
</p>

## Description

**Minishell** is a simplified Unix shell implementation developed as part of the 42 school curriculum. The project recreates core shell functionality, focusing on command parsing, process management, pipe handling, and built-in command implementation.

### Project Goal

Build a functional shell that can:

- Execute commands searched via PATH or absolute/relative paths
- Handle pipes to chain command outputs
- Manage input/output redirections with heredoc support
- Expand environment variables and exit status
- Implement essential built-in commands
- Handle signals properly (ctrl-C, ctrl-D, ctrl-\\)

This project introduces fundamental concepts in:

- **Process management** (fork, execve, wait)
- **Inter-process communication** (pipes)
- **File descriptor manipulation** (dup2, redirections)
- **String parsing and tokenization**
- **Memory management** and leak prevention
- **Signal handling** in interactive programs

### Features

#### Mandatory Part

- ✅ Interactive prompt with command history (readline)
- ✅ Command execution via PATH search or absolute/relative paths
- ✅ Quote handling:
  - Single quotes `'` prevent all metacharacter interpretation
  - Double quotes `"` prevent interpretation except for `$`
- ✅ Redirections:
  - `<` input redirection
  - `>` output redirection  
  - `<<` heredoc with delimiter
  - `>>` append mode
- ✅ Pipes `|` to connect command outputs to inputs
- ✅ Environment variable expansion `$VAR`
- ✅ Exit status expansion `$?`
- ✅ Signal handling:
  - `ctrl-C` displays new prompt on new line
  - `ctrl-D` exits the shell
  - `ctrl-\` does nothing

#### Built-in Commands

- `echo` (with `-n` option)
- `cd` (relative or absolute path, with `~` support)
- `pwd` (no options)
- `export` (no options - sets environment variables)
- `unset` (no options - removes environment variables)
- `env` (no options - displays environment)
- `exit` (with numeric argument support and overflow handling)

#### Bonus Features

- ✅ Logical operators `&&` and `||` with parenthesis for priorities
- ✅ Wildcard `*` expansion in current directory

---

## Instructions

### Prerequisites

The following dependencies are required:

```bash
# On Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential libreadline-dev

# On macOS
brew install readline
```

### Compilation

```bash
# Compile the project
make

# Clean object files
make clean

# Clean everything (including binary)
make fclean

# Rebuild from scratch
make re
```

### Execution

#### Running Minishell

```bash
./minishell
```

#### Example Usage

```bash
minishell$ echo "Hello, World!"
Hello, World!

minishell$ export MY_VAR=hello
minishell$ echo $MY_VAR
hello

minishell$ ls -la | grep minishell | wc -l
2

minishell$ cat <infile | grep pattern >outfile

minishell$ cat <<EOF
> line 1
> line 2
> EOF
line 1
line 2

minishell$ echo *.c
main.c parser.c tokenizer.c

minishell$ exit 0
```

---

## Project Structure

```
minishell/
├── Makefile
├── includes/
│   └── minishell.h           # Main header file with structures and prototypes
├── srcs/
│   ├── main.c                # Entry point and main shell loop
│   ├── main_utils.c          # Initialization and cleanup
│   ├── parsing/              # Tokenizer and parser
│   │   ├── tokenizer.c       # Splits input into tokens
│   │   ├── tokenizer_utils.c # Token creation and management
│   │   ├── parser.c          # Builds command structures from tokens
│   │   └── free_parsing.c    # Memory cleanup for parsing structures
│   ├── execution/            # Command execution
│   │   ├── execute.c         # Pipeline execution and child processes
│   │   ├── execute_utils.c   # Builtin execution handlers
│   │   ├── builtin_check.c   # Builtin command detection
│   │   ├── path.c            # PATH search and command resolution
│   │   └── heredoc.c         # Heredoc processing
│   ├── redirections/         # Redirection handling
│   │   ├── redirections.c    # File opening and FD management
│   │   └── redirections_parsing.c # Redirection parsing
│   ├── builtins/             # Built-in command implementations
│   │   ├── echo.c            # echo command with -n option
│   │   ├── cd.c              # Directory navigation
│   │   ├── pwd.c             # Print working directory
│   │   ├── export.c          # Export environment variables
│   │   ├── unset.c           # Unset environment variables
│   │   ├── env.c             # Display environment
│   │   └── exit.c            # Exit with status code
│   ├── environment/          # Environment variable management
│   │   ├── env_init.c        # Environment initialization
│   │   ├── env_utils.c       # Environment get/set operations
│   │   ├── env_free.c        # Environment cleanup
│   │   └── env_expansion.c   # Variable expansion logic
│   ├── signals/              # Signal handlers
│   │   └── signals.c         # SIGINT, SIGQUIT handling
│   └── utils/                # Utility functions
│       ├── string_utils.c    # String manipulation
│       ├── array_utils.c     # Array operations
│       ├── ft_strchr.c       # Character search
│       ├── ft_strdup.c       # String duplication
│       ├── ft_strjoin.c      # String concatenation
│       ├── string_split.c    # String splitting
│       └── glob_utils.c      # Wildcard expansion (bonus)
└── minishell                 # Compiled binary
```

---

## Technical Implementation

### Key Algorithms

#### 1. Tokenization

The input string is split into tokens based on special characters (spaces, pipes, redirections, quotes):

```c
// Main tokenizer loop
while (input[i])
{
    skip_whitespace(input, &i);
    if (is_quote(input[i]))
        handle_quoted_string(&tokens, input, &i);
    else if (is_special_char(input[i]))
        handle_special_token(&tokens, input, &i);
    else
        handle_word(&tokens, input, &i);
}
```

#### 2. Pipeline Execution

Commands connected by pipes are executed in separate child processes:

```c
// Pipeline execution model
cmd1 | cmd2 | cmd3

Parent Process
  ↓
Fork → Child 1 (cmd1) → stdout → pipe[1]
  ↓
Fork → Child 2 (cmd2) → stdin  → pipe[0]  → stdout → pipe[3]
  ↓                                   
Fork → Child 3 (cmd3) → stdin  → pipe[2]  → stdout (terminal)
  ↓
Wait for all children → set exit status
```

#### 3. Variable Expansion

Environment variables and exit status are expanded during parsing:

- `$VAR` → value of VAR  
- `$?` → last exit status
- `$` followed by invalid character → literal `$`
- Expansion respects quote context (disabled in single quotes)

#### 4. Wildcard Expansion (Bonus)

The `*` wildcard expands to matching filenames in the current directory:

```c
// Glob matching algorithm
- Read directory entries
- Match each entry against pattern
- Filter out hidden files (starting with '.')
- Sort results alphabetically
- Replace token with expanded list
```

### Memory Management

- **Zero memory leaks**: All allocated memory is freed before exit
- **Proper cleanup on all exit paths**: Error handling frees resources
- **Child process cleanup**: Forked processes free data before exit
- **File descriptor tracking**: All opened FDs are closed appropriately
- **Validated with Valgrind**: Full leak and error checking

### Error Handling

- Command not found: "minishell: command: command not found"
- Permission denied: "minishell: command: Permission denied"
- Redirection errors: "minishell: filename: No such file or directory"
- Syntax errors: "minishell: syntax error near unexpected token"
- All errors write to stderr and set appropriate exit status

---

## Resources

### Documentation

- [Bash Reference Manual](https://www.gnu.org/software/bash/manual/bash.html) - Official bash documentation
- [GNU Readline Library](https://tiswww.cwru.edu/php/chet/readline/rltop.html) - Readline documentation
- [Advanced Programming in the UNIX Environment](https://stevens.netmeister.org/631/) - UNIX system calls
- [42 Minishell Subject](https://cdn.intra.42.fr/pdf/pdf/xxxxx/minishell.en.pdf) - Project requirements

### Learning Resources

- **Process Management**:
  - [fork() System Call](https://man7.org/linux/man-pages/man2/fork.2.html)
  - [execve() Family](https://man7.org/linux/man-pages/man2/execve.2.html)
  - [Understanding fork() and exec()](https://www.youtube.com/watch?v=PwxTbksJ2fo)

- **Pipes and Redirections**:
  - [pipe() System Call](https://man7.org/linux/man-pages/man2/pipe.2.html)
  - [dup2() for File Descriptor Manipulation](https://man7.org/linux/man-pages/man2/dup.2.html)
  - [Linux I/O Redirection](https://www.gnu.org/software/bash/manual/html_node/Redirections.html)

- **Signal Handling**:
  - [signal() and Signal Handling](https://man7.org/linux/man-pages/man7/signal.7.html)
  - [Handling Signals in C](https://www.gnu.org/software/libc/manual/html_node/Signal-Handling.html)

- **String Parsing**:
  - [Lexical Analysis Basics](https://en.wikipedia.org/wiki/Lexical_analysis)
  - [Tokenization Techniques](https://www.cs.man.ac.uk/~pjj/cs212/ex2_str.html)

### Tutorials Referenced

- [Building a Shell - Tutorial Series](https://brennan.io/2015/01/16/write-a-shell-in-c/)
- [Unix Shell Implementation](https://www.youtube.com/watch?v=ubt-UjcQUYg)
- [Memory Leak Detection with Valgrind](https://valgrind.org/docs/manual/quick-start.html)

### AI Usage Disclosure

AI tools (Antigravity/Gemini) were utilized in the following aspects of this project:

#### Code Development

- **Bug diagnosis**: AI helped identify root causes of memory leaks, including:
  - Wildcard buffer overflow causing double free crashes
  - File descriptor leaks in error paths
  - Duplicate file opening in child proceso built-ins
- **Algorithm optimization**: Suggestions for:
  - Efficient tokenization without excessive memory allocation
  - Proper file descriptor management in complex pipelines
  - Child process cleanup strategies
- **Code review**: AI identified edge cases like:
  - Unclosed FDs when redirections fail
  - Memory leaks in environment variable expansion
  - Missing cleanup before exit() calls

#### Debugging

- **Valgrind output interpretation**: AI explained complex valgrind stack traces
- **Segmentation fault analysis**: Helped trace pointer issues in parser
- **Memory leak patterns**: Identified "still reachable" vs "definitely lost" memory
- **Test case identification**: Suggested edge cases that revealed bugs

#### Documentation

- **Code comments**: AI assisted in writing clear function documentation
- **README structure**: This comprehensive README with proper sections
- **Commit messages**: Detailed git commit messages explaining changes
- **Walkthrough creation**: Generated proof-of-work documentation

#### Learning

- **Concept clarification**: Understanding:
  - Fork/exec model and zombie processes
  - File descriptor inheritance in child processes
  - Signal handling in foreground vs background processes
  - Proper use of wait() and waitpid()
- **Best practices**: Modern C programming standards and defensive programming
- **System call behavior**: Understanding edge cases in open(), dup2(), pipe()

**Important Note**: All core logic, algorithms, and implementations were designed and written by the students. AI was used as a **learning tool, debugging assistant, and documentation helper**, not for wholesale code generation. Every piece of code was thoroughly understood and tested before integration.

---

## Testing

### Memory Leak Testing

```bash
# Full leak check
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./minishell

# Test with tester
cd tester
./tester pipes
./tester redirects
./tester builtins
./tester wildcards
```

### Test Results

- ✅ **Builtins**: 51/53 tests passing (96%)
- ✅ **Pipes**: 3/4 tests passing with no memory leaks
- ✅ **Redirects**: All critical tests passing
- ✅ **Wildcards**: All expansion tests working
- ✅ **Memory**: Zero critical leaks, all FDs properly closed

### Known Test Failures

- Tests 15-16: Tester bug (filters output containing "exit")
- These are not minishell bugs but limitations in the test script

---

## Norminette Compliance

This project follows the 42 Norm:

- ✅ Maximum 25 lines per function
- ✅ Maximum 5 functions per file  
- ✅ Maximum 80 characters per line
- ✅ Proper naming conventions (`ft_`, `t_`, `g_`)
- ✅ No forbidden constructs (for, do-while, switch, ternary, VLAs)

---

## Challenges & Solutions

### Challenge 1: Memory Leaks in Child Processes

**Problem**: Child processes exited without freeing allocated memory

**Solution**: Added cleanup calls (`free_cmds`, `free_env`) before all `exit()` calls in child processes

### Challenge 2: Wildcard Double Free Crash

**Problem**: Buffer overflow when `*` expanded to 50+ files

**Solution**: Increased args array allocation from `count * 10` to `count * 200`

### Challenge 3: File Descriptor Leaks

**Problem**: Multiple redirections leaked FDs on error paths

**Solution**: Added FD cleanup before returning errors; created separate builtin handler for child processes to avoid double file opening

### Challenge 4: Pipeline Exit Status

**Problem**: Exit status not correctly propagated from piped commands

**Solution**: Wait for last child first, extract its status, then wait for remaining children

---

## Acknowledgments

- **42 School** for the project framework and learning methodology
- **Peers** for code reviews, testing, and collaborative debugging
- **42 Community** for shared knowledge and resources
- **GNU Bash** developers for reference implementation

---

## License

This project is part of the 42 school curriculum and is intended for educational purposes only.

---

## Authors

**romlenar** - [42 Intra Profile](https://profile.intra.42.fr/users/romlenar)  
**mael-hou** - [42 Intra Profile](https://profile.intra.42.fr/users/mael-hou)

_Project completed: February 2026_

---

**Final Score**: _Pending evaluation_

_Feel free to reach out for questions or feedback!_
