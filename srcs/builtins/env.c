/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 21:56:09 by romlenar          #+#    #+#             */
/*   Updated: 2026/01/29 21:56:10 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	builtin_env(t_data *data, int fd_out)
{
	t_env	*tmp;

	tmp = data->env;
	while (tmp)
	{
		if (tmp->key && tmp->value)
		{
			ft_putstr_fd(tmp->key, fd_out);
			ft_putstr_fd("=", fd_out);
			ft_putstr_fd(tmp->value, fd_out);
			ft_putstr_fd("\n", fd_out);
		}
		tmp = tmp->next;
	}
	return (0);
}
