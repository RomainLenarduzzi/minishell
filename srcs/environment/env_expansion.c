/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_expansion.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 21:56:43 by romlenar          #+#    #+#             */
/*   Updated: 2026/01/29 23:41:37 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	is_var_char(char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
		|| (c >= '0' && c <= '9') || (c == '_'));
}

static char	*get_var_value(char *var_name, t_data *data)
{
	char	*val;

	if (ft_strcmp(var_name, "?") == 0)
		return (ft_itoa(data->exit_status));
	val = get_env_value(data->env, var_name);
	if (!val)
		return (ft_strdup(""));
	return (ft_strdup(val));
}

static void	append_char(char **res, char c)
{
	char	s[2];
	char	*tmp;

	s[0] = c;
	s[1] = '\0';
	tmp = ft_strjoin(*res, s);
	if (!tmp)
		return ;
	free(*res);
	*res = tmp;
}

static void	append_var_value(char **res, char *str, int *i, t_data *data)
{
	int		start;
	char	*name;
	char	*val;
	char	*tmp;

	start = ++(*i);
	if (str[*i] == '?')
		(*i)++;
	else
		while (str[*i] && is_var_char(str[*i]))
			(*i)++;
	name = ft_substr(str, start, *i - start);
	val = get_var_value(name, data);
	tmp = ft_strjoin(*res, val);
	if (tmp)
	{
		free(*res);
		*res = tmp;
	}
	free(name);
	free(val);
}

char	*expand_token(char *str, t_data *data)
{
	char	*res;
	int		i;
	char	q;

	i = 0;
	q = 0;
	res = ft_strdup("");
	while (res && str && str[i])
	{
		if ((str[i] == '\'' || str[i] == '\"') && q == 0)
			q = str[i++];
		else if (str[i] == q)
		{
			q = 0;
			i++;
		}
		else if (str[i] == '$' && (q == 0 || q == '\"') && str[i + 1]
			&& str[i + 1] != ' ' && (q != '\"' || str[i + 1] != '\"'))
			append_var_value(&res, str, &i, data);
		else
			append_char(&res, str[i++]);
	}
	return (res);
}
