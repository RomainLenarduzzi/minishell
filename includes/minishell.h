#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <signal.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <dirent.h>
# include <string.h>
# include <errno.h>
# include <termios.h>
# include <curses.h>
# include <term.h>

/* Global variable for signal handling */
extern int	g_signal;

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
}	t_token_type;

/* Token structure */
typedef struct s_token
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
}	t_token;

/* Command structure */
typedef struct s_cmd
{
	char			**args;
	char			*infile;
	char			*outfile;
	int				append;
	char			*heredoc_delim;
	struct s_cmd	*next;
}	t_cmd;

/* Environment variable structure */
typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

/* Main data structure */
typedef struct s_data
{
	t_token	*tokens;
	t_cmd	*cmds;
	t_env	*env;
	char	**envp;
	int		exit_status;
}	t_data;

/* Main */
void	init_data(t_data *data, char **envp);
void	cleanup_data(t_data *data);

/* Parsing */
t_token	*tokenize(char *input);
t_cmd	*parse_tokens(t_token *tokens, t_data *data);
void	free_tokens(t_token *tokens);
void	free_cmds(t_cmd *cmds);

/* Execution */
void	execute_command(t_data *data);
int		execute_builtin(t_cmd *cmd, t_data *data);
int		is_builtin(char *cmd);
char	*get_cmd_path(char *cmd, char **envp);
void	process_heredocs(t_data *data);
void	cleanup_heredocs(t_data *data);

/* Builtins */
int		builtin_echo(char **args);
int		builtin_cd(char **args, t_data *data);
int		builtin_pwd(void);
int		builtin_export(char **args, t_data *data);
int		builtin_unset(char **args, t_data *data);
int		builtin_env(t_data *data);
int		builtin_exit(char **args, t_data *data);

/* Environment */
char	*get_env_value(t_env *env, char *key);
void	set_env_value(t_env **env, char *key, char *value);
void	unset_env_value(t_env **env, char *key);
t_env	*init_env(char **envp);
void	free_env(t_env *env);

/* Signals */
void	setup_signals(void);
void	handle_sigint(int sig);
void	handle_sigquit(int sig);

/* Utils */
char	**split_string(char *str, char delimiter);
void	free_array(char **array);
int		array_len(char **array);
char	*ft_strdup(char *s);
char	*ft_strjoin(char *s1, char *s2);
void	ft_putstr_fd(char *s, int fd);
int		ft_strlen(char *str);
int		ft_strcmp(char *s1, char *s2);
char	*ft_itoa(int n);
char	*ft_substr(char const *s, unsigned int start, size_t len);
char	*expand_token(char *str, t_data *data);

#endif
