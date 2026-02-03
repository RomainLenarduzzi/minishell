/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 21:56:16 by romlenar          #+#    #+#             */
/*   Updated: 2026/02/03 20:50:53 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	*get_val(char *str)
{
	char	*equal_sign;
	char	*value;

	equal_sign = ft_strchr(str, '=');
	if (!equal_sign)
		return (NULL);
	value = ft_strdup(equal_sign + 1);
	return (value);
}

static int	is_valid_identifier(char *key)
{
	int	i;

	if (!key || (!((key[0] >= 'a' && key[0] <= 'z') || (key[0] >= 'A'
					&& key[0] <= 'Z')) && key[0] != '_'))
		return (0);
	i = 1;
	while (key[i])
	{
		if (!((key[i] >= 'a' && key[i] <= 'z') || (key[i] >= 'A'
					&& key[i] <= 'Z') || (key[i] >= '0' && key[i] <= '9'))
			&& key[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

static int	print_export_error(char *arg, char *key)
{
	ft_putstr_fd("minishell: export: `", 2);
	ft_putstr_fd(arg, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
	free(key);
	return (1);
}

static void	print_export_list(t_env *env, int fd)
{
	t_env	*curr;

	curr = env;
	while (curr)
	{
		ft_putstr_fd("declare -x ", fd);
		ft_putstr_fd(curr->key, fd);
		if (curr->value)
		{
			ft_putstr_fd("=\"", fd);
			ft_putstr_fd(curr->value, fd);
			ft_putstr_fd("\"", fd);
		}
		ft_putstr_fd("\n", fd);
		curr = curr->next;
	}
}

int	builtin_export(char **args, t_data *data)
{
	char	*key;
	char	*value;
	int		i;
	int		ret;

	i = 0;
	ret = 0;
	if (!args[1])
		return (print_export_list(data->env, 1), 0);
	while (args[++i])
	{
		key = get_key(args[i]);
		if (!is_valid_identifier(key))
			ret = print_export_error(args[i], key);
		else
		{
			value = get_val(args[i]);
			if (key && value)
				set_env_value(&data->env, key, value);
			(free(key), free(value));
		}
	}
	return (ret);
}
