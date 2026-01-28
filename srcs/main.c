#include "../includes/minishell.h"

int	g_signal = 0;

void	init_data(t_data *data, char **envp)
{
	data->tokens = NULL;
	data->cmds = NULL;
	data->env = init_env(envp);
	data->envp = envp;
	data->exit_status = 0;
}

void	cleanup_data(t_data *data)
{
	if (data->tokens)
		free_tokens(data->tokens);
	if (data->cmds)
		free_cmds(data->cmds);
	if (data->env)
		free_env(data->env);
	rl_clear_history();
}

void	process_input(char *input, t_data *data)
{
	if (!input || input[0] == '\0')
		return ;
	if (isatty(STDIN_FILENO))
		add_history(input);
	data->tokens = tokenize(input);
	if (!data->tokens)
		return ;
	data->cmds = parse_tokens(data->tokens, data);
	free_tokens(data->tokens);
	data->tokens = NULL;
	if (!data->cmds)
		return ;
	execute_command(data);
	free_cmds(data->cmds);
	data->cmds = NULL;
}

static char	*get_line(void)
{
	char	*line;
	char	*tmp;
	char	buf[2];
	int		ret;

	line = NULL;
	buf[1] = '\0';
	while (1)
	{
		ret = read(STDIN_FILENO, buf, 1);
		if (ret <= 0)
			break ;
		if (!line)
			line = ft_strdup(buf);
		else
		{
			tmp = line;
			line = ft_strjoin(line, buf);
			free(tmp);
		}
		if (buf[0] == '\n')
			break ;
	}
	return (line);
}

void	shell_loop(t_data *data)
{
	char	*input;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			input = readline("minishell$ ");
		else
		{
			char *line = get_line();
			if (line)
			{
				int len = ft_strlen(line);
				if (len > 0 && line[len - 1] == '\n')
					line[len - 1] = '\0';
				input = line;
			}
			else
				input = NULL;
		}
		if (!input)
		{
			if (isatty(STDIN_FILENO))
				printf("exit\n");
			break ;
		}
		process_input(input, data);
		free(input);
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_data	data;

	(void)argc;
	(void)argv;
	setup_signals();
	init_data(&data, envp);
	shell_loop(&data);
	cleanup_data(&data);
	return (data.exit_status);
}
