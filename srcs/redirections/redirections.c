/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 22:07:54 by romlenar          #+#    #+#             */
/*   Updated: 2026/02/04 20:35:04 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	open_redir_file(t_redir *r)
{
	if (r->type == TOKEN_REDIR_IN || r->type == TOKEN_REDIR_HEREDOC)
		return (open(r->filename, O_RDONLY));
	if (r->type == TOKEN_REDIR_APPEND)
		return (open(r->filename, O_WRONLY | O_CREAT | O_APPEND, 0644));
	return (open(r->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644));
}

int	handle_redirections(t_cmd *cmd)
{
	int		fd;
	t_redir	*r;

	r = cmd->redirs;
	while (r)
	{
		fd = open_redir_file(r);
		if (fd < 0)
			return (perror(r->filename), -1);
		if (r->type == TOKEN_REDIR_IN || r->type == TOKEN_REDIR_HEREDOC)
			dup2(fd, STDIN_FILENO);
		else
			dup2(fd, STDOUT_FILENO);
		close(fd);
		r = r->next;
	}
	return (0);
}

static int	process_redirection(t_redir *r, int *fd_in, int *fd_out)
{
	int	fd;
	int	*target;

	fd = open_redir_file(r);
	if (fd < 0)
	{
		perror(r->filename);
		if (*fd_in > 0)
			close(*fd_in);
		if (*fd_out > 1)
			close(*fd_out);
		return (-1);
	}
	target = fd_out;
	if (r->type == TOKEN_REDIR_IN || r->type == TOKEN_REDIR_HEREDOC)
		target = fd_in;
	if (*target > 1)
		close(*target);
	*target = fd;
	return (0);
}

int	resolve_redirections(t_cmd *cmd, int *fd_in, int *fd_out)
{
	t_redir	*r;

	*fd_in = 0;
	*fd_out = 1;
	r = cmd->redirs;
	while (r)
	{
		if (process_redirection(r, fd_in, fd_out) != 0)
			return (-1);
		r = r->next;
	}
	return (0);
}
