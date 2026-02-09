/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 22:00:11 by romlenar          #+#    #+#             */
/*   Updated: 2026/02/04 20:22:08 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	*extract_word(char *input, int *i)
{
	int		start;
	char	quote;

	start = *i;
	while (input[*i] && !is_separator(input[*i]))
	{
		if (input[*i] == '\'' || input[*i] == '\"')
		{
			quote = input[(*i)++];
			while (input[*i] && input[*i] != quote)
				(*i)++;
			if (!input[*i])
			{
				ft_putstr_fd("minishell: unexpected EOF for quote\n", 2);
				return (NULL);
			}
			(*i)++;
		}
		else
			(*i)++;
	}
	return (ft_substr(input, start, *i - start));
}

static void	handle_separator(t_token **tokens, char *input, int *i)
{
	char			*val;
	t_token_type	type;

	val = NULL;
	if (input[*i] == '|')
	{
		val = ft_strdup("|");
		type = TOKEN_PIPE;
		(*i)++;
	}
	else
		handle_redir(input, i, &val, &type);
	if (val)
		add_token(tokens, create_token(type, val));
}

static int	process_token(t_token **tokens, char *input, int *i)
{
	char	*word;

	if (is_separator(input[*i]) && input[*i] != ' ' && input[*i] != '\t')
		handle_separator(tokens, input, i);
	else
	{
		word = extract_word(input, i);
		if (!word)
			return (0);
		add_token(tokens, create_token(TOKEN_WORD, word));
	}
	return (1);
}

t_token	*tokenize(char *input)
{
	t_token	*tokens;
	int		i;

	tokens = NULL;
	i = 0;
	while (input[i])
	{
		skip_whitespace(input, &i);
		if (!input[i])
			break ;
		if (!process_token(&tokens, input, &i))
			return (free_tokens(tokens), NULL);
	}
	if (validate_syntax(tokens) != 0)
		return (free_tokens(tokens), NULL);
	return (tokens);
}
