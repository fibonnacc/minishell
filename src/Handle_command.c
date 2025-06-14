
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handle_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:26:30 by helfatih          #+#    #+#             */
/*   Updated: 2025/05/16 17:26:32 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <uchar.h>

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

// void	check_quot(char *str, bool *in_quot, int *char_quot, int i)
// {
// 	if (str[i] == '\'')
// 	{
// 		if (*in_quot == false)
// 		{
// 			*in_quot = true;
// 			*char_quot = str[i];
// 		}
// 		else if (*char_quot == str[i])
// 		{
// 			*in_quot = false;
// 			*char_quot = 0;
// 		}
// 	}
// }

bool  con(char *str)
{
  if (str[0] == '\'')
    return(false);
  else
    return(true);
}

bool  flaging(char *str)
{
  if (str[0] != '\'' && str[0] != '\"')
  {
    return (true);
  }
  else
  {
    return(false);
  }
}

char	*expand_env(char *str)
{
	char	*result, *string, *valeur;
	size_t	(i), j, old_size, new_size, len, start;

	old_size = (ft_strlen(str) * 2 + 1);
	result = (char *)ft_calloc(old_size, 1);
	if (result == NULL)
		return (NULL);
	i = 0;
	j = 0;
	bool condition = con(str);
	bool flag = flaging(str);
	while (str[i])
	{
		if (condition && str[i] == '$' && (ft_isalnum(str[i + 1]) || str[i + 1] == '_') && str[i + 1])
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
			if(valeur)
			{
				len = ft_strlen(valeur);
				if (valeur)
				{
					if (j + len >= old_size)
					{
						new_size = len + old_size;
						result = manual_realloc(result, new_size);
						old_size = new_size;
					}
					if (flag)
					{
						int k = 0;
						while (valeur[k])
						{
							if (valeur[k] == ' ')
							{
								while(valeur[k] == ' ')
								{
									k++;
								}
								result[j++] = ' ';
							}
							else
							result[j++] = valeur[k++];
						}
					}
					else
				{
						ft_strlcpy(&result[j], valeur, len + 1);
						j += len;
					}
				}
				continue;
			}
		}	
		result[j++] = str[i++];
	}
	return (result);
}


int	handle_pipe(t_token **current, t_command **current_cmd, t_command *first_cmd)
{
	t_command	*new_cmd;

	if ((*current)->type != TOKEN_PIPE)
		return (1);
	if ((*current)->next == NULL)
	{
		printf("minishell: syntax error near unexpected token `|'\n");
		return (0);
	}
	new_cmd = create_command();
	if (!new_cmd)
	{
		free_cmd(first_cmd);
		return (0);
	}
	(*current_cmd)->next = new_cmd;
	*current_cmd = new_cmd;
	*current = (*current)->next;
	return (1);
}

int	handle_redir_in(t_token **current, t_command *cmd, t_command *first_cmd)
{
	if ((*current)->type != TOKEN_REDIR_IN)
		return (1);
	if (!(*current)->next)
	{
		printf("minishell: syntax error near unexpected token `newline'\n");
		return (0);
	}
	if ((*current)->next->type != TOKEN_WORD)
	{
		printf("minishell: syntax error near unexpected token `%s'\n", (*current)->next->av);
		return (0);
	}
	if (cmd->file_input)
		free(cmd->file_input);
	cmd->file_input = ft_strdup((*current)->next->av);
	if (!cmd->file_input)
	{
		free_cmd(first_cmd);
		return (0);
	}
	(*current) = (*current)->next->next;
	return (1);
}

int	handle_redir_out(t_token **current, t_command *cmd, t_command *first_cmd)
{
	if ((*current)->type != TOKEN_REDIR_OUT)
		return (1);
	if (!(*current)->next)
	{
		printf("minishell: syntax error near unexpected token `newline'\n");
		return (0);
	}
	if ((*current)->next->type != TOKEN_WORD)
	{
		printf("minishell: syntax error near unexpected token `%s'\n", (*current)->next->av);
		return (0);
	}
	if (cmd->file_output)
		free(cmd->file_output);
	cmd->file_output = ft_strdup((*current)->next->av);
	if (!cmd->file_output)
	{
		free_cmd(first_cmd);
		return (0);
	}
	cmd->append = 0;
	(*current) = (*current)->next->next;
	return (1);
}

int	handle_redir_append(t_token **current, t_command *cmd, t_command *first_cmd)
{
	if ((*current)->type != TOKEN_REDIR_APPEND)
		return (1);
	if (!(*current)->next)
	{
		printf("minishell: syntax error near unexpected token `newline'\n");
		return (0);
	}
	if ((*current)->next->type != TOKEN_WORD)
	{
		printf("minishell: syntax error near unexpected token `%s'\n", (*current)->next->av);
		return (0);
	}
	if (cmd->file_output)
		free(cmd->file_output);
	cmd->file_output = ft_strdup((*current)->next->av);
	if (!cmd->file_output)
	{
		free_cmd(first_cmd);
		return (0);
	}
	cmd->append = 1;
	(*current) = (*current)->next->next;
	return (1);
}

int	handle_heredoc(t_token **current, t_command *cmd, t_command *first_cmd)
{
	if ((*current)->type != TOKEN_HERDOC)
		return (1);
	if (!(*current)->next)
	{
		printf("minishell: syntax error near unexpected token `newline'\n");
		return (0);
	}
	if ((*current)->next->type != TOKEN_WORD)
	{
		printf("minishell: syntax error near unexpected token `%s'\n", (*current)->next->av);
		return (0);
	}
	if (cmd->herdoc)
		free(cmd->herdoc);
	cmd->herdoc = ft_strdup((*current)->next->av);
	if (!cmd->herdoc)
	{
		free_cmd(first_cmd);
		return (0);
	}
	(*current) = (*current)->next->next;
	return (1);
}

void	handle_word(t_token **current, t_command *cmd)
{
	append_arg(cmd, (*current)->av);
	(*current) = (*current)->next;
}

t_command *parsing_command(t_token *token)
{
    t_token *current = token;
    t_command *first_cmd;
    t_command *current_cmd = create_command();
    
    if (!current_cmd)
        return (NULL);
    first_cmd = current_cmd;
    
    while (current)
    {
        if (current->type == TOKEN_PIPE)
        {
            if (!handle_pipe(&current, &current_cmd, first_cmd))
                return (NULL);
        }
        else if (current->type == TOKEN_REDIR_IN)
        {
            if (!handle_redir_in(&current, current_cmd, first_cmd))
                return (NULL);
        }
        else if (current->type == TOKEN_REDIR_OUT)
        {
            if (!handle_redir_out(&current, current_cmd, first_cmd))
                return (NULL);
        }
        else if (current->type == TOKEN_REDIR_APPEND)
        {
            if (!handle_redir_append(&current, current_cmd, first_cmd))
                return (NULL);
        }
        else if (current->type == TOKEN_HERDOC)
        {
            if (!handle_heredoc(&current, current_cmd, first_cmd))
                return (NULL);
        }
        else if (current->type == TOKEN_WORD)
        {
			append_arg(current_cmd, current->av);
			current = current->next;
        }
	}
    	return (first_cmd);
}

