/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections_parsing.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 22:07:51 by romlenar          #+#    #+#             */
/*   Updated: 2026/01/29 22:07:52 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	*strip_quotes(char *str)
{
	char	*new;
	int		i;
	int		j;
	char	q;

	i = 0;
	j = 0;
	q = 0;
	new = malloc(ft_strlen(str) + 1);
	if (!new)
		return (NULL);
	while (str[i])
	{
		if (!q && (str[i] == '\'' || str[i] == '\"'))
			q = str[i++];
		else if (str[i] == q && ++i)
			q = 0;
		else
			new[j++] = str[i++];
	}
	return (new[j] = '\0', new);
}

static void	add_redir(t_cmd *cmd, t_redir *new_redir)
{
	t_redir	*tmp;

	if (!cmd->redirs)
	{
		cmd->redirs = new_redir;
		return ;
	}
	tmp = cmd->redirs;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new_redir;
}

void	handle_redirection(t_cmd *cmd, t_token **tokens, t_data *data)
{
	t_token_type	type;
	t_redir			*redir;

	type = (*tokens)->type;
	*tokens = (*tokens)->next;
	if (!*tokens)
		return ;
	redir = malloc(sizeof(t_redir));
	if (!redir)
		return ;
	redir->type = type;
	redir->next = NULL;
	if (type == TOKEN_REDIR_HEREDOC)
		redir->filename = strip_quotes((*tokens)->value);
	else
		redir->filename = expand_token((*tokens)->value, data);
	add_redir(cmd, redir);
	*tokens = (*tokens)->next;
}
