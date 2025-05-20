/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:26:05 by helfatih          #+#    #+#             */
/*   Updated: 2025/05/16 17:26:13 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include <stdbool.h>
#include <stdlib.h>

void	my_echo(char **args)
{
	int i;
	bool	j;

	j = false;
	i = 1;
	if (args[i] && ft_strncmp(args[1], "-n", ft_strlen("-n")) == 0)
	{
		j = true;
		i++;
	}
	while(args[i])
	{
		ft_putstr_fd(args[i], 1);
		if (args[i + 1])
			ft_putstr_fd(" ", 1);
		i++;
	}
	if (j == false)
		printf("\n");
}

bool	built_in(char *cmd)
{
	return (ft_strncmp(cmd, "echo", ft_strlen("echo")) == 0);
}

void execute_command(t_command	*cmd)
{
	int i;
    pid_t pid;

	i = 0;
	if (built_in(cmd->args[0]))
	{
		my_echo(cmd->args);
		return;
	}
	pid = fork();
	if (pid == 0)
    {
        execvp(cmd->args[0], cmd->args);
        perror("execvp failed");
        exit(1);
    }
	else if (pid > 0)
    {
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
		if (in_quot && line[i] == '$' && (ft_isalnum(line[i + 1]) || line[i + 1] == '_'))
		{
			if (i > start)
				handle_word_token(&token, start, line, &i);
			start = i;
			i++;
			while ((ft_isalnum(line[i]) || line[i] == '_') && line[i])
				i++;
			handle_word_token(&token, start, line, &i);
			start = i;
			continue;
		}
		if (!in_quot && (line[i] == '|' || line[i] == '<' || line[i] == '>'))
		{
			handle_word_token(&token, start, line, &i);
			i = handle_speciale_token(&token, line, i);
			start = i;
		}
		else if (line[i] == '\"' || line[i] == '\'')
		{
			start = i;
			char q = line[i];
			i++;
			while (line[i] != q)
				i++;
			i++;
			handle_word_token(&token, start, line, &i);
			start = i;
		}
		else if (!in_quot && (line[i] == ' ' || line[i] == '\t'))
		{
			handle_word_token(&token, start, line,  &i);
			while (line[i] == ' ' || line[i] == '\t')
				i++;
			start = i;
			continue;
		}
		else
		{
			i++;
		}
	}
	handle_word_token(&token, start, line, &i);
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

void	continue_parsing(t_token **token)
{
	t_token	*current;

	current = *token;
	while (current)
	{
		if (current->type == TOKEN_WORD)
		{
			current->av = remove_quotes((current->av));
		}
		current = current->next;
	}
}

void make_prompt()
{
    char *line;
    char *prompt;
    char *part1;
    char *part2;
    t_token *token;
    t_command *cmd;

    part1 = ft_strjoin(COLOR_START, getenv("HOME"));
    part2 = ft_strjoin(part1, "@minishell> ");
    free(part1);
    prompt = ft_strjoin(part2, COLOR_RESET); 
	free(part2);

    if (!prompt)
        return;

    while (1)
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
			//print_token(token);
			continue_parsing(&token);
			//printf ("after removing\n");
			//print_token(token);
            cmd = parsing_command(token);
            if (cmd->args)
                 execute_command(cmd);
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
