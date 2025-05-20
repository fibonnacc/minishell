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

// void	make_tokens(t_list	**head, char *line)
// {
// 	int i = 0;
// 	bool	in_quot = false;
// 	char	find_quot = 0;
// 	int	start = 0;
// 	char **result = ft_split(line, ' ');
// 	while (result[i])
// 	{
// 		int j = 0;
// 		while (result[i][j])
// 		{
// 			handle_quot(line, &i, &in_quot, &find_quot);
// 			if (line[i] == '"' || line[i] == '\'')
// 			{
// 				start = j;
// 				j++;
// 				while (result[i][j] != '\"')
// 					i++;
// 				char *word = ft_substr(result[i], start, (j - start + 2));
// 				ft_lstadd_back(head, ft_lstnew(word));
// 				start = ++i;
// 				continue;
// 			}
// 		}
// 	}
// }

bool	is_space(char c) {
	return (c == ' ' || c == '\t');
}

void	add_token(t_list **head, char *line, int start, int end) {
	if (end > start) {
		char *token = ft_substr(line, start, end - start);
		ft_lstadd_back(head, ft_lstnew(token));
		free(token);
	}
}

void	make_tokens(t_list **head, char *line) {
	int i = 0;
	int start = 0;
	bool in_quote = false;
	char quote_char = 0;

	while (line[i]) {
		if (!in_quote && is_space(line[i])) {
			add_token(head, line, start, i);
			while (is_space(line[i]))
				i++;
			start = i;
			continue;
		}
		else if (!in_quote && (line[i] == '\'' || line[i] == '"')) {
			in_quote = true;
			quote_char = line[i++];
			start = i;
			while (line[i] && line[i] != quote_char)
				i++;
			add_token(head, line, start, i);
			i++; // skip closing quote
			start = i;
			in_quote = false;
			continue;
		}
		else if (!in_quote && (line[i] == '|' || line[i] == '<' || line[i] == '>')) {
			add_token(head, line, start, i);
			start = i;
			i++;
			add_token(head, line, start, i);
			start = i;
			continue;
		}
		i++;
	}
	add_token(head, line, start, i);
}


void	print(t_list *head)
{
	t_list	*current = head;
	while (current)
	{
		printf("%s\n", (char *)(current->content));
		current = current->next;
	}
	printf("NULL\n");
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
	char	*word = "ls cat";
	make_tokens(&head, word);
	print(head);
	free_list(&head);
}
