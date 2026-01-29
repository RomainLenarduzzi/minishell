
#include "../../includes/minishell.h"

void	exec_child(t_cmd *cmd, t_data *data, int prev_fd, int pipe_fd[2])
{
	char		*path;
	struct stat	sb;

	dup_child_fds(prev_fd, pipe_fd, (cmd->next != NULL));
	if (handle_redirections(cmd) != 0)
		exit(1);
	if (is_builtin(cmd->args[0]))
		exit(exec_builtin_cmd(cmd, data));
	path = get_cmd_path(cmd->args[0], data->envp);
	if (!path)
	{
		if (strchr(cmd->args[0], '/'))
			print_exec_error(cmd->args[0], "No such file or directory");
		else
			print_exec_error(cmd->args[0], "command not found");
		exit(127);
	}
	if (execve(path, cmd->args, data->envp) == -1)
	{
		if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
			print_exec_error(cmd->args[0], "Is a directory");
		else
			print_exec_error(cmd->args[0], strerror(errno));
		free(path);
		exit(126);
	}
}

void	run_pipeline(t_data *data)
{
	t_cmd	*cmd;
	int		pipe_fd[2];
	int		prev_fd;
	pid_t	pid;
	cmd = data->cmds;
	prev_fd = -1;
	while (cmd)
	{
		if (cmd->next)
			pipe(pipe_fd);
		pid = fork();
		if (pid == 0)
			exec_child(cmd, data, prev_fd, pipe_fd);
		if (prev_fd != -1)
			close(prev_fd);
		if (cmd->next)
		{
			close(pipe_fd[1]);
			prev_fd = pipe_fd[0];
		}
		cmd = cmd->next;
	}
	wait_all_children(pid, data);

}

void	execute_command(t_data *data)
{
	if (!data->cmds)
		return ;
	process_heredocs(data);
	if (data->cmds->args && data->cmds->args[0])
	{
		if (!data->cmds->next && is_builtin(data->cmds->args[0]))
			data->exit_status = exec_builtin_cmd(data->cmds, data);
		else
			run_pipeline(data);
	}
	cleanup_heredocs(data);
}



