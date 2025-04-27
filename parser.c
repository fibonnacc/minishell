#include "include/minishell.h"
#include "libft/libft.h"
#include <readline/readline.h>
#include <signal.h>

t_token *creat_token(char *line, t_token_type type)
{
	t_token *new_token;

	if (!(new_token = (t_token *)ft_calloc(0, sizeof(t_token))))
		return (NULL);
	if (!(new_token->av = ft_strdup(word)))
		return (free(new_token), NULL);
	new_token->type = type;
	new_token->next = NULL;
	return (new_token);
}

static void	handle_quote(bool	*in_quot ,char *quot_char, int *i, char *line)
{
	if ((line[*i] == '"' || line[*i] == '\''))
	{
		if (*quot_char == '\0')
		{
			*in_quot = true;
			*quot_char = line[*i];
		}
		else if (*quot_char == line[*i])
		{
			*in_quot = false;
			*quot_char = 0;
		}
	}
}

t_token	*tokenize(char *line)
{
	int i;
	t_token	*token = NULL;
	bool	in_quot = false;
	char	quot_char = 0;
	int start = 0;
	char *word;

	i = 0;
	while (line[i])
	{
	 	handle_quote(&in_quot, &quot_char, &i, line);
		if (i > start)
		{
			if (!(word = ft_substr(line, start, i - start)))
				return (NULL);
			if (!(add_token(&token, creat_token(word, TOKEN_WORD))))
				return (free(word), NULL);
			free(word);

		}
		i++;
	}
	return (token);
}


void	my_handler(int sig)
{
	if (sig == SIGINT)
	{
		write(1, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

void make_prompt()
{
    char *line;
    char *prompt;
	t_token	*token;
    
    prompt = ft_strjoin(getenv("HOME"), "@minishell$ ");
    if (!prompt)
		return;
    while(1)
    {
		signal(SIGINT, my_handler);
        line = readline(prompt);
        
        if (!line)
        {
            printf("exit\n");
            break;
        }
        if (line[0] != '\0')
        {
            add_history(line);
			token = tokenize(line);
        }
        free(line);
    }
    free(prompt);
    rl_clear_history();
}

int main()
{
    make_prompt();
}
