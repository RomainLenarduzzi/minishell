/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 22:14:52 by romlenar          #+#    #+#             */
/*   Updated: 2026/02/04 20:24:13 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <curses.h>
# include <dirent.h>
# include <errno.h>
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <term.h>
# include <termios.h>
# include <unistd.h>

/* Global variable for signal handling */
extern int			g_signal;

/* Token types */
typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_REDIR_HEREDOC,
	TOKEN_END
}					t_token_type;

/* Token structure */
typedef struct s_token
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
}					t_token;

/* Command structure */

/* Environment variable structure */
typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}					t_env;

/* Redirection structure */
typedef struct s_redir
{
	t_token_type	type;
	char			*filename;
	struct s_redir	*next;
}					t_redir;

/* Command structure */
typedef struct s_cmd
{
	char			**args;
	t_redir			*redirs;
	struct s_cmd	*next;
}					t_cmd;

/* Main data structure */
typedef struct s_data
{
	t_token			*tokens;
	t_cmd			*cmds;
	t_env			*env;
	char			**envp;
	int				exit_status;
}					t_data;

/* Main */
void				init_data(t_data *data, char **envp);
void				cleanup_data(t_data *data);

/* Parsing */
t_token				*tokenize(char *input);
t_cmd				*parse_tokens(t_token *tokens, t_data *data);
void				free_tokens(t_token *tokens);
void				free_cmds(t_cmd *cmds);

/* Redirections */
int					handle_redirections(t_cmd *cmd);
void				handle_redirection(t_cmd *cmd, t_token **tokens,
						t_data *data);
int					resolve_redirections(t_cmd *cmd, int *fd_in, int *fd_out);

/* Execution */
void				execute_command(t_data *data);
int					execute_builtin(t_cmd *cmd, t_data *data);
int					is_builtin(char *cmd);
char				*get_cmd_path(char *cmd, char **envp);
/* Main Utils */
void				init_data(t_data *data, char **envp);
void				cleanup_data(t_data *data);
void				process_input(char *input, t_data *data);

void				process_heredocs(t_data *data);
void				cleanup_heredocs(t_data *data);

/* Builtins */
int					builtin_echo(char **args, int fd_out);
int					builtin_cd(char **args, t_data *data);
int					builtin_pwd(int fd_out);
int					builtin_export(char **args, t_data *data);
int					builtin_unset(char **args, t_data *data);
int					builtin_env(t_data *data, int fd_out);
int					builtin_exit(char **args, t_data *data);

int					resolve_redirections(t_cmd *cmd, int *fd_in, int *fd_out);

/* Environment */
char				*get_key(char *env_str);
char				*get_env_value(t_env *env, char *key);
void				set_env_value(t_env **env, char *key, char *value);
void				set_env_value_novalue(t_env **env, char *key);
void				increment_shlvl(t_env **env);
void				unset_env_value(t_env **env, char *key);
t_env				*init_env(char **envp);
void				free_env(t_env *env);
t_env				*sort_env_list(t_env *env);

/* Signals */
void				setup_signals(void);
void				handle_sigint(int sig);
void				handle_sigquit(int sig);

/* Utils */
char				**split_string(char *str, char delimiter);
void				free_array(char **array);
int					array_len(char **array);
char				*ft_strchr(const char *s, int c);
char				*ft_strdup(char *s);
char				*ft_strjoin(char *s1, char *s2);
void				ft_putstr_fd(char *s, int fd);
int					ft_strlen(char *str);
int					ft_strcmp(char *s1, char *s2);
int					ft_atoi(char *str);
char				*ft_itoa(int n);
char				*ft_substr(char const *s, unsigned int start, size_t len);
char				*expand_token(char *str, t_data *data);

char				*expand_token(char *str, t_data *data);

/* Glob utils */
char				**expand_wildcard(const char *pattern);

/* Tokenizer Utils */
t_token				*create_token(t_token_type type, char *value);
void				add_token(t_token **tokens, t_token *new_token);
int					is_separator(char c);
void				skip_whitespace(char *input, int *i);
int					handle_redir(char *in, int *i, char **v, t_token_type *t);
int					validate_syntax(t_token *tokens);

/* Execution Utils */
void				print_exec_error(char *cmd, char *msg);
void				handle_empty_cmd(t_data *data);
void				handle_no_permission(char *cmd, t_data *data);
void				handle_cmd_not_found(char *cmd, t_data *data);
int					exec_builtin_cmd(t_cmd *cmd, t_data *data);
int					exec_builtin_in_child(t_cmd *cmd, t_data *data);
void				exit_child(t_data *data, int exit_code);
void				dup_child_fds(int prev_fd, int pipe_fd[2], int has_next);
void				wait_all_children(pid_t last_pid, t_data *data);

/* Parser Utils */
void				add_glob_matches(char ***args, int *i, int *capacity,
								char *exp);
void				add_split_words(char ***args, int *i, int *capacity,
								char *exp);
int					count_tokens_only(t_token *tokens);
void				ensure_args_capacity(char ***args, int *capacity,
								int target_count, int current_count);

#endif
