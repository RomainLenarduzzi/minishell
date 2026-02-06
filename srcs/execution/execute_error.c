/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_error.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 18:34:00 by romlenar          #+#    #+#             */
/*   Updated: 2026/02/06 18:34:00 by romlenar         ###   ########.fr       */
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

void	handle_empty_cmd(t_data *data)
{
	print_exec_error("", "command not found");
	exit_child(data, 127);
}

void	handle_no_permission(char *cmd, t_data *data)
{
	print_exec_error(cmd, "Permission denied");
	exit_child(data, 126);
}

void	handle_cmd_not_found(char *cmd, t_data *data)
{
	if (ft_strchr(cmd, '/'))
		print_exec_error(cmd, "No such file or directory");
	else
		print_exec_error(cmd, "command not found");
	exit_child(data, 127);
}
