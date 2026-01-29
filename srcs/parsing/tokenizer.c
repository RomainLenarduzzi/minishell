/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 22:00:11 by romlenar          #+#    #+#             */
/*   Updated: 2026/01/29 22:07:35 by romlenar         ###   ########.fr       */
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
			if (input[*i])
				(*i)++;
		}
		else
			(*i)++;
	}
	return (ft_substr(input, start, *i - start));
}

static void	assign_redir(char **v, t_token_type *t, char *s, t_token_type type)
{
	*v = ft_strdup(s);
	*t = type;
}

static int	handle_redir(char *in, int *i, char **v, t_token_type *t)
{
	if (in[*i] == '<' && in[*i + 1] == '<')
		assign_redir(v, t, "<<", TOKEN_REDIR_HEREDOC);
	else if (in[*i] == '<')
		assign_redir(v, t, "<", TOKEN_REDIR_IN);
	else if (in[*i] == '>' && in[*i + 1] == '>')
		assign_redir(v, t, ">>", TOKEN_REDIR_APPEND);
	else if (in[*i] == '>')
		assign_redir(v, t, ">", TOKEN_REDIR_OUT);
	else
		return (0);
	if (*v)
		*i += ft_strlen(*v);
	return (1);
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
			if (word && word[0])
				add_token(&tokens, create_token(TOKEN_WORD, word));
			else if (word)
				free(word);
		}
	}
	return (tokens);
}
