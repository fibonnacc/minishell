#include "include/minishell.h"
#include "my_libft/libft.h"

int	main()
{
	int start = 0;
	int end = 0;

	char *str = "\"ls -la\"";
	int i = 0;
	if (str[i] == '"')
	{
		start = i + 1;
		char *buffer = str + 1;
		i = 0;
		while (buffer[i] != '"')
		{
			i++;
			end = i;
		}
		char *word = ft_substr(buffer, start, end - start);
		printf("---------> %s\n", word);
	}
}
