/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 20:18:11 by romlenar          #+#    #+#             */
/*   Updated: 2026/02/04 20:18:31 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	add_glob_matches(char ***args, int *i, char *exp)
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

void	add_split_words(char ***args, int *i, char *exp)
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

int	count_tokens_only(t_token *tokens)
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
