
#include "../../includes/minishell.h"



static int	exec_builtin_cmd(t_cmd *cmd, t_data *data)
{
	int	ret;
	int	stdin_backup;
	int	stdout_backup;

	stdin_backup = dup(STDIN_FILENO);
	stdout_backup = dup(STDOUT_FILENO);
	if (handle_redirections(cmd) != 0)
	{
		dup2(stdin_backup, STDIN_FILENO);
		dup2(stdout_backup, STDOUT_FILENO);
		close(stdin_backup);
		close(stdout_backup);
		return (1);
	}
	ret = 0;
	if (ft_strcmp(cmd->args[0], "echo") == 0)
		ret = builtin_echo(cmd->args);
	else if (ft_strcmp(cmd->args[0], "cd") == 0)
		ret = builtin_cd(cmd->args, data);
	else if (ft_strcmp(cmd->args[0], "pwd") == 0)
		ret = builtin_pwd();
	else if (ft_strcmp(cmd->args[0], "export") == 0)
		ret = builtin_export(cmd->args, data);
	else if (ft_strcmp(cmd->args[0], "unset") == 0)
		ret = builtin_unset(cmd->args, data);
	else if (ft_strcmp(cmd->args[0], "env") == 0)
		ret = builtin_env(data);
	else if (ft_strcmp(cmd->args[0], "exit") == 0)
		ret = builtin_exit(cmd->args, data);
	dup2(stdin_backup, STDIN_FILENO);
	dup2(stdout_backup, STDOUT_FILENO);
	close(stdin_backup);
	close(stdout_backup);
	return (ret);
}

void	handle_exec_error(char *cmd)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd, 2);
	ft_putstr_fd(": command not found\n", 2);
}



void	exec_child(t_cmd *cmd, t_data *data, int prev_fd, int pipe_fd[2])
{
	char	*path;

	if (prev_fd != -1)
	{
		dup2(prev_fd, STDIN_FILENO);
		close(prev_fd);
	}
	if (cmd->next)
	{
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
		close(pipe_fd[0]);
	}
	if (handle_redirections(cmd) != 0)
		exit(1);
	if (is_builtin(cmd->args[0]))
		exit(exec_builtin_cmd(cmd, data));
	path = get_cmd_path(cmd->args[0], data->envp);
	if (!path)
	{
		handle_exec_error(cmd->args[0]);
		exit(127);
	}
	if (execve(path, cmd->args, data->envp) == -1)
	{
		perror("execve");
		exit(126);
	}
}

void	run_pipeline(t_data *data)
{
	t_cmd	*cmd;
	int		pipe_fd[2];
	int		prev_fd;
	pid_t	pid;
	int		status;

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
	waitpid(pid, &status, 0); // Wait for last command specifically
	if (WIFEXITED(status))
		data->exit_status = WEXITSTATUS(status);
	while (wait(NULL) > 0)
		;
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

int	execute_builtin(t_cmd *cmd, t_data *data)
{
	return (exec_builtin_cmd(cmd, data));
}

