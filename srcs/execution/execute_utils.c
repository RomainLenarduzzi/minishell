/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 21:59:35 by romlenar          #+#    #+#             */
/*   Updated: 2026/01/29 21:59:36 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

int	dispatch_builtin_cmd(t_cmd *cmd, t_data *data, int fd_out)
{
	if (ft_strcmp(cmd->args[0], "echo") == 0)
		return (builtin_echo(cmd->args, fd_out));
	else if (ft_strcmp(cmd->args[0], "cd") == 0)
		return (builtin_cd(cmd->args, data));
	else if (ft_strcmp(cmd->args[0], "pwd") == 0)
		return (builtin_pwd(fd_out));
	else if (ft_strcmp(cmd->args[0], "export") == 0)
		return (builtin_export(cmd->args, data));
	else if (ft_strcmp(cmd->args[0], "unset") == 0)
		return (builtin_unset(cmd->args, data));
	else if (ft_strcmp(cmd->args[0], "env") == 0)
		return (builtin_env(data, fd_out));
	else if (ft_strcmp(cmd->args[0], "exit") == 0)
		return (builtin_exit(cmd->args, data));
	return (0);
}

int	exec_builtin_cmd(t_cmd *cmd, t_data *data)
{
	int	ret;
	int	f[2];

	if (resolve_redirections(cmd, &f[0], &f[1]) != 0)
	{
		if (f[0] != STDIN_FILENO && f[0] > 0)
			close(f[0]);
		if (f[1] != STDOUT_FILENO && f[1] > 0)
			close(f[1]);
		return (1);
	}
	ret = dispatch_builtin_cmd(cmd, data, f[1]);
	if (f[0] != STDIN_FILENO)
		close(f[0]);
	if (f[1] != STDOUT_FILENO)
		close(f[1]);
	return (ret);
}
