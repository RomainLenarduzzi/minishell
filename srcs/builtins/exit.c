#include "../../includes/minishell.h"

static int	ft_atoi(char *str)
{
	int	result;
	int	sign;
	int	i;

	result = 0;
	sign = 1;
	i = 0;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result * sign);
}

int	builtin_exit(char **args, t_data *data)
{
	int	exit_code;

	printf("exit\n");
	exit_code = 0;
	if (args[1])
		exit_code = ft_atoi(args[1]);
	cleanup_data(data);
	exit(exit_code);
	return (0);
}
