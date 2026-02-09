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

static void	apply_token_expansion(char ***args, int *i, int *cap, char *exp)
{
	ensure_args_capacity(args, cap, *i, *i);
	(*args)[(*i)++] = exp;
}

static void	handle_token_logic(t_token **tok, t_data *data,
	char ***args, int *i_cap[2])
{
	char	*exp;
	int		*i;
	int		*cap;

	i = i_cap[0];
	cap = i_cap[1];
	exp = expand_token((*tok)->value, data);
	if (exp && exp[0] != '\0')
	{
		ensure_args_capacity(args, cap, *i, *i);
		if (ft_strchr(exp, '*') && (*tok)->value[0] != '\''
			&& (*tok)->value[0] != '"')
			add_glob_matches(args, i, cap, exp);
		else if (ft_strchr((*tok)->value, '$') && (*tok)->value[0] != '\''
			&& (*tok)->value[0] != '"')
			add_split_words(args, i, cap, exp);
		else
			apply_token_expansion(args, i, cap, exp);
	}
	else
		free(exp);
	*tok = (*tok)->next;
}

static char	**get_cmd_args(t_cmd *cmd, t_token **toks, t_data *data)
{
	char	**args;
	int		i;
	int		capacity;
	int		*i_cap[2];

	capacity = count_tokens_only(*toks) * 2 + 1;
	args = malloc(sizeof(char *) * capacity);
	if (!args)
		return (NULL);
	i = 0;
	i_cap[0] = &i;
	i_cap[1] = &capacity;
	while (*toks && (*toks)->type != TOKEN_PIPE)
	{
		if ((*toks)->type >= TOKEN_REDIR_IN
			&& (*toks)->type <= TOKEN_REDIR_HEREDOC)
			handle_redirection(cmd, toks, data);
		else
			handle_token_logic(toks, data, &args, i_cap);
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
