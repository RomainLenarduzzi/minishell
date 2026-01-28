#include "../../includes/minishell.h"



static int	count_args_no_redir(t_token *tokens)
{
	int	count;

	count = 0;
	while (tokens && tokens->type != TOKEN_PIPE)
	{
		if (tokens->type == TOKEN_REDIR_IN || tokens->type == TOKEN_REDIR_OUT
			|| tokens->type == TOKEN_REDIR_APPEND || tokens->type == TOKEN_REDIR_HEREDOC)
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

static char	**get_cmd_args(t_cmd *cmd, t_token **tokens, t_data *data)
{
	char	**args;
	int		count;
	int		i;

	count = count_args_no_redir(*tokens);
	args = malloc(sizeof(char *) * (count + 1));
	if (!args)
		return (NULL);
	i = 0;
	while (*tokens && (*tokens)->type != TOKEN_PIPE)
	{
		if ((*tokens)->type == TOKEN_REDIR_IN || (*tokens)->type == TOKEN_REDIR_OUT
			|| (*tokens)->type == TOKEN_REDIR_APPEND || (*tokens)->type == TOKEN_REDIR_HEREDOC)
		{
			handle_redirection(cmd, tokens, data);
		}
		else
		{
			args[i] = expand_token((*tokens)->value, data);
			*tokens = (*tokens)->next;
			i++;
		}
	}
	args[i] = NULL;
	if (*tokens && (*tokens)->type == TOKEN_PIPE)
		*tokens = (*tokens)->next;
	return (args);
}

t_cmd	*parse_tokens(t_token *tokens, t_data *data)
{
	t_cmd	*head;
	t_cmd	*curr;

	if (!tokens)
		return (NULL);
	head = NULL;
	curr = NULL;
	while (tokens)
	{
		if (!head)
		{
			head = malloc(sizeof(t_cmd));
			curr = head;
		}
		else
		{
			curr->next = malloc(sizeof(t_cmd));
			curr = curr->next;
		}
		if (!curr)
			return (NULL);
		curr->redirs = NULL;
		curr->next = NULL;
		curr->args = get_cmd_args(curr, &tokens, data);
	}
	return (head);
}
