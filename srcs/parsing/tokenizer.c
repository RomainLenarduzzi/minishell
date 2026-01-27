#include "../../includes/minishell.h"

static t_token	*create_token(t_token_type type, char *value)
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

static void	add_token(t_token **tokens, t_token *new_token)
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

static int	is_separator(char c)
{
	return (c == ' ' || c == '\t' || c == '|' || c == '<' || c == '>');
}

static char	*extract_word(char *input, int *i)
{
	int		start;
	int		len;
	char	*word;
	char	quote;

	start = *i;
	while (input[*i] && !is_separator(input[*i]))
	{
		if (input[*i] == '\'' || input[*i] == '\"')
		{
			quote = input[*i];
			(*i)++;
			while (input[*i] && input[*i] != quote)
				(*i)++;
			if (input[*i])
				(*i)++;
		}
		else
			(*i)++;
	}
	len = *i - start;
	word = malloc(sizeof(char) * (len + 1));
	if (!word)
		return (NULL);
	len = 0;
	while (start < *i)
		word[len++] = input[start++];
	word[len] = '\0';
	return (word);
}

static void	handle_separator(t_token **tokens, char *input, int *i)
{
	char	*val;
	t_token_type type;

	if (input[*i] == '|')
	{
		val = ft_strdup("|");
		type = TOKEN_PIPE;
		(*i)++;
	}
	else if (input[*i] == '<')
	{
		if (input[*i + 1] == '<')
		{
			val = ft_strdup("<<");
			type = TOKEN_REDIR_HEREDOC;
			(*i) += 2;
		}
		else
		{
			val = ft_strdup("<");
			type = TOKEN_REDIR_IN;
			(*i)++;
		}
	}
	else if (input[*i] == '>')
	{
		if (input[*i + 1] == '>')
		{
			val = ft_strdup(">>");
			type = TOKEN_REDIR_APPEND;
			(*i) += 2;
		}
		else
		{
			val = ft_strdup(">");
			type = TOKEN_REDIR_OUT;
			(*i)++;
		}
	}
	else
		return ;
	add_token(tokens, create_token(type, val));
}

static void	skip_whitespace(char *input, int *i)
{
	while (input[*i] && (input[*i] == ' ' || input[*i] == '\t'))
		(*i)++;
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
		if (input[i] == '|' || input[i] == '<' || input[i] == '>')
			handle_separator(&tokens, input, &i);
		else
		{
			word = extract_word(input, &i);
			if (word && word[0])
				add_token(&tokens, create_token(TOKEN_WORD, word));
		}
	}
	return (tokens);
}
