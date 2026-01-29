#include "../../includes/minishell.h"

static void	skip_quotes(char *input, int *i)
{
	char	quote;

	quote = input[*i];
	(*i)++;
	while (input[*i] && input[*i] != quote)
		(*i)++;
	if (input[*i])
		(*i)++;
}

static char	*extract_word(char *input, int *i)
{
	int		start;
	int		len;

	start = *i;
	while (input[*i] && !is_separator(input[*i]))
	{
		if (input[*i] == '\'' || input[*i] == '\"')
			skip_quotes(input, i);
		else
			(*i)++;
	}
	len = *i - start;
	return (ft_substr(input, start, len));
}

static int	handle_redir(char *input, int *i, char **val, t_token_type *type)
{
	if (input[*i] == '<')
	{
		if (input[*i + 1] == '<')
		{
			*val = ft_strdup("<<");
			*type = TOKEN_REDIR_HEREDOC;
			(*i) += 2;
		}
		else
		{
			*val = ft_strdup("<");
			*type = TOKEN_REDIR_IN;
			(*i)++;
		}
		return (1);
	}
	return (0);
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
	else if (handle_redir(input, i, &val, &type))
		;
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
