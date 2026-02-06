/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 21:59:38 by romlenar          #+#    #+#             */
/*   Updated: 2026/02/03 21:05:04 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	handle_exec_fail(char *path, char *cmd, t_data *data)
{
	struct stat	sb;

	if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
		print_exec_error(cmd, "Is a directory");
	else
		print_exec_error(cmd, strerror(errno));
	free(path);
	exit_child(data, 126);
}

static void	exec_external(t_cmd *cmd, t_data *data)
{
	char		*path;
	struct stat	sb;

	if (cmd->args[0] && cmd->args[0][0] == '\0')
		handle_empty_cmd(data);
	path = get_cmd_path(cmd->args[0], data->envp);
	if (!path)
	{
		if (ft_strchr(cmd->args[0], '/') && stat(cmd->args[0], &sb) == 0)
			handle_no_permission(cmd->args[0], data);
		handle_cmd_not_found(cmd->args[0], data);
	}
	execve(path, cmd->args, data->envp);
	handle_exec_fail(path, cmd->args[0], data);
}

void	exec_child(t_cmd *cmd, t_data *data, int prev_fd, int pipe_fd[2])
{
	dup_child_fds(prev_fd, pipe_fd, (cmd->next != NULL));
	if (handle_redirections(cmd) != 0)
		exit_child(data, 1);
	if (cmd->args && cmd->args[0] && is_builtin(cmd->args[0]))
		exit_child(data, exec_builtin_in_child(cmd, data));
	if (!cmd->args || !cmd->args[0])
		exit_child(data, 0);
	if (cmd->args[0][0] == '\0')
		handle_empty_cmd(data);
	exec_external(cmd, data);
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
