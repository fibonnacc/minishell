/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:26:13 by helfatih          #+#    #+#             */
/*   Updated: 2025/07/18 13:10:53 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"


int	is_number(char *str)
{
	int	i;

	i = 0;
	if (str[i] == '\0')
		return (0);
	// if ((str[i] == '-' || str[i] == '+') && str[i + 1] == '\0')
	// 	return (0);
	if (str[i] == '-' || str[i] == '+')
		i++;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

int validation(t_command *cmd)
{
  if (cmd->args[1] && !is_number(cmd->args[1]))
  {
    printf("minishell: exit: %s: numeric argument required\n", cmd->args[1]);
    return(2);
  }
  if (cmd->args[2])
  {
    if (!is_number(cmd->args[2]))
    {
      printf("minishell: exit: too many arguments\n");
      return(2);
    }
    else
    {
      printf("minishell: exit: too many arguments\n");
      return(1);
    }
  }
  return(0);
}

int  make_exit(t_command *cmd)
{
  int status;
  int i;

  status = 0;
  i = 0;
  if (cmd->args[1] != NULL)
  {
    i = validation(cmd);
    if (i != 0)
      return(i);
    status = ft_atoi(cmd->args[1]) % 256;
    if (status < 0)
      status += 256;
    return(status);
  }
  return(0);
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

int is_directory(t_command **cmd, t_data **data)
{
  DIR *folder;
  int i = 0;

  while (i < (*data)->count_red_out)
  {
    folder = opendir((*cmd)->file_output[i]);
    if (folder != NULL)
    {
      printf("minishell : %s:  Is a directory\n", (*cmd)->file_output[i]);
      set_status(1);
      closedir(folder);
      return(1);
    }
    closedir(folder);
    i++;
  }
  return(0);
}

void  excute_redirection_of_child(t_command **cmd, t_data **data, int *fd_out, int *fd_in)
{
  int flags;
  int i;

  flags = 0;
  i = 0;
  if ((*cmd)->file_input)
  {
    while (i < (*data)->count_red_in)
    {
      *fd_in = open((*cmd)->file_input[i], O_RDONLY);
      if (*fd_in < 0)
      {
        printf("minishell: %s: No such file or directory\n", (*cmd)->file_input[i]);
        set_status(1);
        exit(1);
      }
      dup2(*fd_in, STDIN_FILENO);
      close(*fd_in);
      i++;
    }
  }
  if ((*cmd)->file_output)
  {
    int i;
    if (is_directory(cmd, data))
      exit(1);
    i = 0;
    while(i < (*data)->count_red_out)
    {
      flags = O_WRONLY | O_CREAT | append_or_trunc(cmd);
      *fd_out = open((*cmd)->file_output[i], flags, 0644);
      if (*fd_out < 0)
      {
        perror("minishell");
        set_status(1);
        exit(1);
      }
      dup2(*fd_out, STDOUT_FILENO);
      close(*fd_out);
      i++;
    }
  }
}

void  my_exit(t_command **cmd)
{
  if (strcmp((*cmd)->args[0], "exit") == 0)
  {
    printf("exit\n");
    int i = make_exit(*cmd);
    if (i == 0)
    {
      set_status(get_status());
      exit(get_status());
    }
    set_status(i);
    exit(i);
  }
}

void  open_and_duplicate(t_command **cmd, int *flags, int *fd_out)
{
  int i;
  if ((*cmd)->file_output)
  {
    i = 0;
    while ((*cmd)->file_output[i])
    {
      *flags = O_WRONLY | O_CREAT | append_or_trunc(cmd);
      *fd_out = open((*cmd)->file_output[i], *flags, 0644);
      if (*fd_out < 0)
        return;
      if (dup2(*fd_out, STDOUT_FILENO) == -1)
      {
        close(*fd_out);
        return;
      }
      close(*fd_out);
      i++;
    }
  }
}

int  is_directory_parent(t_command **cmd, t_data **data)
{
  DIR *folder;
  int i = 0;

  while (i < (*data)->count_red_out)
  {
    folder = opendir((*cmd)->file_output[i]);
    if (folder != NULL)
    {
      printf("minishell : %s:  Is a directory\n", (*cmd)->file_output[i]);
      set_status(1);
      closedir(folder);
      return(1);
    }
    closedir(folder);
    i++;
  }
  return(0);
}

void  excute_redirection_of_parent(t_command **cmd, int *fd_out, t_data **data)
{
  int (saved_stdout), saved_stdin, flags;
 
  if ((*cmd)->file_output)
  {
    if (is_directory_parent(cmd, data))
      return;
  }
  saved_stdout = dup(STDOUT_FILENO);
  saved_stdin = dup(STDIN_FILENO);
  open_and_duplicate(cmd, &flags, fd_out);
  my_exit(cmd);
  my_echo(*cmd);
  dup2(saved_stdout, STDOUT_FILENO);
  dup2(saved_stdin, STDIN_FILENO);
  close(saved_stdout);
  close(saved_stdin);
}

int  *init_status()
{
  static int  var = 0;
  return(&var);
}

void set_status(int val)
{
  int *status = init_status();
  *status = val;
}

int  get_status()
{
  return (*init_status());
  // return(*status);
}

void execute_command(t_command *cmd, char **env, t_data **data)
{
  pid_t pid;
  int save = dup(0);
  char *command;
  int fd_out = -1;
  int fd_in = -1;


  if (!(cmd->file) && cmd->herdoc_file)
  {
    fd_in = open(cmd->herdoc_file, O_RDONLY);
    if (fd_in < 0)
    {
      printf("minishell: No such file or directory\n");
      set_status(2);
      exit(1);
    }
    dup2(fd_in, STDIN_FILENO);
    close(fd_in);
  }
  if (cmd->args && built_in(cmd->args[0]))
  {
    excute_redirection_of_parent(&cmd, &fd_out, data);
    return;
  }
  signal(SIGINT, SIG_IGN);
  pid = fork();
  if (pid == 0)
  {
    if ((cmd)->file)
    {
      (cmd)->file = false;
      set_status(130);
      exit(130);
    }
    signal(SIGQUIT, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    excute_redirection_of_child(&cmd, data, &fd_out, &fd_in);
    if (cmd->args)
    {
      command = get_command(cmd->args[0], env);
      if (!command)
      {
        printf("minishell: %s: command not found\n", cmd->args[0]);
        set_status(127);
        exit(127);
      }
    }
    if (cmd->args)
    {
      execve(command, cmd->args, env);
      perror("execve failed");
    }
    set_status(1);
    exit(1);
  }
  else if (pid > 0)
  {
    dup2(save, 0);
    close(save);
    int status;
    waitpid(pid, &status, 0);
    if (WIFSIGNALED(status))
    {
      int sig = WTERMSIG(status);
      if (sig == SIGINT)
        write(1, "\n", 1);
      else if (sig == SIGQUIT)
        write(2, "Quit (core dumped)\n", 19);
      set_status(128 + sig);
    }
    else
      set_status(WEXITSTATUS(status));
    signal(SIGINT, my_handler);
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

  (*data)->start = 0;
  (*data)->end = 0;
	if (!check_somthing(line, data))
		return (NULL);
	while (line[(*data)->end])
	{
		if (line[(*data)->end] == '$' && (ft_isalnum(line[(*data)->end + 1]) || line[(*data)->end + 1] == '_'))
		{
			handle_dollar(&token, line, data);
			continue;
		}
		if (line[(*data)->end] == '$' && !ft_isalnum(line[(*data)->end + 1]))
    {
			handle_some_cases(&token, line, data);
    }
		if (line[(*data)->end] == '|' || line[(*data)->end] == '<' || line[(*data)->end] == '>')
		{
			handle_word_token(&token, line, data);
			(*data)->end = handle_speciale_token(&token, line, (*data)->end, data);
			(*data)->start = (*data)->end;
		}
		else if (line[(*data)->end] == '\"' || line[(*data)->end] == '\'')
			handle_special_quot(&token, line, data);
		else if (line[(*data)->end] == ' ' || line[(*data)->end] == '\t')
			handle_white_spaces(&token, line, data);
    else
		  (*data)->end++;
	}
  handle_word_token(&token, line, data);
	return (token);
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
  //(*data)->exit = 0;
  return(true);
}

void my_handler(int sig)
{
  if (sig == SIGINT)
  {
    write(1, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
    set_status(130);
  }
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
    // data->exit = get_status();
		line = readline(promt());
		if (!line)
		{
      // handle the exit status
			printf("exit\n");
			return;
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
			//print_token(token);
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
      // print_commands(cmd);
      if (data->count_herdoc > 0)
        excute_herdoc_for_child(&cmd, &data);  
			//print_commands(cmd);
			if (cmd)
				execute_command(cmd, env, &data);
			free_token(&token);
      free_cmd(cmd);
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
