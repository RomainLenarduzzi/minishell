/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 21:59:41 by romlenar          #+#    #+#             */
/*   Updated: 2026/01/29 21:59:42 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	*get_filename(void)
{
	static int	i;
	char		*num;
	char		*name;

	num = ft_itoa(i++);
	name = ft_strjoin("/tmp/.minishell_heredoc_", num);
	free(num);
	return (name);
}

int	handle_heredoc(t_redir *redir, t_data *data)
{
	char	*line;
	char	*fname;
	int		fd;

	fname = get_filename();
	fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		return (free(fname), perror("heredoc open"), -1);
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, redir->filename) == 0)
		{
			free(line);
			break ;
		}
		line = expand_token(line, data);
		ft_putstr_fd(line, fd);
		ft_putstr_fd("\n", fd);
		free(line);
	}
	close(fd);
	(free(redir->filename), redir->filename = fname);
	return (0);
}

void	process_heredocs(t_data *data)
{
	t_cmd	*cmd;
	t_redir	*redir;

	cmd = data->cmds;
	while (cmd)
	{
		redir = cmd->redirs;
		while (redir)
		{
			if (redir->type == TOKEN_REDIR_HEREDOC)
				handle_heredoc(redir, data);
			redir = redir->next;
		}
		cmd = cmd->next;
	}
}

void	cleanup_heredocs(t_data *data)
{
	t_cmd	*cmd;
	t_redir	*redir;

	cmd = data->cmds;
	while (cmd)
	{
		redir = cmd->redirs;
		while (redir)
		{
			if (redir->type == TOKEN_REDIR_HEREDOC && redir->filename)
				unlink(redir->filename);
			redir = redir->next;
		}
		cmd = cmd->next;
	}
}
