/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 22:00:05 by romlenar          #+#    #+#             */
/*   Updated: 2026/01/30 22:46:44 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	add_glob_matches(char ***args, int *i, char *exp)
{
	char	**matches;
	int		j;

	matches = expand_wildcard(exp);
	if (!matches)
	{
		(*args)[(*i)++] = exp;
		return ;
	}
	j = 0;
	while (matches[j])
	{
		(*args)[(*i)++] = matches[j];
		j++;
	}
	free(matches);
	free(exp);
}

static void	add_split_words(char ***args, int *i, char *exp)
{
	char	**words;
	int		j;

	words = split_string(exp, ' ');
	if (!words)
	{
		(*args)[(*i)++] = exp;
		return ;
	}
	j = 0;
	while (words[j])
	{
		(*args)[(*i)++] = ft_strdup(words[j]);
		j++;
	}
	free_array(words);
	free(exp);
}

static int	count_tokens_only(t_token *tokens)
{
	int	count;

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
			count++;
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

	args = malloc(sizeof(char *) * (count_tokens_only(*toks) * 10 + 1));
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
			{
				if (ft_strchr(exp, '*') && (*toks)->value[0] != '\'' && (*toks)->value[0] != '"')
					add_glob_matches(&args, &i, exp);
				else if (ft_strchr((*toks)->value, '$'))
					add_split_words(&args, &i, exp);
				else
					args[i++] = exp;
			}
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
