#include "../../includes/minishell.h"

int	builtin_env(t_data *data)
{
	t_env	*tmp;

	tmp = data->env;
	while (tmp)
	{
		printf("%s=%s\n", tmp->key, tmp->value);
		tmp = tmp->next;
	}
	return (0);
}
