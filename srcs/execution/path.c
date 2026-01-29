/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 21:59:45 by romlenar          #+#    #+#             */
/*   Updated: 2026/01/29 21:59:46 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	char	*full;
	char	*tmp;
	int		i;

	if (cmd && ft_strchr(cmd, '/') && access(cmd, F_OK) == 0)
		return (ft_strdup(cmd));
	tmp = find_path_in_env(envp);
	if (!tmp || !cmd || !cmd[0])
		return (NULL);
	paths = split_string(tmp, ':');
	i = -1;
	while (paths && paths[++i])
	{
		tmp = ft_strjoin(paths[i], "/");
		full = ft_strjoin(tmp, cmd);
		free(tmp);
		if (access(full, F_OK) == 0)
			return (free_array(paths), full);
		free(full);
	}
	free_array(paths);
	return (NULL);
}
