#include "../../includes/minishell.h"

int	handle_redirections(t_cmd *cmd)
{
	int		fd;
	t_redir	*redir;

	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == TOKEN_REDIR_IN || redir->type == TOKEN_REDIR_HEREDOC)
		{
			fd = open(redir->filename, O_RDONLY);
			if (fd < 0)
			{
				perror(redir->filename);
				return (-1);
			}
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		else if (redir->type == TOKEN_REDIR_OUT || redir->type == TOKEN_REDIR_APPEND)
		{
			if (redir->type == TOKEN_REDIR_APPEND)
				fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
			else
				fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd < 0)
			{
				perror(redir->filename);
				return (-1);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		redir = redir->next;
	}
	return (0);
}

int	resolve_redirections(t_cmd *cmd, int *fd_in, int *fd_out)
{
	int		fd;
	t_redir	*redir;

	*fd_in = STDIN_FILENO;
	*fd_out = STDOUT_FILENO;
	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == TOKEN_REDIR_IN || redir->type == TOKEN_REDIR_HEREDOC)
		{
			fd = open(redir->filename, O_RDONLY);
			if (fd < 0)
			{
				perror(redir->filename);
				return (-1);
			}
			if (*fd_in != STDIN_FILENO)
				close(*fd_in);
			*fd_in = fd;
		}
		else if (redir->type == TOKEN_REDIR_OUT || redir->type == TOKEN_REDIR_APPEND)
		{
			if (redir->type == TOKEN_REDIR_APPEND)
				fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
			else
				fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd < 0)
			{
				perror(redir->filename);
				return (-1);
			}
			if (*fd_out != STDOUT_FILENO)
				close(*fd_out);
			*fd_out = fd;
		}
		redir = redir->next;
	}
	return (0);
}
