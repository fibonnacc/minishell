/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handle_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouryal <ybouryal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 09:32:38 by ybouryal          #+#    #+#             */
/*   Updated: 2025/05/07 09:37:50 by ybouryal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <stddef.h>
#include <stdlib.h>

void	free_cmd(t_command *cmd)
{
	t_command	*current;
	t_command	*next;
	int	i;

	current = cmd;
	while (current)
	{
		next = current->next;
		if (current->args)
		{
			i = 0;
			while (current->args[i])
			{
				free(current->args[i]);
				i++;
			}
			free(current->args);
		}
		if (current->file_input)
			free(current->file_input);
		if (current->file_output)
			free(current->file_output);
		if (current->herdoc)
			free(current->herdoc);
		free(current);
		current = next;
	}
}

t_command	*create_command()
{
	t_command	*cmd;

	cmd = (t_command *)ft_calloc(1, sizeof(t_command));
	if(cmd == NULL)
	{
		return (NULL);
	}
	cmd->args = NULL;
	cmd->file_input = NULL;
	cmd->file_output = NULL;
	cmd->herdoc = NULL;
	cmd->next = NULL;
	return (cmd);
}

char	*manual_realloc(char *old, size_t len)
{
	char *new = malloc(len + 1);
	if (new == NULL)
		return (NULL);
	if (old)
	{
		ft_memcpy(new, old, len);
		free(old);
	}
	new[len] = '\0';
	return (new);
}

char	*expand_env(char *str)
{
	char	*result;
	char	*string;
	char	*valeur;
	size_t	(i), j, old_size, new_size, len, start;

	old_size = (ft_strlen(str) * 2 + 1);
	result = (char *)ft_calloc(old_size, 1);
	if (result == NULL)
		return (NULL);
	i = 0;
	j = 0;
	while (str[i])
	{
		if (str[i] == '$' && ft_isalnum(str[i + 1]) && (is_space(str[i + 1]) != 0) && str[i + 1] && str[i + 1] != '"' && str[i + 1] != '\'')
		{
			i++;
			start = i;
			while((ft_isalnum(str[i]) || str[i] == '_') && str[i])
				i++;
			string = ft_substr(str, start, i - start);	
			if (!string)
				return (NULL);
			valeur = getenv(string);
			free(string);

			len = ft_strlen(valeur);
			if (valeur)
			{
				if (j + len >= old_size)
				{
					new_size = len + old_size;
					result = manual_realloc(result, new_size);
					old_size = new_size;
				}
				ft_strlcpy(&result[j], valeur, len + 1);
				j += len;
			}
			continue;
		}
		result[j++] = str[i++];
	}
	return (result);
}

t_command	*parsing_command(t_token *token)
{
	t_token	*current;
	t_command	*new_cmd;
	t_command	*first_cmd;
	t_command	*current_cmd;
	char	*expanded;

	current_cmd = NULL;
	new_cmd = NULL;
	first_cmd = NULL;
	expanded = NULL;
	current_cmd = create_command();
	if (!current_cmd)
		return (NULL);
	first_cmd = current_cmd;
	current = token;
	while (current)
	{
		if (current->type == TOKEN_PIPE && current->next)
		{
			new_cmd = create_command();
			if (!new_cmd)
			{
				free_cmd(first_cmd);
				return (NULL);
			}
			current_cmd->next = new_cmd;
			current_cmd = new_cmd;
			current = current->next;
		}

		if (current->type == TOKEN_REDIR_IN && current->next && current->next->type == TOKEN_WORD)
		{
			if(current_cmd->file_input)
				free(current_cmd->file_input);
			current_cmd->file_input = ft_strdup(current->next->av);
			if (current_cmd == NULL)
			{
				free_cmd(first_cmd);
				return(NULL);
			}
			current = current->next->next;
			continue;
		}

		if (current->type == TOKEN_REDIR_OUT && current->next && current->next->type == TOKEN_WORD)
		{
			if (current_cmd->file_output)
				free(current_cmd->file_output);
			current_cmd->file_output = ft_strdup(current->next->av);
			if (current_cmd == NULL)
			{
				free_cmd(first_cmd);
				return (NULL);
			}
			current_cmd->append = 0;
			current = current->next->next;
			continue;
		}

		if (current->type == TOKEN_REDIR_APPEND && current->next && current->next->type == TOKEN_WORD)
		{
			if (current_cmd->file_output)
				free(current_cmd->file_output);
			current_cmd->file_output = ft_strdup(current->next->av);
			if (current_cmd == NULL)
			{
				free_cmd(first_cmd);
				return (NULL);
			}
			current_cmd->append = 1;
			current = current->next->next;
			continue;
		}

		if (current->type == TOKEN_HERDOC && current->next && current->next->type == TOKEN_WORD)
		{
			if (current_cmd->herdoc)
				free(current_cmd->herdoc);
			current_cmd->herdoc = ft_strdup(current->next->av);
			if (!current_cmd->herdoc)
			{
				free_cmd(first_cmd);
				return(NULL);
			}
			current = current->next->next;
			continue;
		}

		if (current->type == TOKEN_WORD)
		{
			expanded = expand_env(current->av);
			if (!expanded)
			{
				free_cmd(first_cmd);
				return (NULL);
			}
			append_arg(current_cmd, expanded);
			free(expanded);
		}
		current = current->next;
	}
	return (first_cmd);
}
