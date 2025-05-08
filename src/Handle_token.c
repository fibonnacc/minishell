/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handle_token.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouryal <ybouryal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 09:47:38 by ybouryal          #+#    #+#             */
/*   Updated: 2025/05/07 09:49:10 by ybouryal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

t_token *creat_token(char *line, t_token_type type)
{
	t_token *new_token;

	if (!(new_token = (t_token *)ft_calloc(1, sizeof(t_token))))
		return (NULL);
	if (!(new_token->av = ft_strdup(line)))
		return (free(new_token), NULL);
	new_token->type = type;
	new_token->next = NULL;
	return (new_token);
}

void	handle_quote(bool	*in_quot ,char *quot_char, int *i, char *line)
{
	if ((line[*i] == '"' || line[*i] == '\''))
	{
		if (*quot_char == '\0')
		{
			*in_quot = true;
			*quot_char = line[*i];
		}
		else if (*quot_char == line[*i])
		{
			*in_quot = false;
			*quot_char = 0;
		}
	}
}

void	add_token(t_token **token, t_token *new_token)
{
	t_token *current;

	if (!*token)
	{
		*token = new_token;
		return ;
	}
	current = *token;
	while (current->next)
		current = current->next;
	current->next = new_token;
}

void	handle_word_token(t_token **token, int start, char *line, int i)
{
	char *word;

	if (i > start)
	{
		word = ft_substr(line, start, i - start);
		if (word && *word != '\0')
		{
			add_token(token, creat_token(word, get_token_type(word)));
			free(word);
		}
	}
}

int	handle_speciale_token(t_token **token, char *line, int i)
{
	char	special[3];

	if ((line[i] == '>' || line[i] == '<') && (line[i + 1] == line[i]))
	{
		special[0] = line[i];
		special[1] = line[i];
		special[2] = '\0';
		add_token(token, creat_token(special, get_token_type(special)));
		return (i + 2);
	}
	else
	{
		special[0] = line[i];
		special[1] = '\0';
		add_token(token, creat_token(special, get_token_type(special)));
		return (i + 1);
	}
}

