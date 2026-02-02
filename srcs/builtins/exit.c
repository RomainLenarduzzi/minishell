/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 21:56:12 by romlenar          #+#    #+#             */
/*   Updated: 2026/02/02 20:46:13 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	has_overflow(char *str)
{
	long long	num;
	char		*endptr;

	errno = 0;
	num = strtoll(str, &endptr, 10);
	if (errno == ERANGE || *endptr != '\0')
		return (1);
	(void)num;
	return (0);
}

static int	is_str_num(char *str)
{
	int	i;

	i = 0;
	if (str[i] == '-' || str[i] == '+')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (!isdigit(str[i]))
			return (0);
		i++;
	}
	if (has_overflow(str))
		return (0);
	return (1);
}

int	builtin_exit(char **args, t_data *data)
{
	int	exit_code;

	printf("exit\n");
	if (args[1])
	{
		if (!is_str_num(args[1]))
		{
			ft_putstr_fd("minishell: exit: ", 2);
			ft_putstr_fd(args[1], 2);
			ft_putstr_fd(": numeric argument required\n", 2);
			cleanup_data(data);
			exit(2);
		}
		if (args[2])
		{
			ft_putstr_fd("minishell: exit: too many arguments\n", 2);
			return (1);
		}
		exit_code = ft_atoi(args[1]);
	}
	else
		exit_code = data->exit_status;
	cleanup_data(data);
	exit(exit_code);
	return (0);
}
