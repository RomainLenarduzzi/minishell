#include "../../includes/minishell.h"

int	builtin_pwd(int fd_out)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		ft_putstr_fd(cwd, fd_out);
		ft_putstr_fd("\n", fd_out);
		return (0);
	}
	else
	{
		perror("pwd");
		return (1);
	}
}
