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

int	handle_heredoc(t_cmd *cmd)
{
	char	*line;
	char	*filename;
	int		fd;

	if (!cmd->heredoc_delim)
		return (0);
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
		if (ft_strcmp(line, cmd->heredoc_delim) == 0)
		{
			free(line);
			break ;
		}
		ft_putstr_fd(line, fd);
		ft_putstr_fd("\n", fd);
		free(line);
	}
	close(fd);
	cmd->infile = filename;
	return (0);
}

void	process_heredocs(t_data *data)
{
	t_cmd	*cmd;

	cmd = data->cmds;
	while (cmd)
	{
		if (cmd->heredoc_delim)
			handle_heredoc(cmd);
		cmd = cmd->next;
	}
}

void	cleanup_heredocs(t_data *data)
{
	t_cmd	*cmd;

	cmd = data->cmds;
	while (cmd)
	{
		if (cmd->heredoc_delim && cmd->infile)
		{
			unlink(cmd->infile);
		}
		cmd = cmd->next;
	}
}
