/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_check.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antigravity <antigravity@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/07 20:00:00 by antigravity       #+#    #+#             */
/*   Updated: 2026/02/07 20:00:00 by antigravity      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	print_syntax_error(char *token)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd(token, 2);
	ft_putstr_fd("'\n", 2);
	return (1);
}

static int	check_pipe_errors(t_token *token)
{
	if (!token->next)
	{
		ft_putstr_fd("minishell: syntax error: unexpected end of file\n", 2);
		return (1);
	}
	if (token->next->type == TOKEN_PIPE)
		return (print_syntax_error("|"));
	return (0);
}

static int	check_redir_errors(t_token *token)
{
	if (!token->next)
		return (print_syntax_error("newline"));
	if (token->next->type != TOKEN_WORD)
		return (print_syntax_error(token->next->value));
	return (0);
}

static int	check_operator_errors(t_token *token)
{
	if (ft_strcmp(token->value, "&&") == 0 || ft_strcmp(token->value, "||") == 0
		|| token->value[0] == '(' || token->value[0] == ')')
	{
		ft_putstr_fd("minishell: syntax error: operator not supported\n", 2);
		return (1);
	}
	return (0);
}

int	validate_syntax(t_token *tokens)
{
	t_token	*curr;

	if (!tokens)
		return (0);
	if (tokens->type == TOKEN_PIPE)
		return (print_syntax_error("|"));
	curr = tokens;
	while (curr)
	{
		if (curr->type == TOKEN_PIPE)
		{
			if (check_pipe_errors(curr))
				return (2);
		}
		else if (curr->type >= TOKEN_REDIR_IN
			&& curr->type <= TOKEN_REDIR_HEREDOC)
		{
			if (check_redir_errors(curr))
				return (2);
		}
		else if (check_operator_errors(curr))
			return (2);
		curr = curr->next;
	}
	return (0);
}
