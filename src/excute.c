#include "../include/minishell.h"
#include <stdio.h>

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

