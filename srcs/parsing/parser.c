#include "../../includes/minishell.h"

static char	*strip_quotes(char *str)
{
	char	*new_str;
	int		i;
	int		j;
	char	quote;

	if (!str)
		return (NULL);
	new_str = malloc(sizeof(char) * (ft_strlen(str) + 1));
	if (!new_str)
		return (NULL);
	i = 0;
	j = 0;
	while (str[i])
	{
		if (str[i] == '\'' || str[i] == '\"')
		{
			quote = str[i++];
			while (str[i] && str[i] != quote)
				new_str[j++] = str[i++];
			if (str[i])
				i++;
		}
		else
			new_str[j++] = str[i++];
	}
	new_str[j] = '\0';
	return (new_str);
}

static void	handle_redirection(t_cmd *cmd, t_token **tokens, t_data *data)
{
	t_token_type	type;

	type = (*tokens)->type;
	*tokens = (*tokens)->next;
	if (!*tokens)
		return ; // Should be error
	if (type == TOKEN_REDIR_IN)
		cmd->infile = expand_token((*tokens)->value, data);
	else if (type == TOKEN_REDIR_OUT)
	{
		cmd->outfile = expand_token((*tokens)->value, data);
		cmd->append = 0;
	}
	else if (type == TOKEN_REDIR_APPEND)
	{
		cmd->outfile = expand_token((*tokens)->value, data);
		cmd->append = 1;
	}
	else if (type == TOKEN_REDIR_HEREDOC)
	{
		cmd->heredoc_delim = strip_quotes((*tokens)->value); 
	}
	*tokens = (*tokens)->next;
}

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
		curr->infile = NULL;
		curr->outfile = NULL;
		curr->append = 0;
		curr->heredoc_delim = NULL;
		curr->next = NULL;
		curr->args = get_cmd_args(curr, &tokens, data);
	}
	return (head);
}
