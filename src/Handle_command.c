/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handle_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:26:30 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/05 15:37:15 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

t_command	*create_command(void)
{
	t_command	*cmd;

	cmd = (t_command *)gc_calloc(1, sizeof(t_command));
	if (cmd == NULL)
	{
		return (NULL);
	}
	cmd->args = NULL;
	cmd->file_input = NULL;
	cmd->file_output = NULL;
	cmd->herdoc = NULL;
	cmd->herdoc_file = NULL;
	cmd->file = false;
	cmd->redir_error = false;
	cmd->next = NULL;
	return (cmd);
}

char	*expand_env(char *str, char **env)
{
	t_var	var;

	if (!init_var(str, &var))
		return (NULL);
	while (str[var.i])
	{
		if (var.condition && str[var.i] == '$' && str[var.i + 1] == '?')
		{
			remplace_var(&var);
			continue ;
		}
		if (var.condition && str[var.i] == '$' && str[var.i + 1]
			&& (ft_isalnum(str[var.i + 1]) || str[var.i + 1] == '_'))
		{
			var.string = split_var(&var.i, str, &var.start);
			if (!var.string)
				return (NULL);
			var.valeur = get_env(var.string, env);
			if (var.valeur)
				make_the_envirement(&var);
			continue ;
		}
		var.result[var.j++] = str[var.i++];
	}
	return (var.result);
}

t_command	*parsing_command(t_token *token, t_data **data)
{
	t_token		*current;
	t_command	*first_cmd;
	t_command	*current_cmd;
	int			i;

	if (reset_value(&current_cmd, &current, data, token) == 0)
		return (NULL);
	first_cmd = current_cmd;
	i = 0;
	while (current)
	{
		if (current->type == TOKEN_PIPE)
		{
			i = 0;
			(*data)->count_red_in = 0;
			if (!handle_pipe(&current, &current_cmd, first_cmd, data))
				return (NULL);
		}
		else if ((current)->type == TOKEN_REDIR_IN)
		{
			if (!handle_redir_in(&current, current_cmd, data))
				return (0);
		}
		else if ((current)->type == TOKEN_REDIR_OUT)
		{
			if (!handle_redir_out(&current, current_cmd))
				return (0);
		}
		else if ((current)->type == TOKEN_REDIR_APPEND)
		{
			if (!handle_redir_append(&current, current_cmd))
				return (0);
		}
		else if (current->type == TOKEN_HERDOC)
		{
			if (!handle_heredoc(&current, current_cmd, &i))
				return (0);
		}
		else if (current->type == TOKEN_WORD)
		{
			append_arg(current_cmd, current->av, data);
			current = current->next;
		}
	}
	return (first_cmd);
}
