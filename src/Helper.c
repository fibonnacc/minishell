/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Helper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouryal <ybouryal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 09:58:03 by ybouryal          #+#    #+#             */
/*   Updated: 2025/05/07 09:58:09 by ybouryal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	free_token(t_token **token)
{
	t_token	*current;
	t_token	*next;

	current = *token;
	while (current)
	{
		next = current->next;
		if (current->av)
			free(current->av);
		free(current);
		current = next;
	}
}

int	is_space(char c)
{
	if (c == ' ')
		return (0);
	return (1);
}

void	append_arg(t_command *cmd, char *str)
{
	int i;
	int j;
	char	**new_array;

	i = 0;
	if (cmd->args)
	{
		while (cmd->args[i])
			i++;
	}

	new_array = ft_calloc(i + 2, sizeof(char *));
	if (!new_array)
		return;

	j = 0;
	while (j < i)
	{
		new_array[j] = cmd->args[j];
		j++;
	}
	new_array[i] = ft_strdup(str);
	if (!new_array[j])
	{
		free(new_array);
		return ;
	}
	new_array[i + 1] = NULL;
	if (cmd->args)
	{
		free(cmd->args);
	}
	cmd->args = new_array;
}


