#include "../include/minishell.h"

int	handle_pipe(t_token **current, t_command **current_cmd, t_command *first_cmd)
{
  t_command	*new_cmd;

  if ((*current)->type != TOKEN_PIPE)
    return (1);
  if ((*current)->next == NULL)
  {
    printf("minishell: syntax error near unexpected token `|'\n");
    return (0);
  }
  new_cmd = create_command();
  if (!new_cmd)
  {
    free_cmd(first_cmd);
    return (0);
  }
  (*current_cmd)->next = new_cmd;
  *current_cmd = new_cmd;
  *current = (*current)->next;
  return (1);
}

int	handle_redir_in(t_token **current, t_command *cmd, t_command *first_cmd)
{
  // if ((*current)->type != TOKEN_REDIR_IN)
  //   return (1);
  if (!(*current)->next)
  {
    printf("minishell: syntax error near unexpected token `newline'\n");
    return (0);
  }
  if ((*current)->next->type != TOKEN_WORD)
  {
    printf("minishell: syntax error near unexpected token `%s'\n", (*current)->next->av);
    return (0);
  }
  if (cmd->file_input)
    free(cmd->file_input);
  cmd->file_input = ft_strdup((*current)->next->av);
  if (!cmd->file_input)
  {
    free_cmd(first_cmd);
    return (0);
  }
  (*current) = (*current)->next->next;
  return (1);
}

int	handle_redir_out(t_token **current, t_command *cmd, t_command *first_cmd)
{
  // if ((*current)->type != TOKEN_REDIR_OUT)
  //   return (1);
  if (!(*current)->next)
  {
    printf("minishell: syntax error near unexpected token `newline'\n");
    return (0);
  }
  if ((*current)->next->type != TOKEN_WORD)
  {
    printf("minishell: syntax error near unexpected token `%s'\n", (*current)->next->av);
    return (0);
  }
  if (cmd->file_output)
    free(cmd->file_output);
  cmd->file_output = ft_strdup((*current)->next->av);
  if (!cmd->file_output)
  {
    free_cmd(first_cmd);
    return (0);
  }
  cmd->append = 0;
  (*current) = (*current)->next->next;
  return (1);
}

int	handle_redir_append(t_token **current, t_command *cmd, t_command *first_cmd)
{
  // if ((*current)->type != TOKEN_REDIR_APPEND)
  //   return (1);
  if (!(*current)->next)
  {
    printf("minishell: syntax error near unexpected token `newline'\n");
    return (0);
  }
  if ((*current)->next->type != TOKEN_WORD)
  {
    printf("minishell: syntax error near unexpected token `%s'\n", (*current)->next->av);
    return (0);
  }
  if (cmd->file_output)
    free(cmd->file_output);
  cmd->file_output = ft_strdup((*current)->next->av);
  if (!cmd->file_output)
  {
    free_cmd(first_cmd);
    return (0);
  }
  cmd->append = 1;
  (*current) = (*current)->next->next;
  return (1);
}

int	handle_heredoc(t_token **current, t_command *cmd, t_command *first_cmd)
{
  // if ((*current)->type != TOKEN_HERDOC)
  //   return (1);
  if (!(*current)->next)
  {
    printf("minishell: syntax error near unexpected token `newline'\n");
    return (0);
  }
  if ((*current)->next->type != TOKEN_WORD)
  {
    printf("minishell: syntax error near unexpected token `%s'\n", (*current)->next->av);
    return (0);
  }
  if (cmd->herdoc)
    free(cmd->herdoc);
  cmd->herdoc = ft_strdup((*current)->next->av);
  if (!cmd->herdoc)
  {
    free_cmd(first_cmd);
    return (0);
  }
  (*current) = (*current)->next->next;
  return (1);
}
