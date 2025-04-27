#include "include/minishell.h"
#include "libft/libft.h"
#include <stdio.h>

char	*sub_t(char *line)
{
	int i = 0;
	int len = 0;
	while (line[i])
	{
		if (line[i] != '"')
		{
			len++;
		}
		i++;
	}
	char *buffer = malloc((len + 1) * sizeof(char));
	if (!buffer)
	{
		return (NULL);
	}
	i = 0;
	int j = 0;
	while(line[i])
	{
		if (line[i] != '"')
		{
			buffer[j] = line[i];
			j++;
		}
		i++;
	}
	buffer[i] = '\0';
	return (buffer);
}

void	my_split(char *line)
{
	char **av = ft_split(line, ' ');
	int i = 0;
	while (av[i])
	{
		printf ("%s\n", av[i]);
		i++;
	}
}

void make_prompt()
{
    char *line;
    char *prompt;
	char *print;
    
    prompt = ft_strjoin(getenv("HOME"), "@minishell$ ");
    if (!prompt)
		return;
    while(1)
    {
        line = readline(prompt);
        
        if (!line)
        {
            printf("exit\n");
            break;
        }
        if (line)
        {
            add_history(line);
        }
		print = sub_t(line);
		my_split(print);
        free(line);
    }
    free(prompt);
    rl_clear_history();
}

int main()
{
    make_prompt();
}
