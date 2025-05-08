#include "my_libft/libft.h"
#include <stdbool.h>
#include <stdio.h>

void	handle_quot(char *line, int *i, bool *in_quot, char *find_quot)
{
	if (line[*i] == '"' || line[*i] == '\'')
	{
		if (*find_quot == '\0')
		{
			*find_quot = line[*i];
			*in_quot = true;
		}
		else if (line[*i] == *find_quot)
		{
			*find_quot = 0;
			*in_quot = false;
		}
	}
}

void	make_tokens(t_list	**head, char *line)
{
	int i = 0;
	bool	in_quot = false;
	char	find_quot = 0;
	int	start = 0;
	while (line[i])
	{
		handle_quot(line, &i, &in_quot, &find_quot);
		if (!in_quot && (line[i] == '|' || line[i] == '>' || line[i] == '<'))
		{
			if (i > start)
			{
				char *word = ft_substr(line, start, i - start);
				if (word)
				{
					ft_lstadd_back(head, ft_lstnew(word));
				}
			}
			else
			{
				char	*special = malloc(sizeof(char) * 3);
				if (line[i] == line[i + 1])
				{
					special[0] = line[i];
					special[1] = line[i + 1];
					special[2] = '\0';
					i += 2;
				}
				else
				{
					special[0] = line[i];
					special[1] = '\0';
					i++;
				}
				ft_lstadd_back(head, ft_lstnew(special));
			}
			start = i;
			continue;
		}
		else if (!in_quot && line[i] == ' ')
		{
			if (i > start)
			{
				char *word = ft_substr(line, start, i - start);
				if (word)
				{
					ft_lstadd_back(head, ft_lstnew(word));
				}
			}
			while (line[i] == ' ')
				i++;
			start = i;
			continue;
		}
		i++;
	}
	if (i > start)
	{
		char *word = ft_substr(line, start, i - start);
		if (word)
		{
			ft_lstadd_back(head, ft_lstnew(word));
		}
	}
}

void	print(t_list *head)
{
	t_list	*current = head;
	while (current)
	{
		printf("%s\n", (char *)(current->content));
		current = current->next;
	}
}

void	free_list(t_list	**head)
{
	t_list	*current = *head;
	void	*next;
	while (current)
	{
		next = current->next;
		free(current->content);
		free(current);
		current = next;
	}
	*head = NULL;
}

int	main()
{
	t_list	*head = NULL;
	char	*word = "ls cat >";
	make_tokens(&head, word);
	print(head);
	free_list(&head);
}
