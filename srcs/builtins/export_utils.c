/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 20:43:09 by romlenar          #+#    #+#             */
/*   Updated: 2026/02/03 20:46:35 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	*get_key(char *str)
{
	int		i;
	char	*key;

	i = 0;
	while (str[i] && str[i] != '=')
		i++;
	key = malloc(sizeof(char) * (i + 1));
	if (!key)
		return (NULL);
	i = 0;
	while (str[i] && str[i] != '=')
	{
		key[i] = str[i];
		i++;
	}
	key[i] = '\0';
	return (key);
}

char	*get_val(char *str)
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
