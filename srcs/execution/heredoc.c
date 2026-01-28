#include "../../includes/minishell.h"

static char	*get_filename(void)
{
	static int	i = 0;
	char		*num;
	char		*name;

	num = ft_itoa(i++);
	name = ft_strjoin("/tmp/minishell_heredoc_", num);
	free(num);
	return (name);
}

int	handle_heredoc(t_redir *redir)
{
	char	*line;
	char	*filename;
	int		fd;
	char	*delim;

	delim = redir->filename;
	filename = get_filename();
	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
	{
		perror("heredoc open");
		free(filename);
		return (-1);
	}
	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (ft_strcmp(line, delim) == 0)
		{
			free(line);
			break ;
		}
		ft_putstr_fd(line, fd);
		ft_putstr_fd("\n", fd);
		free(line);
	}
	close(fd);
	free(delim); // Free the delimiter as we replace it
	redir->filename = filename;
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
				handle_heredoc(redir);
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
			{
				unlink(redir->filename);
			}
			redir = redir->next;
		}
		cmd = cmd->next;
	}
}
