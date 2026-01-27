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
