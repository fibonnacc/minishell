#include "../include/minishell.h"

int g_value = 0;
static void  read_and_convert(char *buffer, int *fd, unsigned char *c, int *i)
{
  if (read(*fd, c, 1) != 1)
  {
    close(*fd);
    free(buffer);
    return;
  }
  buffer[(*i)++] = 'a' + (*c % 26);
}

static char *generate_file_name()
{
  unsigned char c;
  char *buffer; 
  int fd, i;

  buffer = malloc(10);
  if (!buffer)
    return NULL;
  fd = open("/dev/random", O_RDONLY);
  if (fd < 0)
    return (free(buffer), NULL);
  i = 0;
  while(i < 10)
    read_and_convert(buffer, &fd, &c, &i);
  buffer[i] = '\0';
  close(fd);
  return (buffer);
}

static void  make_loop(t_command **cmd , int *fd, int i)
{
  char *line;

  g_value = 0;
  while(1)
  {
    line = readline("> ");
    if (!line)
    {
      if (!g_value)
        printf(" warning: here-document at line delimited by end-of-file (wanted `%s')\n", (*cmd)->herdoc[i]);
      break;
    }
    if ((*cmd)->herdoc[i] == NULL)
      return;
    if (strcmp(line, (*cmd)->herdoc[i]) == 0)
    {
      free(line);
      return;
    }
    write(*fd, line, ft_strlen(line));
    write(*fd, "\n", 1);
    free(line);
  }
}

static void  minishell_init(char **buffer, char **join, int *fd)
{
    *buffer = generate_file_name();
    if (!*buffer)
      return;
    *join = ft_strjoin("/tmp/", *buffer);
    if (!*join)
      return;
    *fd = open(*join, O_RDWR | O_CREAT, 0777);
    if (*fd < 0)
      return;

}

static void  my_server(int ig)
{
  if (ig == SIGINT)
  {
    close(0);
    g_value = 1;
    printf("\n");
  }
}

int  herdoc_condition_1(t_command **cmd, t_data **data)
{
  if (g_value == 1)
  {
    (*cmd)->file = true;
    (*data)->exit = 130;
    return(0);
  }
  return(1);
}

void  herdoc_condition_2(t_command **cmd, t_data **data, char *join, int i)
{
  if (i == (*data)->count_herdoc - 1)
    (*cmd)->herdoc_file = ft_strdup(join);
  else
    unlink(join);
}

void excute_herdoc_for_child(t_command **cmd, t_data **data)
{
  int (i), fd;
  char (*buffer), *join;

  i = 0;
  int save = dup(0);
  while (i < (*data)->count_herdoc)
  {
    minishell_init(&buffer, &join, &fd);
    signal(SIGINT, my_server);
    make_loop(cmd, &fd, i);
    herdoc_condition_2(cmd, data, join, i);
    free(buffer);
    free(join);
    if (!herdoc_condition_1(cmd, data))
      break;
    i++;
  }
  dup2(save, 0);
  close(save);
  free_array((*cmd)->herdoc);
  (*data)->count_herdoc = 0;
}
