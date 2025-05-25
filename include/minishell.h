#ifndef MINISHELL_H
#define MINISHELL_H

#ifndef	COLOR_START
#define COLOR_START "\001\033[1;36m\002"
#endif
#ifndef	COLOR_RESET
#define COLOR_RESET "\001\033[1;33m\002"
#endif
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <readline/history.h>
#include <unistd.h>
#include "../my_libft/libft.h"
#include <stdbool.h>
#include <signal.h>
#include <sys/wait.h>

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
	bool	should_expand;
	bool	should_not_expand;
  bool  should_join;
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


bool	special_character(char *str);

void	make_prompt();
bool	special_character(char *str);
t_token *creat_token(char *line, t_token_type type);
void	handle_quote(bool	*in_quot ,char *quot_char, int *i, char *line);
void	add_token(t_token **token, t_token *new_token);
bool is_closed_quotes(char *str);
void	handle_word_token(t_token **token, int start, char *line, int *i, bool should_expand, bool should_not_expand);
t_token_type	get_token_type(char *str);
int	handle_speciale_token(t_token **token, char *line, int i);
t_token	*tokenize(char *line);
void	append_arg(t_command *cmd, char *str);
t_command	*create_command();
void	free_cmd(t_command *cmd);
char	*expand_env(char *str, t_token **token);
t_command	*parsing_command(t_token *token);
int	is_space(char c);
void	free_token(t_token **token);
char *remove_quotes(char *str);


#endif
