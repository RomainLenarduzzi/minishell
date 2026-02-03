/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   glob_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romlenar <romlenar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 06:26:00 by romlenar          #+#    #+#             */
/*   Updated: 2026/02/03 20:28:36 by romlenar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	match_pattern(const char *pattern, const char *str)
{
	if (*pattern == '\0' && *str == '\0')
		return (1);
	if (*pattern == '*')
	{
		if (match_pattern(pattern + 1, str))
			return (1);
		if (*str != '\0' && match_pattern(pattern, str + 1))
			return (1);
		return (0);
	}
	if (*pattern == *str)
		return (match_pattern(pattern + 1, str + 1));
	return (0);
}

static int	count_matches(DIR *dir, const char *pattern)
{
	struct dirent	*entry;
	int				count;

	count = 0;
	while (1)
	{
		entry = readdir(dir);
		if (!entry)
			break ;
		if (entry->d_name[0] == '.')
			continue ;
		if (match_pattern(pattern, entry->d_name))
			count++;
	}
	return (count);
}

static void	collect_matches(DIR *dir, const char *pattern, char **matches)
{
	struct dirent	*entry;
	int				i;

	i = 0;
	while (1)
	{
		entry = readdir(dir);
		if (!entry)
			break ;
		if (entry->d_name[0] == '.')
			continue ;
		if (match_pattern(pattern, entry->d_name))
			matches[i++] = ft_strdup(entry->d_name);
	}
	matches[i] = NULL;
}

char	**expand_wildcard(const char *pattern)
{
	DIR		*dir;
	char	**matches;
	int		count;

	dir = opendir(".");
	if (!dir)
		return (NULL);
	count = count_matches(dir, pattern);
	if (count == 0)
	{
		closedir(dir);
		return (NULL);
	}
	matches = malloc(sizeof(char *) * (count + 1));
	if (!matches)
	{
		closedir(dir);
		return (NULL);
	}
	rewinddir(dir);
	collect_matches(dir, pattern, matches);
	closedir(dir);
	return (matches);
}
