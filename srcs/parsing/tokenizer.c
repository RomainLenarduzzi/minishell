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

static int	check_syntax_error(t_token *c)
{
	if (c->type == TOKEN_PIPE)
	{
		if (!c->next || c->next->type == TOKEN_PIPE)
			return (ft_putstr_fd("minishell: syntax error near `|'\n", 2), 2);
	}
	if (c->type >= TOKEN_REDIR_IN && c->type <= TOKEN_REDIR_HEREDOC)
	{
		if (!c->next || c->next->type != TOKEN_WORD)
			return (ft_putstr_fd("minishell: syntax error near redir\n", 2), 2);
	}
	if (ft_strcmp(c->value, "&&") == 0 || ft_strcmp(c->value, "||") == 0
		|| c->value[0] == '(' || c->value[0] == ')')
	{
		ft_putstr_fd("minishell: syntax error: operator not supported\n", 2);
		return (2);
	}
	return (0);
}

static int	validate_syntax(t_token *tokens)
{
	t_token	*curr;

	if (!tokens)
		return (0);
	if (tokens->type == TOKEN_PIPE)
	{
		ft_putstr_fd("minishell: syntax error near `|'\n", 2);
		return (2);
	}
	curr = tokens;
	while (curr)
	{
		if (check_syntax_error(curr))
			return (2);
		curr = curr->next;
	}
	return (0);
}

t_token	*tokenize(char *input)
{
	t_token	*tokens;
	int		i;
	char	*word;

	tokens = NULL;
	i = 0;
	while (input[i])
	{
		skip_whitespace(input, &i);
		if (!input[i])
			break ;
		if (is_separator(input[i]) && input[i] != ' ' && input[i] != '\t')
			handle_separator(&tokens, input, &i);
		else
		{
			word = extract_word(input, &i);
			if (!word)
				return (free_tokens(tokens), NULL);
			add_token(&tokens, create_token(TOKEN_WORD, word));
		}
	}
	if (validate_syntax(tokens) != 0)
		return (free_tokens(tokens), NULL);
	return (tokens);
}
