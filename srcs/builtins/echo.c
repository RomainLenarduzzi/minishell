#include "../../includes/minishell.h"

static int	is_n_flag(char *str)
{
	int	i;

	if (!str || str[0] != '-')
		return (0);
	i = 1;
	while (str[i])
	{
		if (str[i] != 'n')
			return (0);
		i++;
	}
	return (i > 1);
}

int	builtin_echo(char **args)
{
	int	i;
	int	print_newline;

	print_newline = 1;
	i = 1;
	while (args[i] && is_n_flag(args[i]))
	{
		print_newline = 0;
		i++;
	}
	while (args[i])
	{
		printf("%s", args[i]);
		if (args[i + 1])
			printf(" ");
		i++;
	}
	if (print_newline)
		printf("\n");
	return (0);
}
