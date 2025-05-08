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

char	*expand_env(char *str)
{
	char	*result;
	int	start;
	char	*string;
	char	*valeur;
	int	i;
	int	j;

	result = (char *)ft_calloc(ft_strlen(str) * 2 + 1, 1);
	if (result == NULL)
		return (NULL);
	i = 0;
	j = 0;
	while (str[i])
	{
		if (str[i] == '$' && (is_space(str[i]) != 0) && str[i + 1] && str[i + 1] != '"' && str[i + 1] != '\'')
		{
			i++;
			start = i;
			while((ft_isalnum(str[i]) || str[i] == '_') && str[i])
				i++;
			string = ft_substr(str, start, i - start);	
			printf ("string is : %s\n", string);
			if (!string)
				return (NULL);
			valeur = getenv(string);
			free(string);
			if (valeur)
			{
				ft_strlcpy(&result[j], valeur, (ft_strlen(valeur) + 1));
				j += ft_strlen(valeur);
			}
			free(valeur);
			continue;
		}
		result[j++] = str[i++];
	}
	printf ("result: %s\n", result);
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
