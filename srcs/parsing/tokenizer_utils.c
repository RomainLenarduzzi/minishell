/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 22:42:43 by romlenar          #+#    #+#             */
/*   Updated: 2026/02/04 20:23:59 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

t_token	*create_token(t_token_type type, char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->value = value;
	token->next = NULL;
	return (token);
}

void	add_token(t_token **tokens, t_token *new_token)
{
	t_token	*tmp;

	if (!*tokens)
	{
		*tokens = new_token;
		return ;
	}
	tmp = *tokens;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new_token;
}

int	is_separator(char c)
{
	return (c == ' ' || c == '\t' || c == '|' || c == '<' || c == '>');
}

void	skip_whitespace(char *input, int *i)
{
	while (input[*i] && (input[*i] == ' ' || input[*i] == '\t'))
		(*i)++;
}

int	handle_redir(char *in, int *i, char **v, t_token_type *t)
{
	if (in[*i] == '<' && in[*i + 1] == '<')
		*v = ft_strdup("<<");
	else if (in[*i] == '<')
		*v = ft_strdup("<");
	else if (in[*i] == '>' && in[*i + 1] == '>')
		*v = ft_strdup(">>");
	else if (in[*i] == '>')
		*v = ft_strdup(">");
	else
		return (0);
	if (!*v)
		return (0);
	if (ft_strlen(*v) == 2 && in[*i] == '<')
		*t = TOKEN_REDIR_HEREDOC;
	else if (ft_strlen(*v) == 2)
		*t = TOKEN_REDIR_APPEND;
	else if (in[*i] == '<')
		*t = TOKEN_REDIR_IN;
	else
		*t = TOKEN_REDIR_OUT;
	*i += ft_strlen(*v);
	return (1);
}
