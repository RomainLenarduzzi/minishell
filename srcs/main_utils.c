/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 22:09:58 by romlenar          #+#    #+#             */
/*   Updated: 2026/01/29 22:09:59 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	init_data(t_data *data, char **envp)
{
	data->tokens = NULL;
	data->cmds = NULL;
	data->env = init_env(envp);
	data->envp = envp;
	data->exit_status = 0;
}

void	cleanup_data(t_data *data)
{
	if (data->tokens)
		free_tokens(data->tokens);
	if (data->cmds)
		free_cmds(data->cmds);
	if (data->env)
		free_env(data->env);
	rl_clear_history();
}

void	process_input(char *input, t_data *data)
{
	if (!input || input[0] == '\0')
		return ;
	data->tokens = tokenize(input);
	if (!data->tokens)
	{
		data->exit_status = 1;
		return ;
	}
	data->cmds = parse_tokens(data->tokens, data);
	free_tokens(data->tokens);
	data->tokens = NULL;
	if (!data->cmds)
		return ;
	execute_command(data);
	free_cmds(data->cmds);
	data->cmds = NULL;
}
