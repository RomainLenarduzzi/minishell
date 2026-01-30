/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 22:00:05 by romlenar          #+#    #+#             */
/*   Updated: 2026/01/29 22:13:14 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	print_syntax_error(char *token)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd(token, 2);
	ft_putstr_fd("'\n", 2);
}

static int	count_args_no_redir(t_token *tokens, t_data *data)
{
	int		count;
	char	*tmp;

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
		else if (tokens->type >= TOKEN_AND && tokens->type <= TOKEN_RPAREN)
		{
			return (-1);
		}
		else
		{
			tmp = expand_token(tokens->value, data);
			if (tmp && tmp[0] != '\0')
				count++;
			free(tmp);
			tokens = tokens->next;
		}
	}
	return (count);
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

static char	**get_cmd_args(t_cmd *cmd, t_token **toks, t_data *data)
{
	char	**args;
	char	*exp;
	int		i;
	int		c;

	c = count_args_no_redir(*toks, data);
	if (c == -1)
	{
		print_syntax_error((*toks)->value);
		return (NULL);
	}
	args = malloc(sizeof(char *) * (c * 10 + 1));
	if (!args)
		return (NULL);
	i = 0;
	while (*toks && (*toks)->type != TOKEN_PIPE)
	{
		if ((*toks)->type >= TOKEN_REDIR_IN
			&& (*toks)->type <= TOKEN_REDIR_HEREDOC)
		{
			if (handle_redirection(cmd, toks, data))
				return (free(args), NULL);
		}
		else if ((*toks)->type >= TOKEN_AND && (*toks)->type <= TOKEN_RPAREN)
		{
			print_syntax_error((*toks)->value);
			return (free(args), NULL);
		}
		else
		{
			exp = expand_token((*toks)->value, data);
			if (exp && exp[0] != '\0')
			{
				if (ft_strchr((*toks)->value, '$'))
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
	int		has_prev;

	if (!toks)
		return (NULL);
	head = malloc(sizeof(t_cmd));
	if (!head)
		return (NULL);
	curr = head;
	has_prev = 0;
	while (curr)
	{
		curr->redirs = NULL;
		curr->next = NULL;
		curr->args = get_cmd_args(curr, &toks, data);
		if (!curr->args)
			return (data->exit_status = 2, free_cmds(head), NULL);
		if (!curr->args[0] && !curr->redirs)
		{
			if (has_prev || (toks && toks->type == TOKEN_PIPE))
				return (data->exit_status = 2, print_syntax_error("|"), free_cmds(head), NULL);
			return (free_cmds(head), NULL);
		}
		if (toks && toks->type == TOKEN_PIPE)
		{
			toks = toks->next;
			if (!toks)
				return (data->exit_status = 2, print_syntax_error("newline"), free_cmds(head), NULL);
			has_prev = 1;
		}
		else if (!toks)
			break ;
		curr->next = malloc(sizeof(t_cmd));
		curr = curr->next;
	}
	return (head);
}
