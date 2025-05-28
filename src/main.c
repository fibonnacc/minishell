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

void	handle_dollar(t_token **token, char *line, int *i, int *start, int *j)
{

	if (*i > *start)
  {
		handle_word_token(token, *start, line, i, j);
  }
	*start = *i;
	(*i)++;
	while ((ft_isalnum(line[*i]) || line[*i] == '_') && line[*i])
		(*i)++;
  if (*i > *start)
  {
	  handle_word_token(token, *start, line, i, j);
  }
	*start = *i;
}

void	handle_special_quot(t_token **token, char *line, int *i, int *start, int *j)
{
  if (line[*start] == '$' && !ft_isalnum(line[*i]))
    *start = *i;
  if (*i > *start)
    handle_word_token(token, *start, line, i, j);
	*start = *i;
	char q = line[*i];
	(*i)++;
	while (line[*i] != q)
		(*i)++;
  if (line[*i] == q)
    (*i)++;
  handle_word_token(token, *start, line, i, j);
	*start = *i;
}

void	handle_white_spaces(t_token **token, char *line, int *i, int *start, int *j)
{
	handle_word_token(token, *start, line,  i, j);
	while (line[*i] == ' ' || line[*i] == '\t')
		(*i)++;
	*start = *i;

}

bool  check_somthing(char *word)
{
  if (!special_character(word))
  {
    printf("this character is not valid\n");
    return(false);
  }
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
  int j = 0;
  if (!check_somthing(line))
    return (NULL);
	while (line[i])
	{
		handle_quote(&in_quot, &quot_char, &i, line);
		if (line[i] == '$' && (ft_isalnum(line[i + 1]) || line[i + 1] == '_'))
		{
			handle_dollar(&token, line, &i, &start, &j);
			continue;
		}
		if (!in_quot && (line[i] == '|' || line[i] == '<' || line[i] == '>'))
		{
			handle_word_token(&token, start, line, &i, &j);
			i = handle_speciale_token(&token, line, i, &j);
			start = i;
		}
		else if (line[i] == '\"' || line[i] == '\'')
		{
			handle_special_quot(&token, line, &i, &start, &j);
		}
		else if (!in_quot && (line[i] == ' ' || line[i] == '\t'))
		{
			handle_white_spaces(&token, line, &i, &start,  &j);
			continue;
		}
		else
			i++;
	}
	handle_word_token(&token, start, line, &i, &j);
  j++;
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
  int i = 0;
	while (token)
	{
		printf("the value : {%s}", token->av);
		if (token->type == TOKEN_HERDOC)
    {
      if (token->info[i].should_join == 1)
        printf(" :   {True} : ");
      else
        printf(" :   {false} : ");
			printf ("TOKEN_HERDOC\n");
    }
		else if (token->type == TOKEN_REDIR_APPEND)
    {
      if (token->info[i].should_join == 1)
        printf(" : {True} : ");
      else
        printf(" : {false} : ");
			printf ("TOKEN_REDIR_APPEND\n");
    }
		else if (token->type == TOKEN_REDIR_IN)
    {
      if (token->info[i].should_join == 1)
        printf(" : {True} : ");
      else
        printf(" : {false} : ");
			printf ("TOKEN_REDIR_IN\n");
    }
		else if (token->type == TOKEN_REDIR_OUT)
    {
      if (token->info[i].should_join == 1)
        printf(" : {True} : ");
      else
        printf(" : {false} : ");
			printf ("TOKEN_REDIR_OUT\n");
    }
		else if (token->type == TOKEN_PIPE)
    {
      if (token->info[i].should_join == 1)
        printf(" : {True} : ");
      else
        printf(" : {false} : ");
			printf ("TOKEN_PIPE\n");
    }
		else
    {
      if (token->info[i].should_join == 1)
        printf(" : {True} : ");
      else
        printf(" : {false} : ");
			printf ("TOKEN_WORD\n");
    }
    i++;
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

void join_nodes(t_token **token)
{
  char *joined;
  t_token *curr;
  t_token *next;
  int i;

  curr = *token;
  i = 0;
	while (curr && curr->next)
	{
		if (curr->info[i].should_join == true)
		{
			next = curr->next;

			joined = ft_strjoin(curr->av, next->av);
			free(curr->av);
			curr->av = joined;
			curr->next = next->next;
			if (next->av)
				free(next->av);
			if (next->info)
				free(next->info);
			free(next);
			continue;
		}
		i++;
		curr = curr->next;
	}
}

void make_prompt()
{
  char *line;
  t_token *token;
  t_command *cmd;

  while (1)
  {
    signal(SIGINT, my_handler);
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
      if (!token)
        return;
      print_token(token);
      continue_parsing(&token);
      join_nodes(&token);
      printf ("------------------------------------------\n");
      print_token(token);
      cmd = parsing_command(token);
      if (!cmd)
        return;
      if (cmd->args)
        execute_command(cmd);
      free_token(&token);
    }
    free(line);
  }
  rl_clear_history();
}

int main()
{
    make_prompt();
}
