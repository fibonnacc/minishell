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
#include <stdio.h>


#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void execute_command(char **args)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        // Child process
        execvp(args[0], args); // try execvp first for PATH support
        perror("execvp failed");
        exit(1);
    }
    else if (pid > 0)
    {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
    }
    else
    {
        perror("fork failed");
    }
}

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

		if (cmd->file_input)
			printf("Input File: '%s'\n", cmd->file_input);
		
		if (cmd->file_output)
		{
			printf("Output File: '%s'\n", cmd->file_output);
			printf("Append: %s\n", cmd->append ? "Yes" : "No");
		}

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

void print_token(t_token *token)
{
	while (token)
	{
		printf("the value : %s -> ", token->av);
		if (token->type == TOKEN_HERDOC)
			printf ("TOKEN_HERDOC\n");
		else if (token->type == TOKEN_REDIR_APPEND)
			printf ("TOKEN_REDIR_APPEND\n");
		else if (token->type == TOKEN_REDIR_IN)
			printf ("TOKEN_REDIR_IN\n");
		else if (token->type == TOKEN_REDIR_OUT)
			printf ("TOKEN_REDIR_OUT\n");
		else if (token->type == TOKEN_PIPE)
			printf ("TOKEN_PIPE\n");
		else
			printf ("TOKEN_WORD\n");
		token = token->next;
	}
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
			printf("---------------------------\n");
			print_token(token);
			if (cmd->args)
				execute_command(cmd->args);
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
