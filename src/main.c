/* ************************************************************************** */
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int	g_value = 0;

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
	if (ft_strncmp(cmd, "echo", 5) == 0)
		return (true);
	else
	{
		return (false);
	}
}

void	free_2D_array(char **str)
{
	if (!str || !str[0])
		return;
	int i = 0;
	while (str[i])
	{
		free(str[i]);
		i++;
	}
}

char *get_command(char *cmd, char **env)
{
	char *path_env;
	char **split_env;
	char *complete_path;
	char *first_join;

	int i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], "PATH=", 5) == 0)
		{
			path_env = env[i] + 5;
			break;
		}
		i++;
	}
	split_env = ft_split(path_env, ':');
	first_join = ft_strjoin("/", cmd);
	i = 0;
	while(split_env[i])
	{
		complete_path = ft_strjoin(split_env[i], first_join);
		if (access(complete_path, X_OK) == 0)
		{
			free_2D_array(split_env);
			free(first_join);
			return (complete_path);
		}
		if (complete_path)
			free(complete_path);
		i++;
	}
	free_2D_array(split_env);
	return (NULL);
}

void execute_command(t_command *cmd, char **env)
{
    pid_t pid;
	if (built_in(cmd->args[0]))
	{
		my_echo(cmd->args);
		return;
	}

	pid = fork();
	if (pid == 0)
    {
		char *command = get_command(cmd->args[0], env);
		if (!command)
		{
			printf("minishell: %s: command not found\n", cmd->args[0]);
			exit(127);
		}
        execve(command, cmd->args, env);
        perror("execvp failed");
        exit(1);
    }
	else if (pid > 0)
    {
        int status;
        g_value = pid; // use g_value to store child PID
        waitpid(pid, &status, 0);
        g_value = 0; // reset after child exits
    }
    else
    {
        perror("fork failed");
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

void	handle_dollar(t_token **token, char *line, int *i, int *start)
{

	if (*i > *start)
	{
		handle_word_token(token, *start, line, i);
	}
	*start = *i;
	(*i)++;
	while ((ft_isalnum(line[*i]) || line[*i] == '_') && line[*i])
		(*i)++;
	if (*i > *start)
	{
		handle_word_token(token, *start, line, i);
	}
	*start = *i;
}

void	handle_special_quot(t_token **token, char *line, int *i, int *start)
{
	if (line[*start] == '$' && !ft_isalnum(line[*i]))
		*start = *i;
	if (*i > *start)
		handle_word_token(token, *start, line, i);
	*start = *i;
	char q = line[*i];
	(*i)++;
	while (line[*i] != q)
		(*i)++;
	if (line[*i] == q)
		(*i)++;
	handle_word_token(token, *start, line, i);
	*start = *i;
}

void	handle_white_spaces(t_token **token, char *line, int *i, int *start)
{
	handle_word_token(token, *start, line,  i);
	while (line[*i] == ' ' || line[*i] == '\t')
		(*i)++;
	*start = *i;
}

bool  check_somthing(char *word)
{
	if (!is_closed_quotes(word))
	{
		printf ("the quote does not close!\n");
		return(false);
	}
	return (true);
}

t_token	*tokenize(char *line)
{
	int i;
	t_token	*token = NULL;
	bool	in_quot = false;
	char	quot_char = 0;
	int start = 0;

	i = 0;
	if (!check_somthing(line))
		return (NULL);
	while (line[i])
	{
		handle_quote(&in_quot, &quot_char, &i, line);
		if (line[i] == '$' && (ft_isalnum(line[i + 1]) || line[i + 1] == '_'))
		{
			handle_dollar(&token, line, &i, &start);
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
			handle_special_quot(&token, line, &i, &start);
		}
		else if (!in_quot && (line[i] == ' ' || line[i] == '\t'))
		{
			handle_white_spaces(&token, line, &i, &start);
			continue;
		}
		else
		i++;
	}
	handle_word_token(&token, start, line, &i);
	return (token);
}

void my_handler(int sig)
{
	if (sig == SIGINT)
	{
		if (g_value > 0) // child is running
		{
			kill(g_value, SIGINT); // send SIGINT to child
		}
		else
		{
			g_value = -1; // mark Ctrl-C pressed at prompt
			write(1, "\n", 1);
			rl_on_new_line();
			rl_replace_line("", 0);
			rl_redisplay();
		}
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

void join_nodes(t_token **token)
{
	char *joined;
	t_token *curr;
	t_token *next;

	curr = *token;
	//printf("-----------------------\n");
    //print_token(curr);
	while (curr && curr->next)
	{
		//printf("%d->%s\n", curr->info, curr->av);
		if (curr->info)
		{
			next = curr->next;

			joined = ft_strjoin(curr->av, next->av);
			free(curr->av);
			curr->av = joined;
			curr->next = next->next;
			if (!next->info)
				curr->info = false;
			if (next->av)
				free(next->av);
			free(next);
			continue;
		}
		curr = curr->next;
	}
	// printf("%d->%s\n", curr->info, curr->av);
}

void make_prompt(char **env)
{
	char *line;
	t_token *token;
	t_command *cmd;

	signal(SIGINT, my_handler);
	while (1)
	{
		if (g_value == -1)
		{
			g_value = 0;
			continue;
		}
		line = readline(promt());
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
			// printf ("after removing------------------------------------------\n");
			// print_token(token);
			join_nodes(&token);
			// printf (" after joining------------------------------------------\n");
			// print_token(token);
			cmd = parsing_command(token);
			if (!cmd)
			{
				free(line);
				continue;
			}
			//print_commands(cmd);
			if (cmd->args)
				execute_command(cmd, env);
			free_token(&token);
		}
		free(line);
	}
	rl_clear_history();
}

int main(int ac, char **av, char **env)
{
	(void)ac;
	(void)av;
	make_prompt(env);
}
