/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_parsing.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 22:00:01 by romlenar          #+#    #+#             */
/*   Updated: 2026/01/29 22:00:02 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	free_tokens(t_token *tokens)
{
	t_token	*tmp;

	while (tokens)
	{
		tmp = tokens;
		tokens = tokens->next;
		free(tmp->value);
		free(tmp);
	}
}

void	free_cmds(t_cmd *cmds)
{
	t_cmd	*tmp;
	t_redir	*redir;
	t_redir	*tmp_redir;

	while (cmds)
	{
		tmp = cmds;
		cmds = cmds->next;
		free_array(tmp->args);
		redir = tmp->redirs;
		while (redir)
		{
			tmp_redir = redir;
			redir = redir->next;
			free(tmp_redir->filename);
			free(tmp_redir);
		}
		free(tmp);
	}
}
