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

int	g_value = 0;

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
		if (line[i] == '$' && !ft_isalnum(line[i + 1]))
			handle_some_cases(&token, line, &i, &start);
		if (!in_quot && (line[i] == '|' || line[i] == '<' || line[i] == '>'))
		{
			handle_word_token(&token, start, line, &i);
			i = handle_speciale_token(&token, line, i);
			start = i;
		}
		else if (line[i] == '\"' || line[i] == '\'')
			handle_special_quot(&token, line, &i, &start);
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

bool  logic_of_meta(t_token *cmd)
{
  t_token *cur = cmd;
  while (cur && cur->next)
  {
    if (cur->type != TOKEN_PIPE && cur->type != TOKEN_WORD 
      && cur->next->type != TOKEN_PIPE && cur->next->type != TOKEN_WORD )
    {
      printf("minishell: syntax error  near unexpected token %s\n", cur->next->av);
      return (false);
    }
    else
      cur = cur->next;
  }
  return(true);
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
      if (!logic_of_meta(token))
      {
        free(line);
        continue;
      }
		  //printf (" after joining------------------------------------------\n");
			//print_token(token);
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
