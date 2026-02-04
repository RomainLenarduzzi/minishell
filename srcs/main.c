/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 22:10:01 by romlenar          #+#    #+#             */
/*   Updated: 2026/02/04 20:38:19 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int			g_signal = 0;

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
			(free(tmp), (void)0);
		}
		if (buf[0] == '\n')
			break ;
	}
	return (line);
}

static char	*get_next_input(void)
{
	char	*input;
	int		len;

	if (isatty(STDIN_FILENO))
		return (readline("minishell$ "));
	input = get_line();
	if (input)
	{
		len = ft_strlen(input);
		if (len > 0 && input[len - 1] == '\n')
			input[len - 1] = '\0';
	}
	return (input);
}

void	shell_loop(t_data *data)
{
	char	*input;

	while (1)
	{
		input = get_next_input();
		if (!input)
		{
			if (isatty(STDIN_FILENO))
				ft_putstr_fd("exit\n", 1);
			break ;
		}
		if (input[0])
		{
			add_history(input);
			process_input(input, data);
		}
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
