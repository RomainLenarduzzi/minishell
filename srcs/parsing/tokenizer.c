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
			if (!input[*i])
			{
				ft_putstr_fd("minishell: unexpected EOF while looking for matching `", 2);
				write(2, &quote, 1);
				ft_putstr_fd("'\n", 2);
				return (NULL);
			}
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

static int	validate_syntax(t_token *tokens)
{
	t_token	*curr;

	if (!tokens)
		return (0);
	if (tokens->type == TOKEN_PIPE)
		return (ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2), 2);
	curr = tokens;
	while (curr)
	{
		if (curr->type == TOKEN_PIPE)
		{
			if (!curr->next)
				return (ft_putstr_fd("minishell: syntax error near unexpected token `newline'\n", 2), 2);
			if (curr->next->type == TOKEN_PIPE)
				return (ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2), 2);
		}
		if (curr->type >= TOKEN_REDIR_IN && curr->type <= TOKEN_REDIR_HEREDOC)
		{
			if (!curr->next || curr->next->type != TOKEN_WORD)
			{
				if (!curr->next)
					return (ft_putstr_fd("minishell: syntax error near unexpected token `newline'\n", 2), 2);
				ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
				ft_putstr_fd(curr->next->value, 2);
				return (ft_putstr_fd("'\n", 2), 2);
			}
		}
		if (ft_strcmp(curr->value, "&&") == 0)
			return (ft_putstr_fd("minishell: syntax error near unexpected token `&&'\n", 2), 2);
		if (ft_strcmp(curr->value, "||") == 0)
			return (ft_putstr_fd("minishell: syntax error near unexpected token `||'\n", 2), 2);
		if (curr->value[0] == '(' || curr->value[0] == ')')
		{
			ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
			write(2, &curr->value[0], 1);
			return (ft_putstr_fd("'\n", 2), 2);
		}
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
			{
				free_tokens(tokens);
				return (NULL);
			}
			if (word[0])
				add_token(&tokens, create_token(TOKEN_WORD, word));
			else if (word)
				free(word);
		}
	}
	if (validate_syntax(tokens) != 0)
	{
		free_tokens(tokens);
		return (NULL);
	}
	return (tokens);
}
