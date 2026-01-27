# Minishell

A simple shell implementation in C, mimicking the behavior of bash.

## Project Description

Minishell is a 42 School project that aims to create a basic shell with features like command execution, pipes, redirections, environment variables, and built-in commands.

## Features

### Mandatory Features
- ✅ Display a prompt when waiting for a new command
- ✅ Working history (using readline)
- ✅ Search and launch executables (via PATH or absolute/relative paths)
- ✅ Quote handling:
  - Single quotes `'` prevent metacharacter interpretation
  - Double quotes `"` prevent interpretation except for `$`
- ✅ Redirections:
  - `<` input redirection
  - `>` output redirection
  - `<<` heredoc (with delimiter)
  - `>>` append mode
- ✅ Pipes `|` to connect command outputs to inputs
- ✅ Environment variable expansion `$VAR`
- ✅ Exit status expansion `$?`
- ✅ Signal handling:
  - `ctrl-C` displays new prompt on new line
  - `ctrl-D` exits the shell
  - `ctrl-\` does nothing

### Built-in Commands
- `echo` (with `-n` option)
- `cd` (relative or absolute path)
- `pwd` (no options)
- `export` (no options)
- `unset` (no options)
- `env` (no options/arguments)
- `exit` (no options)

## Project Structure

```
minishell/
├── Makefile
├── includes/
│   └── minishell.h          # Main header file
├── srcs/
│   ├── main.c               # Entry point
│   ├── parsing/             # Tokenizer and parser
│   │   ├── tokenizer.c
│   │   ├── parser.c
│   │   └── free_parsing.c
│   ├── execution/           # Command execution
│   │   ├── execute.c
│   │   └── builtin_check.c
│   ├── builtins/            # Built-in commands
│   │   ├── echo.c
│   │   ├── cd.c
│   │   ├── pwd.c
│   │   ├── export.c
│   │   ├── unset.c
│   │   ├── env.c
│   │   └── exit.c
│   ├── environment/         # Environment variables
│   │   ├── env_init.c
│   │   ├── env_utils.c
│   │   └── env_free.c
│   ├── signals/             # Signal handlers
│   │   └── signals.c
│   └── utils/               # Utility functions
│       ├── string_utils.c
│       ├── array_utils.c
│       ├── ft_strdup.c
│       └── ft_strjoin.c
└── minishell                # Compiled binary
```

## Compilation

### Requirements
- `gcc` or `cc` compiler
- `readline` library (`libreadline-dev` on Ubuntu/Debian)
- `make`

### Build Commands

```bash
# Compile the project
make

# Clean object files
make clean

# Clean everything (including binary)
make fclean

# Recompile everything
make re
```

## Usage

```bash
# Run minishell
./minishell

# Example commands
minishell$ echo "Hello World"
minishell$ ls -la | grep minishell
minishell$ export MY_VAR=hello
minishell$ echo $MY_VAR
minishell$ exit
```

## Norme Compliance

This project follows the 42 Norm:
- Maximum 25 lines per function
- Maximum 5 functions per file
- Maximum 4 parameters per function
- Maximum 5 variables per function
- Snake_case naming convention
- Specific prefixes: `s_` for structs, `t_` for typedefs, `g_` for globals
- No `for`, `do...while`, `switch`, ternary operators, or VLAs

## Current Status

✅ **Base structure created and compiling**

### To Do:
- [ ] Implement tokenizer (parsing/tokenizer.c)
- [ ] Implement parser (parsing/parser.c)
- [ ] Implement command execution logic
- [ ] Implement all built-in commands
- [ ] Implement pipes
- [ ] Implement redirections
- [ ] Implement heredoc
- [ ] Implement environment variable expansion
- [ ] Test with various edge cases
- [ ] Memory leak testing

## Authors

- RomainLenarduzzi
- Your teammate

## Resources

- [Bash Reference Manual](https://www.gnu.org/software/bash/manual/)
- [Readline Documentation](https://tiswww.case.edu/php/chet/readline/rltop.html)
