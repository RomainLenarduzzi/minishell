/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_sort.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 20:11:30 by romlenar          #+#    #+#             */
/*   Updated: 2026/02/11 20:11:30 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static t_env	*copy_env_node(t_env *node)
{
	t_env	*new_node;

	new_node = malloc(sizeof(t_env));
	if (!new_node)
		return (NULL);
	new_node->key = ft_strdup(node->key);
	new_node->value = NULL;
	if (node->value)
		new_node->value = ft_strdup(node->value);
	new_node->next = NULL;
	return (new_node);
}

static void	insert_sorted_env(t_env **sorted, t_env *new_node)
{
	t_env	*current;
	t_env	*prev;

	if (!*sorted || ft_strcmp(new_node->key, (*sorted)->key) < 0)
	{
		new_node->next = *sorted;
		*sorted = new_node;
		return ;
	}
	current = *sorted;
	prev = NULL;
	while (current && ft_strcmp(new_node->key, current->key) > 0)
	{
		prev = current;
		current = current->next;
	}
	new_node->next = current;
	if (prev)
		prev->next = new_node;
}

t_env	*sort_env_list(t_env *env)
{
	t_env	*sorted;
	t_env	*current;
	t_env	*new_node;

	sorted = NULL;
	current = env;
	while (current)
	{
		new_node = copy_env_node(current);
		if (!new_node)
		{
			free_env(sorted);
			return (NULL);
		}
		insert_sorted_env(&sorted, new_node);
		current = current->next;
	}
	return (sorted);
}
