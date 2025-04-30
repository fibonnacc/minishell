#ifndef MINISHELL_H
#define MINISHELL_H


#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <readline/history.h>
#include <unistd.h>
#include "../my_libft/libft.h"
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

typedef struct s_command
{
	char	**args;
	char	*file_input;
	char	*file_output;
	char	*herdoc;
	int		append;
	struct	s_command *next;
}	t_command;

void	make_prompt();
t_token *creat_token(char *line, t_token_type type);
void	handle_quote(bool	*in_quot ,char *quot_char, int *i, char *line);
void	add_token(t_token **token, t_token *new_token);
void	handle_word_token(t_token **token, int start, char *line, int i);
t_token_type	get_token_type(char *str);
int	handle_speciale_token(t_token **token, char *line, int i);
t_token	*tokenize(char *line);

#endif
