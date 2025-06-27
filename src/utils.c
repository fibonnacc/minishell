#include "../include/minishell.h"
#include <stdio.h>

void	handle_dollar(t_token **token, char *line, int *i, int *start)
{
	if (*i > *start)
	{
		handle_word_token(token, *start, line, i);
	}
	*start = *i;
	(*i)++;
	while ((ft_isalnum(line[*i]) || line[*i] == '_') && line[*i])
		(*i)++;
	if (*i > *start)
	{
		handle_word_token(token, *start, line, i);
	}
	*start = *i;
}

void	handle_special_quot(t_token **token, char *line, int *i, int *start)
{
  char q;

	if (line[*start] == '$' && (line[*i] == '\"' || line[*i] == '\''))
		*start = *i;
	if (*i > *start)
  {
    if (line[(*i) - 1] == '$')
    {
      (*i) -= 1;
		  handle_word_token(token, *start, line, i);
      (*i)++;
    }
    else
      handle_word_token(token, *start, line, i);
  }
	*start = *i;
	q = line[*i];
	(*i)++;
	while (line[*i] != q)
		(*i)++;
	if (line[*i] == q)
		(*i)++;
	handle_word_token(token, *start, line, i);
	*start = *i;
}

void	handle_white_spaces(t_token **token, char *line, int *i, int *start)
{
	handle_word_token(token, *start, line,  i);
	while (line[*i] == ' ' || line[*i] == '\t')
		(*i)++;
	*start = *i;
}

bool  check_somthing(char *word)
{
	if (!is_closed_quotes(word))
	{
		printf ("minishell : the quote does not close!\n");
		return(false);
	}
	return (true);
}

void	handle_some_cases(t_token **token, char *line, int *i, int *start)
{
	if (line[(*i) + 1] == '?')
	{
		(*i) += 2;
		handle_word_token(token, *start, line, i);
		*start = *i;
	}
	else if (line[(*i) + 1] == '!')
	{
		*start = (*i) += 2;
	}
}
