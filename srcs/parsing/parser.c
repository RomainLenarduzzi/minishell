/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 22:00:05 by romlenar          #+#    #+#             */
/*   Updated: 2026/01/29 22:13:14 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	count_args_no_redir(t_token *tokens, t_data *data)
{
	int		count;
	char	*tmp;

	count = 0;
	while (tokens && tokens->type != TOKEN_PIPE)
	{
		if (tokens->type >= TOKEN_REDIR_IN
			&& tokens->type <= TOKEN_REDIR_HEREDOC)
		{
			tokens = tokens->next;
			if (tokens)
				tokens = tokens->next;
		}
		else
		{
			tmp = expand_token(tokens->value, data);
			(tmp && tmp[0] != '\0' && count++);
			free(tmp);
			tokens = tokens->next;
		}
	}
	return (count);
}

static char	**get_cmd_args(t_cmd *cmd, t_token **toks, t_data *data)
{
	char	**args;
	char	*exp;
	int		i;

	args = malloc(sizeof(char *) * (count_args_no_redir(*toks, data) + 1));
	if (!args)
		return (NULL);
	i = 0;
	while (*toks && (*toks)->type != TOKEN_PIPE)
	{
		if ((*toks)->type >= TOKEN_REDIR_IN
			&& (*toks)->type <= TOKEN_REDIR_HEREDOC)
			handle_redirection(cmd, toks, data);
		else
		{
			exp = expand_token((*toks)->value, data);
			if (exp && exp[0] != '\0')
				args[i++] = exp;
			else
				free(exp);
			*toks = (*toks)->next;
		}
	}
	args[i] = NULL;
	return (args);
}

t_cmd	*parse_tokens(t_token *toks, t_data *data)
{
	t_cmd	*head;
	t_cmd	*curr;

	if (!toks)
		return (NULL);
	head = malloc(sizeof(t_cmd));
	if (!head)
		return (NULL);
	curr = head;
	while (curr)
	{
		curr->redirs = NULL;
		curr->next = NULL;
		curr->args = get_cmd_args(curr, &toks, data);
		if (toks && toks->type == TOKEN_PIPE)
			toks = toks->next;
		if (!toks)
			break ;
		curr->next = malloc(sizeof(t_cmd));
		curr = curr->next;
	}
	return (head);
}
