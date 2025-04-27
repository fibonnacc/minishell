#ifndef MINISHELL_H
#define MINISHELL_H

#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../libft/libft.h"

typedef struct s_data
{
	char	**av;
	struct s_data *next;
}	t_data;

void	make_prompt();

#endif
