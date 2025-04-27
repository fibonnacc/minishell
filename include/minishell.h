#ifndef MINISHELL_H
#define MINISHELL_H


#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <readline/history.h>
#include <unistd.h>
#include "../libft/libft.h"
#include <stdbool.h>
#include <signal.h>


typedef enum s_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_HERDOC
}	t_token_type;

typedef struct s_token
{
	char	*av;
	t_token_type	type;
	struct s_token *next;
}	t_token;

void	make_prompt();

#endif
