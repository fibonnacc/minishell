/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   excute_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:56:53 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/04 14:08:34 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	compare_newline(char **str, bool *j, int *i)
{
	int	k;

	k = 0;
	while (str[*i] && ft_strncmp(str[*i], "-n", 2) == 0)
	{
		k = 1;
		while (str[*i][k])
		{
			if (str[*i][k] != 'n')
			{
				*j = false;
				return ;
			}
			k++;
		}
		*j = true;
		(*i)++;
	}
}

char	*check_file(char *cmd)
{
	DIR	*folder;

	folder = opendir(cmd);
	if (folder != NULL)
	{
		write(2, "minishell: ", 11);
		write(2, cmd, ft_strlen(cmd));
		write(2, ": Is a directory\n", 17);
		set_status(126);
		closedir(folder);
		return (NULL);
	}
	closedir(folder);
	if (access(cmd, X_OK) == 0)
	{
		return (gc_strdup(cmd));
	}
	if (access(cmd, F_OK) == 0)
	{
		return (gc_strdup(cmd));
	}
	return (NULL);
}

void	my_echo(t_command *cmd)
{
	int		i;
	bool	j;

	j = false;
	i = 1;
	if (!cmd->args[i])
	{
		printf("\n");
		return ;
	}
	compare_newline(cmd->args, &j, &i);
	while (cmd->args[i])
	{
		ft_putstr_fd(cmd->args[i], 1);
		if (cmd->args[i + 1])
			ft_putstr_fd(" ", 1);
		i++;
	}
	if (j == false)
		printf("\n");
	fflush(stdout);
	set_status(0);
}

bool	built_in(char *cmd)
{
	if (ft_strcmp(cmd, "exit") == 0)
		return (true);
	if (ft_strcmp(cmd, "echo") == 0)
		return (true);
	if (ft_strcmp(cmd, "cd") == 0)
		return (true);
	if (ft_strcmp(cmd, "pwd") == 0)
		return (true);
	if (ft_strcmp(cmd, "export") == 0)
		return (true);
	if (ft_strcmp(cmd, "unset") == 0)
		return (true);
	if (ft_strcmp(cmd, "env") == 0)
		return (true);
	else
	{
		return (false);
	}
}

void	free_2d_array(char **str)
{
	int	i;

	if (!str)
		return ;
	i = 0;
	while (str[i])
	{
		free(str[i]);
		i++;
	}
	free(str);
}
