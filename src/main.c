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

int  make_exit(t_command *cmd, t_data **data)
{
  if (ft_strncmp(cmd->args[0], "exit", 4) == 0)
  {
    if (cmd->args[1] != NULL)
    {
      int status = ft_atoi(cmd->args[1]);
      (*data)->exit = WEXITSTATUS(status);
      return(1);
    }
    else
    {
      (*data)->exit = 0;
      return(1);
    }
  }
  return(0);
}

void  check_exit_status(t_command *cmd, t_data **data)
{
  int i = 0;
  while (cmd->args[i])
  {
    if (ft_strchr(cmd->args[i],'$') && ft_strchr(cmd->args[i],'?'))
    {
      char  *expand = ft_itoa((*data)->exit);
      char  *buffer = malloc((ft_strlen(expand) + ft_strlen(cmd->args[i]) + 1));

      int k = 0;
      int j = 0;
      while(cmd->args[i][k] && cmd->args[i][k] != '$')
      {
        buffer[j++] = cmd->args[i][k++];
      }
      if (cmd->args[i][k] == '$' && cmd->args[i][k + 1] == '?')
      {
        int f = 0;
        while (expand[f])
          buffer[j++] = expand[f++];
        k = j;
        k++;
      }
      if (cmd->args[i][k])
      {
        while (cmd->args[i][k])
          buffer[j++] = cmd->args[i][k++];
      }
      buffer[j] = 0;
      free(cmd->args[i]);
      cmd->args[i] = buffer;
      return;
    }
    i++;
  }
}

int append_or_trunc(t_command **cmd)
{
  if ((*cmd)->append)
  {
    return(O_APPEND);
  }
  else
    return(O_TRUNC);
}

void  excute_redirection_of_child(t_command **cmd, t_data **data, int *fd_out, int *fd_in)
{
  int flags;
  int i;

  flags = 0;
  i = 0;
  if ((*cmd)->file_output)
  {
    flags = O_WRONLY | O_CREAT | append_or_trunc(cmd);
    *fd_out = open((*cmd)->file_output, flags, 0644);
    if (*fd_out < 0)
    {
      perror("open output file");
      (*data)->exit = 1;
      exit(1);
    }
    dup2(*fd_out, STDOUT_FILENO);
    close(*fd_out);
  }
  if ((*cmd)->file_input)
  {
    while (i < (*data)->count_red_in)
    {
      *fd_in = open((*cmd)->file_input[i], O_RDONLY);
      if (*fd_in < 0)
      {
        printf("minishell: %s: No such file or directory\n", (*cmd)->file_input[i]);
        exit(1);
      }
      dup2(*fd_in, STDIN_FILENO);
      close(*fd_in);
      i++;
    }
  }
}

void  excute_redirection_of_parent(t_command **cmd, t_data **data, int *fd_out)
{
  int (saved_stdout), saved_stdin, flags;

  saved_stdout = dup(STDOUT_FILENO);
  saved_stdin = dup(STDIN_FILENO);
  if ((*cmd)->file_output)
  {
    flags = O_WRONLY | O_CREAT | append_or_trunc(cmd);
    *fd_out = open((*cmd)->file_output, flags, 0644);
    if (*fd_out < 0)
      return;
    if (dup2(*fd_out, STDOUT_FILENO) == -1)
    {
      close(*fd_out);
      return;
    }
    close(*fd_out);
  }
  if (make_exit(*cmd, data))
    exit((*data)->exit);
  my_echo(*cmd, data);  // You can replace this with your built-in dispatcher

  // 🔁 Restore original stdout/stdin
  dup2(saved_stdout, STDOUT_FILENO);
  dup2(saved_stdin, STDIN_FILENO);
  close(saved_stdout);
  close(saved_stdin);
}


