#include "../include/minishell.h"

void  compare_newline(char *str, bool *j, int *i)
{
  int k;

  k = 0;
  if (str[*i] && ft_strncmp(str, "-n", ft_strlen("-n")) == 0)
  {
    while(str[k])
    {
      if (str[k] != 'n')
        *j = false;
      else
        *j = true;
      k++;
    }
  }
  if (*j == true)
    (*i)++;
}

void	my_echo(t_command *cmd, t_data **data)
{
  int i;
  bool	j;

  j = false;
  i = 1;
  compare_newline(cmd->args[i], &j, &i);
  check_exit_status(cmd, data);
  while(cmd->args[i])
  {
    ft_putstr_fd(cmd->args[i], 1);
    if (cmd->args[i + 1])
      ft_putstr_fd(" ", 1);
    i++;
  }
  if (j == false)
    printf("\n");
}

bool	built_in(char *cmd)
{
  if (ft_strncmp(cmd, "exit", 4) == 0)
    return(true);
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
  if (ft_strchr(cmd, '/'))
  {
    if (access(cmd, X_OK) == 0)
    {
      return(ft_strdup(cmd));
    }
    return(NULL);
  }
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

