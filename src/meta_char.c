#include "../include/minishell.h"
#include <string.h>

int	handle_pipe(t_token **current, t_command **current_cmd, t_command *first_cmd, t_data **data)
{
  t_command	*new_cmd;

  if ((*current)->type != TOKEN_PIPE)
    return (1);
  if ((*current)->next == NULL)
  {
    printf("minishell: syntax error near unexpected token `|'\n");
    (*data)->exit = 2;
    return (0);
  }
  new_cmd = create_command(data);
  if (!new_cmd)
  {
    free_cmd(first_cmd);
    return (0);
  }
  (*current_cmd)->next = new_cmd;
  *current_cmd = new_cmd;
  *current = (*current)->next;
  (*data)->exit = 0;
  return (1);
}

int	handle_redir_in(t_token **current, t_command *cmd, t_command *first_cmd, t_data **data)
{
  // if ((*current)->type != TOKEN_REDIR_IN)
  //   return (1);
  if (!(*current)->next)
  {
    printf("minishell: syntax error near unexpected token `newline'\n");
    set_status(2);
    // (*data)->exit = 2;
    return (0);
  }
  if ((*current)->next->type != TOKEN_WORD)
  {
    printf("minishell: syntax error near unexpected token `%s'\n", (*current)->next->av);
    set_status(2);
    // (*data)->exit = 2;
    return (0);
  }
  //if (cmd->file_input)
    // free(cmd->file_input);
  cmd->file_input[(*data)->count_red_in] = ft_strdup((*current)->next->av);
  if (!cmd->file_input[(*data)->count_red_in])
  {
    free_cmd(first_cmd);
    return (0);
  }
  (*data)->count_red_in++;
  (*current) = (*current)->next->next;
  // (*data)->exit = 0;
  return (1);
}

int	handle_redir_out(t_token **current, t_command *cmd, t_command *first_cmd, t_data **data)
{
  (void)data;
  // if ((*current)->type != TOKEN_REDIR_OUT)
  //   return (1);
  if (!(*current)->next)
  {
    printf("minishell: syntax error near unexpected token `newline'\n");
    (*data)->exit = 2;
    return (0);
  }
  if ((*current)->next->type != TOKEN_WORD)
  {
    printf("minishell: syntax error near unexpected token `%s'\n", (*current)->next->av);
    set_status(2);
    // (*data)->exit = 2;
    return (0);
  }
  cmd->file_output[(*data)->count_red_out] = ft_strdup((*current)->next->av);
  if (!cmd->file_output[(*data)->count_red_out])
  {
    free_cmd(first_cmd);
    return (0);
  }
  (*data)->count_red_out++;
  cmd->append = 0;
  (*current) = (*current)->next->next;
  return (1);
}

int	handle_redir_append(t_token **current, t_command *cmd, t_command *first_cmd, t_data **data)
{
  // if ((*current)->type != TOKEN_REDIR_APPEND)
  //   return (1);
  (void)data;
  if (!(*current)->next)
  {
    printf("minishell: syntax error near unexpected token `newline'\n");
    set_status(2);
    // (*data)->exit = 2;
    return (0);
  }
  if ((*current)->next->type != TOKEN_WORD)
  {
    printf("minishell: syntax error near unexpected token `%s'\n", (*current)->next->av);
    set_status(2);
    // (*data)->exit = 2;
    return (0);
  }
  cmd->file_output[(*data)->count_red_out] = ft_strdup((*current)->next->av);
  if (!cmd->file_output[(*data)->count_red_out])
  {
    free_cmd(first_cmd);
    return (0);
  }
  (*data)->count_red_out++;
  cmd->append = 1;
  (*current) = (*current)->next->next;
  // (*data)->exit = 0;
  return (1);
}

int	handle_heredoc(t_token **current, t_command *cmd, t_command *first_cmd, t_data **data, int *i)
{
  (void)data;
  // if ((*current)->type != TOKEN_HERDOC)
  //   return (1);
  if (!(*current)->next)
  {
    printf("minishell: syntax error near unexpected token `newline'\n");
    set_status(2);
    // (*data)->exit = 2;
    return (0);
  }
  if ((*current)->next->type != TOKEN_WORD)
  {
    printf("minishell: syntax error near unexpected token `%s'\n", (*current)->next->av);
    set_status(2);
    // (*data)->exit = 2;
    return (0);
  }
  cmd->herdoc[(*i)] = ft_strdup((*current)->next->av);
  if (!cmd->herdoc[*i])
  {
    free_cmd(first_cmd);
    return (0);
  }
  (*i)++;
  (*current) = (*current)->next->next;
  return (1);
}
