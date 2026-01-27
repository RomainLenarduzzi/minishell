
#include "../../includes/minishell.h"

static int	exec_builtin_cmd(t_cmd *cmd, t_data *data)
{
	int	ret;

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
	return (ret);
}

void	handle_exec_error(char *cmd)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd, 2);
	ft_putstr_fd(": command not found\n", 2);
}

static void	handle_redirections(t_cmd *cmd)
{
	int	fd;

	if (cmd->infile)
	{
		fd = open(cmd->infile, O_RDONLY);
		if (fd < 0)
		{
			perror(cmd->infile);
			exit(1);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
	if (cmd->outfile)
	{
		if (cmd->append)
			fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
			fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd < 0)
		{
			perror(cmd->outfile);
			exit(1);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}
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
	handle_redirections(cmd);
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

