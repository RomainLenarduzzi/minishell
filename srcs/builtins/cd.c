#include "../../includes/minishell.h"

int	builtin_cd(char **args, t_data *data)
{
	char	*path;

	(void)data;
	if (!args[1])
	{
		path = getenv("HOME");
		if (!path)
		{
			printf("cd: HOME not set\n");
			return (1);
		}
	}
	else
		path = args[1];
	if (chdir(path) != 0)
	{
		perror("cd");
		return (1);
	}
	return (0);
}
