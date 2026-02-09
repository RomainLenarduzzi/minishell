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

void	ensure_args_capacity(char ***args, int *capacity,
	int target_count, int current_count)
{
	char	**new_args;
	int		i;

	if (target_count + 1 < *capacity)
		return ;
	*capacity *= 2;
	if (target_count + 1 >= *capacity)
		*capacity = target_count + 2;
	new_args = malloc(sizeof(char *) * (*capacity));
	if (!new_args)
		return ;
	i = 0;
	while (i < current_count)
	{
		new_args[i] = (*args)[i];
		i++;
	}
	new_args[i] = NULL;
	free(*args);
	*args = new_args;
}

void	add_glob_matches(char ***args, int *i, int *capacity, char *exp)
{
	char	**matches;
	int		j;

	matches = expand_wildcard(exp);
	if (!matches)
	{
		(*args)[(*i)++] = exp;
		return ;
	}
	ensure_args_capacity(args, capacity, *i + array_len(matches), *i);
	j = 0;
	while (matches[j])
		(*args)[(*i)++] = matches[j++];
	free(matches);
	free(exp);
}

void	add_split_words(char ***args, int *i, int *capacity, char *exp)
{
	char	**words;
	int		j;

	words = split_string(exp, ' ');
	if (!words)
	{
		(*args)[(*i)++] = exp;
		return ;
	}
	ensure_args_capacity(args, capacity, *i + array_len(words), *i);
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
