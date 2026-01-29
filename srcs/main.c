#include "../includes/minishell.h"

int	g_signal = 0;


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
