#include "../../includes/minishell.h"

char	*find_path_in_env(char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		if (strncmp(envp[i], "PATH=", 5) == 0)
			return (envp[i] + 5);
		i++;
	}
	return (NULL);
}

char	*get_cmd_path(char *cmd, char **envp)
{
	char	**paths;
	char	*path_env;
	char	*part_path;
	char	*full_path;
	int		i;

	if (access(cmd, F_OK) == 0)
		return (ft_strdup(cmd));
	path_env = find_path_in_env(envp);
	if (!path_env)
		return (NULL);
	paths = split_string(path_env, ':');
	i = 0;
	while (paths[i])
	{
		part_path = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(part_path, cmd);
		free(part_path);
		if (access(full_path, F_OK) == 0)
		{
			free_array(paths);
			return (full_path);
		}
		free(full_path);
		i++;
	}
	free_array(paths);
	return (NULL);
}
