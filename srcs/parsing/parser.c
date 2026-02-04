/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 22:00:05 by romlenar          #+#    #+#             */
/*   Updated: 2026/02/04 20:17:25 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	handle_token_logic(char ***args, int *i, t_token **toks, t_data *d)
{
	char	*exp;

	exp = expand_token((*toks)->value, d);
	if (exp && exp[0] != '\0')
	{
		if (ft_strchr(exp, '*') && (*toks)->value[0] != '\''
			&& (*toks)->value[0] != '"')
			add_glob_matches(args, i, exp);
		else if (ft_strchr((*toks)->value, '$') && (*toks)->value[0] != '\''
			&& (*toks)->value[0] != '"')
			add_split_words(args, i, exp);
		else
			(*args)[(*i)++] = exp;
	}
	else
		free(exp);
	*toks = (*toks)->next;
}

static char	**get_cmd_args(t_cmd *cmd, t_token **toks, t_data *data)
{
	char	**args;
	int		i;

	args = malloc(sizeof(char *) * (count_tokens_only(*toks) * 200 + 1));
	if (!args)
		return (NULL);
	i = 0;
	while (*toks && (*toks)->type != TOKEN_PIPE)
	{
		if ((*toks)->type >= TOKEN_REDIR_IN
			&& (*toks)->type <= TOKEN_REDIR_HEREDOC)
			handle_redirection(cmd, toks, data);
		else
			handle_token_logic(&args, &i, toks, data);
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
