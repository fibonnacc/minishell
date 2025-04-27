#include "include/minishell.h"
#include "libft/libft.h"

t_token *creat_token(char *line, t_token_type type)
{
	t_token *new_token;

	if (!(new_token = (t_token *)ft_calloc(0, sizeof(t_token))))
		return (NULL);
	if (!(new_token->av = ft_strdup(line)))
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

void	add_token(t_token **token, t_token *new_token)
{
	t_token *current;

	if (!*token)
	{
		*token = new_token;
		return ;
	}
	current = *token;
	while (current->next)
		current = current->next;
	current->next = new_token;
}

void	handle_word_token(t_token **token, int start, char *line, int i)
{
	char *word;

	if (i > start)
	{
		word = ft_substr(line, start, i - start);
		if (word)
		{
			add_token(token, creat_token(word, TOKEN_WORD));
			free(word);
		}
	}

}

t_token_type	get_token_type(char *str)
{
	if ((ft_strncmp(str, "|", ft_strlen("|"))) == 0)
		return (TOKEN_PIPE);
	else if ((ft_strncmp(str, "<", ft_strlen("<"))) == 0)
			return (TOKEN_REDIR_IN);
	else if ((ft_strncmp(str, ">", ft_strlen(">"))) == 0)
			return (TOKEN_REDIR_OUT);
	else if ((ft_strncmp(str, ">>", ft_strlen(">>"))) == 0)
			return (TOKEN_REDIR_APPEND);
	else if ((ft_strncmp(str, "<<", ft_strlen("<<"))) == 0)
			return (TOKEN_HERDOC);
	else
		return (TOKEN_WORD);
}

int	handle_speciale_token(t_token **token, char *line, int i)
{
	char	special[3];

	if ((line[i] == '>' || line[i] == '<') && (line[i + 1] == line[i]))
	{
		special[0] = line[i];
		special[1] = line[i];
		add_token(token, creat_token(special, get_token_type(special)));
		return (i + 2);
	}
	else
	{
		special[0] = line[i];
		add_token(token, creat_token(special, get_token_type(special)));
		return (i + 1);
	}
}

t_token	*tokenize(char *line)
{
	int i;
	t_token	*token = NULL;
	bool	in_quot = false;
	char	quot_char = 0;
	int start = 0;
	//char *word;

	i = 0;
	while (line[i])
	{
		handle_quote(&in_quot, &quot_char, &i, line);
		if (!in_quot && line[i] == ' ')
		{
			handle_word_token(&token, start, line,  i);
			while (line[i] == ' ')
				i++;
			start = i;
		}
		if (!in_quot && (line[i] == '|' || line[i] == '<' || line[i] == '>'))
		{
			handle_word_token(&token, start, line, i);

			i = handle_speciale_token(&token, line, i);
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

void	print_token(t_token *token)
{
	while (token)
	{
		printf("the token is :%s\n", token->av);
		token = token->next;
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
			print_token(token);
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
