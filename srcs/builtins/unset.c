/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 21:56:23 by romlenar          #+#    #+#             */
/*   Updated: 2026/01/29 21:56:24 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	builtin_unset(char **args, t_data *data)
{
	int	i;

	i = 1;
	while (args[i])
	{
		unset_env_value(&data->env, args[i]);
		i++;
	}
	return (0);
}
