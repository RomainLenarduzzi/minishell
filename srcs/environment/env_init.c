/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 21:56:51 by romlenar          #+#    #+#             */
/*   Updated: 2026/02/03 20:48:04 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

char	*get_key(char *env_str)
{
	int		i;
	char	*key;

	i = 0;
	while (env_str[i] && env_str[i] != '=')
		i++;
	key = malloc(sizeof(char) * (i + 1));
	if (!key)
		return (NULL);
	i = 0;
	while (env_str[i] && env_str[i] != '=')
	{
		key[i] = env_str[i];
		i++;
	}
	key[i] = '\0';
	return (key);
}

static char	*get_value(char *env_str)
{
	int		i;
	char	*value;

	i = 0;
	while (env_str[i] && env_str[i] != '=')
		i++;
	if (!env_str[i])
		return (ft_strdup(""));
	i++;
	value = ft_strdup(&env_str[i]);
	return (value);
}

static t_env	*create_env_node(char *env_str)
{
	t_env	*node;

	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	node->key = get_key(env_str);
	node->value = get_value(env_str);
	node->next = NULL;
	return (node);
}

static void	add_env_node(t_env **env, t_env *new_node)
{
	t_env	*tmp;

	if (!*env)
	{
		*env = new_node;
		return ;
	}
	tmp = *env;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new_node;
}

t_env	*init_env(char **envp)
{
	t_env	*env;
	t_env	*new_node;
	int		i;

	env = NULL;
	i = 0;
	while (envp[i])
	{
		new_node = create_env_node(envp[i]);
		if (!new_node)
			return (NULL);
		add_env_node(&env, new_node);
		i++;
	}
	increment_shlvl(&env);
	return (env);
}
