#include "../include/minishell.h"
#include <stdio.h>

void	handle_dollar(t_token **token, char *line, int *i, int *start, t_data **data)
{
	if (*i > *start)
	{
		handle_word_token(token, *start, line, i, data);
	}
	*start = *i;
	(*i)++;
	while ((ft_isalnum(line[*i]) || line[*i] == '_') && line[*i])
		(*i)++;
	if (*i > *start)
	{
		handle_word_token(token, *start, line, i, data);
	}
	*start = *i;
}

void	handle_special_quot(t_token **token, char *line, int *i, int *start, t_data **data)
{
  char q;

	if (line[*start] == '$' && (line[*i] == '\"' || line[*i] == '\''))
		*start = *i;
	if (*i > *start)
  {
    if (line[(*i) - 1] == '$')
    {
      (*i) -= 1;
		  handle_word_token(token, *start, line, i, data);
      (*i)++;
    }
    else
      handle_word_token(token, *start, line, i, data);
  }
	*start = *i;
	q = line[*i];
	(*i)++;
	while (line[*i] != q)
		(*i)++;
	if (line[*i] == q)
		(*i)++;
	handle_word_token(token, *start, line, i, data);
	*start = *i;
}

void	handle_white_spaces(t_token **token, char *line, int *i, int *start, t_data **data)
{
	handle_word_token(token, *start, line,  i, data);
	while (line[*i] == ' ' || line[*i] == '\t')
		(*i)++;
	*start = *i;
}

bool  check_somthing(char *word, t_data **data)
{
	if (!is_closed_quotes(word))
	{
		printf ("minishell : the quote does not close!\n");
    (*data)->exit = 2;
		return(false);
	}
	return (true);
}

void	handle_some_cases(t_token **token, char *line, int *i, int *start, t_data **data)
{
  if (*i > *start)
  {
    handle_word_token(token, *start, line, i, data);
    *start = *i;
  }
	if (line[(*i) + 1] == '?')
	{
		(*i) += 2;
		handle_word_token(token, *start, line, i, data);
		*start = *i;
	}
	else if (line[(*i) + 1] == '!')
	{
		*start = (*i) += 2;
	}
}
