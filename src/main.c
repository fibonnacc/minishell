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

void  store_and_increment(char *str, int *i, char *buffer, int *j)
{
  if (str[*i] == ' ' || str[*i] == '\t')
  {
    while (str[*i] == ' ' || str[*i] == '\t')
      (*i)++;
  }
  else
    buffer[(*j)++] = str[(*i)++];
}

int  make_exit(char *str)
{
  char *buffer;
  int i, j;

  buffer = malloc(ft_strlen(str) + 1);
  if (!buffer)
    return (0);
  i = 0;
  j = 0;
  while (str[i])
  {
    store_and_increment(str, &i, buffer, &j);
  }
  buffer[j] = '\0';
  if (ft_strncmp(buffer, "exit", 4) == 0)
    return(0);
  return(1);
}

void execute_command(t_command *cmd, char **env)
{
    pid_t pid;
    char *command;
    int fd_out = -1;
    int fd_in = -1;
    int exit_code = 0;

    if (built_in(cmd->args[0]))
    {
        int saved_stdout = dup(STDOUT_FILENO);
        int saved_stdin = dup(STDIN_FILENO);
        if (cmd->file_output)
        {
            int flags = O_WRONLY | O_CREAT | (cmd->append ? O_APPEND : O_TRUNC);
            fd_out = open(cmd->file_output, flags, 0644);
            if (fd_out < 0)
            {
                perror("open output file");
                return;
            }
            if (dup2(fd_out, STDOUT_FILENO) == -1)
            {
                perror("dup2");
                close(fd_out);
                return;
            }
            close(fd_out);
        }
        
        my_echo(cmd->args);  // You can replace this with your built-in dispatcher

        // 🔁 Restore original stdout/stdin
        dup2(saved_stdout, STDOUT_FILENO);
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdout);
        close(saved_stdin);
        return;
    }
    pid = fork();
    if (pid == 0)
    {
        if (cmd->file_output)
        {
            int flags = O_WRONLY | O_CREAT | (cmd->append ? O_APPEND : O_TRUNC);
            fd_out = open(cmd->file_output, flags, 0644);
            if (fd_out < 0)
            {
                perror("open output file");
                exit(1);
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }

        if (cmd->file_input)
        {
            fd_in = open(cmd->file_input, O_RDONLY);
            if (fd_in < 0)
            {
                perror("open input file");
                exit(1);
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }
        command = get_command(cmd->args[0], env);
        if (!command)
        {
            fprintf(stderr, "minishell: %s: command not found\n", cmd->args[0]);
            exit(127);
        }
        execve(command, cmd->args, env);
        perror("execve failed");
        exit(1);
    }
    else if (pid > 0)
    {
        int status;
        g_value = pid;
        waitpid(pid, &status, 0);
        exit_code = WEXITSTATUS(status);
        g_value = exit_code;
    }
    else
        perror("fork failed");
}

t_token_type get_token_type(char *str)
{
	if (ft_strncmp(str, ">>", 2) == 0)
		return (TOKEN_REDIR_APPEND);
	else if (ft_strncmp(str, "<<", 2) == 0)
		return (TOKEN_HERDOC);
	else if (ft_strncmp(str, ">", 1) == 0)
		return (TOKEN_REDIR_OUT);
	else if (ft_strncmp(str, "<", 1) == 0)
		return (TOKEN_REDIR_IN);
	else if (ft_strncmp(str, "|", 1) == 0)
		return (TOKEN_PIPE);
	else
		return (TOKEN_WORD);
}

t_token	*tokenize(char *line, int *exit)
{
	t_token	*token = NULL;
	int start, i;

	i = 0;
  start = 0;
	if (!check_somthing(line))
		return (NULL);
	while (line[i])
	{
		if (line[i] == '$' && (ft_isalnum(line[i + 1]) || line[i + 1] == '_'))
		{
			handle_dollar(&token, line, &i, &start, exit);
			continue;
		}
		if (line[i] == '$' && !ft_isalnum(line[i + 1]))
    {
			handle_some_cases(&token, line, &i, &start, exit);
    }
		if (line[i] == '|' || line[i] == '<' || line[i] == '>')
		{
			handle_word_token(&token, start, line, &i, exit);
			i = handle_speciale_token(&token, line, i);
			start = i;
		}
		else if (line[i] == '\"' || line[i] == '\'')
			handle_special_quot(&token, line, &i, &start, exit);
		else if (line[i] == ' ' || line[i] == '\t')
			handle_white_spaces(&token, line, &i, &start, exit);
    else
		  i++;
	}
  handle_word_token(&token, start, line, &i, exit);
	return (token);
}

void my_handler(int sig)
{
	if (sig == SIGINT)
	{
		if (g_value > 0) // child is running
		{
			kill(g_value, SIGINT); // send SIGINT to child
      printf("\n");
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

// void	continue_parsing(t_token **token)
// {
// 	t_token	*current;
//
// 	current = *token;
// 	while (current)
// 	{
// 		if (current->type == TOKEN_WORD)
// 		{
// 			current->av = remove_quotes((current->av));
// 		}
// 		current = current->next;
// 	}
// }

void join_nodes(t_token **token)
{
	char *joined;
	t_token (*curr), *next;

	curr = *token;
	while (curr && curr->next)
	{
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
    if (line[0] == '\0')
      continue;
		if (line[0] != '\0')
		{
			add_history(line);
			token = tokenize(line, &g_value);
      if (!token)
      {
        free(line);
        continue;
      }
      // if (!make_exit(token->av))
      // {
      //   int exit = check_the_next(token->next->av);
      // }
			//print_token(token);
			//continue_parsing(&token);
			// printf ("after removing------------------------------------------\n");
			// print_token(token);
			join_nodes(&token);
      if (logic_of_meta(token) == false)
      {
        free(line);
        continue;
      }
			//  printf (" after joining------------------------------------------\n");
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
    free_cmd(cmd);
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
