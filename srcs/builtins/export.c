#include "../../includes/minishell.h"

static char	*get_key(char *str)
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

static char	*get_val(char *str)
{
	int	i;

	i = 0;
	while (str[i] && str[i] != '=')
		i++;
	if (!str[i])
		return (NULL);
	return (ft_strdup(&str[i + 1]));
}

int	builtin_export(char **args, t_data *data)
{
	char	*key;
	char	*value;
	int		i;

	i = 1;
	while (args[i])
	{
		key = get_key(args[i]);
		value = get_val(args[i]);
		if (key && value)
			set_env_value(&data->env, key, value);
		free(key);
		free(value);
		i++;
	}
	return (0);
}
