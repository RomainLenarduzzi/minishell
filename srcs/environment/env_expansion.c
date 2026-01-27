#include "../../includes/minishell.h"

static char	*get_var_value(char *var_name, t_data *data)
{
	char	*val;

	if (ft_strcmp(var_name, "?") == 0)
		return (ft_itoa(data->exit_status)); // This leaks memory if I don't free it? ft_itoa accepts int.
        // Wait, ft_itoa allocates. So I should return it.
        // But get_env_value returns pointer to internal env?
        // Let's check get_env_value.
	val = get_env_value(data->env, var_name);
	if (!val)
		return (ft_strdup(""));
	return (ft_strdup(val));
}

static int	is_var_char(char c)
{
	return (isalnum(c) || c == '_' || c == '?');
}

// Advances i past the variable name
static void	append_var_value(char **res, char *str, int *i, t_data *data)
{
	int		start;
	char	*var_name;
	char	*var_value;
	char	*tmp;

	(*i)++; // Skip $
	start = *i;
	if (str[*i] == '?')
		(*i)++;
	else
	{
		while (str[*i] && is_var_char(str[*i]))
			(*i)++;
	}
	if (*i == start) // Just a $
	{
		tmp = ft_strjoin(*res, "$");
		free(*res);
		*res = tmp;
		return ;
	}
	var_name = ft_substr(str, start, *i - start); // Need ft_substr
	var_value = get_var_value(var_name, data);
	free(var_name);
	tmp = ft_strjoin(*res, var_value);
	free(*res);
	free(var_value);
	*res = tmp;
}

char	*expand_token(char *str, t_data *data)
{
	char	*res;
	char	*tmp;
	char	quote;
	int		i;
    // We need to build the result string char by char (or chunk by chunk)
    // Initial result empty
	res = ft_strdup("");
	i = 0;
	quote = 0;
	while (str[i])
	{
		if ((str[i] == '\'' || str[i] == '\"') && quote == 0)
		{
			quote = str[i++];
		}
		else if (str[i] == quote)
		{
			quote = 0;
			i++;
		}
		else if (str[i] == '$' && (quote == 0 || quote == '\"'))
		{
            // Expand
            // Check if next char is space or end, then print $ literally?
            if (!str[i + 1] || str[i + 1] == ' ' || (quote == '\"' && str[i + 1] == '\"'))
            {
                 // Literal $
                 char s[2] = {str[i], 0};
                 tmp = ft_strjoin(res, s);
                 free(res);
                 res = tmp;
                 i++;
            }
            else
			    append_var_value(&res, str, &i, data);
		}
		else
		{
			// Append char
            char s[2] = {str[i], 0};
            tmp = ft_strjoin(res, s);
            free(res);
            res = tmp;
			i++;
		}
	}
	return (res);
}
