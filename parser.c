#include "include/minishell.h"
#include "my_libft/libft.h"
#include <stdlib.h>

#include <stdio.h>

void print_commands(t_command *cmd)
{
	int i;
	int cmd_num = 1;

	while (cmd)
	{
		printf("-------- Command %d --------\n", cmd_num++);
		
		// Print args
		if (cmd->args)
		{
			printf("Args: ");
			for (i = 0; cmd->args[i]; i++)
				printf("'%s' ", cmd->args[i]);
			printf("\n");
		}
		else
			printf("Args: (none)\n");

		// Input redirection
		if (cmd->file_input)
			printf("Input File: '%s'\n", cmd->file_input);
		
		// Output redirection
		if (cmd->file_output)
		{
			printf("Output File: '%s'\n", cmd->file_output);
			printf("Append: %s\n", cmd->append ? "Yes" : "No");
		}

		// Heredoc
		if (cmd->herdoc)
			printf("Heredoc: '%s'\n", cmd->herdoc);

		printf("\n");
		cmd = cmd->next;
	}
}

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

t_token_type	get_token_type(char *str)
{
	if ((ft_strncmp(str, "|", ft_strlen("|"))) == 0)
		return (TOKEN_PIPE);
	else if ((ft_strncmp(str, "<", ft_strlen("<"))) == 0)
			return (TOKEN_REDIR_IN);
	else if ((ft_strncmp(str, ">", ft_strlen(">"))) == 0)
			return (TOKEN_REDIR_OUT);
	else if ((ft_strncmp(str, ">>", ft_strlen(">>"))) == 0)
			return (TOKEN_REDIR_APPEND);
	else if ((ft_strncmp(str, "<<", ft_strlen("<<"))) == 0)
			return (TOKEN_HERDOC);
	else
		return (TOKEN_WORD);
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

t_token	*tokenize(char *line)
{
	int i;
	t_token	*token = NULL;
	bool	in_quot = false;
	char	quot_char = 0;
	int start = 0;

	i = 0;
	while (line[i])
	{
		handle_quote(&in_quot, &quot_char, &i, line);
		if (!in_quot && (line[i] == '|' || line[i] == '<' || line[i] == '>'))
		{
			handle_word_token(&token, start, line, i);

			i = handle_speciale_token(&token, line, i);
			start = i;
			continue;
		}
		else if (!in_quot && line[i] == ' ')
		{
			handle_word_token(&token, start, line,  i);
			while (line[i] == ' ')
				i++;
			start = i;
			continue;
		}
		i++;
	}
	handle_word_token(&token, start, line, i);
	return (token);
}


void	my_handler(int sig)
{
	if (sig == SIGINT)
	{
		write(1, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

// void print_token(t_token *token)
// {
//     while (token)
//     {
//         printf("Value: %-20s | Type: ", token->av);
//
//         switch (token->type)
//         {
//             case TOKEN_WORD:
//                 printf("TOKEN_WORD\n");
//                 break;
//             case TOKEN_PIPE:
//                 printf("TOKEN_PIPE\n");
//                 break;
// 			case TOKEN_REDIR_IN:
//                 printf("TOKEN_REDIRECT_IN\n");
//                 break;
//             case TOKEN_REDIR_OUT:
//                 printf("TOKEN_REDIRECT_OUT\n");
//                 break;
//             case TOKEN_REDIR_APPEND:
//                 printf("TOKEN_REDIRECT_APPEND\n");
//                 break;
//             case TOKEN_HERDOC:
//                 printf("TOKEN_HEREDOC\n");
//                 break;
//             default:
//                 printf("UNKNOWN\n");
//                 break;
//         }
//
//         token = token->next;
//     }
// }
//
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

int	is_space(char c)
{
	if (c == ' ')
		return (0);
	return (1);
}

void	free_double_array(char **arr)
{
	int	i;
	if (arr == NULL)
	{
		return;
	}
	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
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
			start = i + 1;
			i++;
			if ((ft_isalnum(str[i]) || str[i] == '_') && str[i])
				i++;
			string = ft_substr(str, start, i - start);
			if (!string)
				return (NULL);
			valeur = getenv(string);
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

void make_prompt()
{
    char *line;
    char *prompt;
	t_token	*token;
	t_command	*cmd;
    
    prompt = ft_strjoin(getenv("HOME"), "@minishell$ ");
    if (!prompt)
		return;
    while(1)
    {
		signal(SIGINT, my_handler);
        line = readline(prompt);
        
        if (!line)
        {
            printf("exit\n");
            break;
        }
        if (line[0] != '\0')
        {
            add_history(line);
			token = tokenize(line);
			cmd = parsing_command(token);
			print_commands(cmd);

			//print_token(token);
			free_token(&token);
        }
        free(line);
    }
    free(prompt);
    rl_clear_history();
}

int main()
{
    make_prompt();
}
