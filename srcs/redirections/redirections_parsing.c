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

static void	add_redir(t_cmd *cmd, t_redir *new_redir)
{
	t_redir	*tmp;

	if (!cmd->redirs)
	{
		cmd->redirs = new_redir;
		return ;
	}
	tmp = cmd->redirs;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new_redir;
}

void	handle_redirection(t_cmd *cmd, t_token **tokens, t_data *data)
{
	t_token_type	type;
	t_redir			*redir;

	type = (*tokens)->type;
	*tokens = (*tokens)->next;
	if (!*tokens)
		return ;
	redir = malloc(sizeof(t_redir));
	if (!redir)
		return ;
	redir->type = type;
	redir->next = NULL;
	if (type == TOKEN_REDIR_HEREDOC)
		redir->filename = strip_quotes((*tokens)->value);
	else
		redir->filename = expand_token((*tokens)->value, data);
	add_redir(cmd, redir);
	*tokens = (*tokens)->next;
}