void execute_command(t_command *cmd, char **env, t_data **data)
{
  pid_t pid;
  int save = dup(0);
  char *command;
  int fd_out = -1;
  int fd_in = -1;
  
  check_exit_status(cmd, data);
  if (cmd->herdoc_file)
  {
    fd_in = open(cmd->herdoc_file, O_RDONLY);
    if (fd_in < 0)
    {
      printf("minishell: No such file or directory\n");
      exit(1);
    }
    dup2(fd_in, STDIN_FILENO);
    close(fd_in);
  }
  if (built_in(cmd->args[0]))
  {
    excute_redirection_of_parent(&cmd, data, &fd_out);
    return;
  }
  pid = fork();
  if (pid == 0)
  {
    signal(SIGQUIT, SIG_DFL);
    excute_redirection_of_child(&cmd, data, &fd_out, &fd_in);
    command = get_command(cmd->args[0], env);
    if (!command)
    {
      printf("minishell: %s: command not found\n", cmd->args[0]);
      (*data)->exit = 127;
      exit((*data)->exit);
    }
    execve(command, cmd->args, env);
    perror("execve failed");
    (*data)->exit = 1;
    exit(1);
  }
  else if (pid > 0)
  {
    dup2(save, 0);
    close(save);
    int status;
    g_value = pid;
    waitpid(pid, &status, 0);
    if (WIFSIGNALED(status))
    {
      int sig = WTERMSIG(status);
      if (sig == SIGQUIT)
        write(2, "Quit (core dumped)\n", 19);
      (*data)->exit = 128 + sig;
    }
    else
      (*data)->exit = WEXITSTATUS(status);
    g_value = 0;
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

t_token	*tokenize(char *line, t_data **data)
{
	t_token	*token = NULL;
	int start, i;

	i = 0;
  start = 0;
	if (!check_somthing(line, data))
		return (NULL);
	while (line[i])
	{
		if (line[i] == '$' && (ft_isalnum(line[i + 1]) || line[i + 1] == '_'))
		{
			handle_dollar(&token, line, &i, &start, data);
			continue;
		}
		if (line[i] == '$' && !ft_isalnum(line[i + 1]))
    {
			handle_some_cases(&token, line, &i, &start, data);
    }
		if (line[i] == '|' || line[i] == '<' || line[i] == '>')
		{
			handle_word_token(&token, start, line, &i, data);
			i = handle_speciale_token(&token, line, i, data);
			start = i;
		}
		else if (line[i] == '\"' || line[i] == '\'')
			handle_special_quot(&token, line, &i, &start, data);
		else if (line[i] == ' ' || line[i] == '\t')
			handle_white_spaces(&token, line, &i, &start, data);
    else
		  i++;
	}
  handle_word_token(&token, start, line, &i, data);
	return (token);
}

void my_handler(int sig)
{
	if (sig == SIGINT)
	{
		if (g_value > 0) // child is running
		{
      g_value = 0;   // send SIGINT to child
      printf("\n");
		}
		else
		{
      // mark Ctrl-C pressed at prompt
			write(1, "\n", 1);
			rl_on_new_line();
			rl_replace_line("", 0);
			rl_redisplay();
		}
	}
}

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

bool  logic_of_meta(t_token *cmd, t_data **data)
{
  t_token *cur = cmd;
  while (cur && cur->next)
  {
    if (cur->type != TOKEN_PIPE && cur->type != TOKEN_WORD 
      && cur->next->type != TOKEN_PIPE && cur->next->type != TOKEN_WORD )
    {
      printf("minishell: syntax error  near unexpected token %s\n", cur->next->av);
      (*data)->exit = 2;
      return (false);
    }
    else
      cur = cur->next;
  }
  (*data)->exit = 0;
  return(true);
}

void make_prompt(char **env)
{
	char *line;
	t_token *token;
  t_data  *data = malloc(sizeof(t_data));
  ft_memset(data, 0, sizeof(t_data));
	t_command *cmd;

	signal(SIGINT, my_handler);
	signal(SIGQUIT, SIG_IGN);
	while (1)
	{
		// if (g_value == -1)
		// {
		// 	g_value = 0;
		//     continue;
		// }
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
			token = tokenize(line, &data);
      if (!token)
      {
        free(line);
        continue;
      }
			// print_token(token);
			//continue_parsing(&token);
			// printf ("after removing------------------------------------------\n");
			// print_token(token);
			join_nodes(&token);
      if (logic_of_meta(token, &data) == false)
      {
        free(line);
        continue;
      }
			//  printf (" after joining------------------------------------------\n");
			// print_token(token);
			cmd = parsing_command(token, &data);
			if (!cmd)
			{
				free(line);
      //printf("{%s}\n", str);
				continue;
			}
      if (data->count_herdoc > 0)
      {
        excute_herdoc_for_child(&cmd, &data);
        
      }
			//print_commands(cmd);
			if (cmd->args)
				execute_command(cmd, env, &data);
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
