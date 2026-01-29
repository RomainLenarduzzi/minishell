#include "../../includes/minishell.h"

void	print_exec_error(char *cmd, char *msg)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd, 2);
	ft_putstr_fd(": ", 2);
	ft_putstr_fd(msg, 2);
	ft_putstr_fd("\n", 2);
}

void	dup_child_fds(int prev_fd, int pipe_fd[2], int has_next)
{
	if (prev_fd != -1)
	{
		dup2(prev_fd, STDIN_FILENO);
		close(prev_fd);
	}
	if (has_next)
	{
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
		close(pipe_fd[0]);
	}
}

void	wait_all_children(pid_t last_pid, t_data *data)
{
	int	status;

	waitpid(last_pid, &status, 0);
	if (WIFEXITED(status))
		data->exit_status = WEXITSTATUS(status);
	while (wait(NULL) > 0)
		;
}

int	exec_builtin_cmd(t_cmd *cmd, t_data *data)
{
	int	ret;
	int	fd_in;
	int	fd_out;

	if (resolve_redirections(cmd, &fd_in, &fd_out) != 0)
		return (1);
	ret = 0;
	if (ft_strcmp(cmd->args[0], "echo") == 0)
		ret = builtin_echo(cmd->args, fd_out);
	else if (ft_strcmp(cmd->args[0], "cd") == 0)
		ret = builtin_cd(cmd->args, data);
	else if (ft_strcmp(cmd->args[0], "pwd") == 0)
		ret = builtin_pwd(fd_out);
	else if (ft_strcmp(cmd->args[0], "export") == 0)
		ret = builtin_export(cmd->args, data);
	else if (ft_strcmp(cmd->args[0], "unset") == 0)
		ret = builtin_unset(cmd->args, data);
	else if (ft_strcmp(cmd->args[0], "env") == 0)
		ret = builtin_env(data, fd_out);
	else if (ft_strcmp(cmd->args[0], "exit") == 0)
		ret = builtin_exit(cmd->args, data);
	if (fd_in != STDIN_FILENO)
		close(fd_in);
	if (fd_out != STDOUT_FILENO)
		close(fd_out);
	return (ret);
}
