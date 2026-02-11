/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 20:11:50 by romlenar          #+#    #+#             */
/*   Updated: 2026/02/11 20:11:50 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	set_env_value_novalue(t_env **env, char *key)
{
	t_env	*tmp;
	t_env	*new_node;

	tmp = *env;
	while (tmp)
	{
		if (ft_strcmp(tmp->key, key) == 0)
			return ;
		tmp = tmp->next;
	}
	new_node = malloc(sizeof(t_env));
	if (!new_node)
		return ;
	new_node->key = ft_strdup(key);
	new_node->value = NULL;
	new_node->next = *env;
	*env = new_node;
}

void	increment_shlvl(t_env **env)
{
	char	*shlvl_str;
	int		shlvl;
	char	*new_shlvl;

	shlvl_str = get_env_value(*env, "SHLVL");
	if (!shlvl_str)
		shlvl = 0;
	else
		shlvl = ft_atoi(shlvl_str);
	shlvl++;
	new_shlvl = ft_itoa(shlvl);
	if (!new_shlvl)
		return ;
	set_env_value(env, "SHLVL", new_shlvl);
	free(new_shlvl);
}
