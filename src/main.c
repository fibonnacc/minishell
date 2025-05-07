/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouryal <ybouryal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 09:54:27 by ybouryal          #+#    #+#             */
/*   Updated: 2025/05/07 09:54:28 by ybouryal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

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
