
#include "../include/minishell.h"

int	check_redir_syntax(t_token **current)
{
	if (!(*current)->next)
	{
		printf("minishell: syntax error near unexpected token `newline'\n");
		set_status(2);
		return (0);
	}
	if ((*current)->next->type != TOKEN_WORD)
	{
		printf("minishell: syntax error near unexpected token `%s'\n",
			(*current)->next->av);
		set_status(2);
		return (0);
	}
	return (1);
}

void	print_open_error(char *filename)
{
	write(2, "minishell: ", 11);
	write(2, filename, ft_strlen(filename));
	if (errno == EISDIR)
		write(2, ": Is a directory\n", 17);
	else if (errno == ENOTDIR)
		write(2, ": Not a directory\n", 18);
	else if (errno == ENOENT)
		write(2, ": No such file or directory\n", 28);
	else if (errno == EACCES)
		write(2, ": Permission denied\n", 20);
	else
	{
		write(2, ": ", 2);
		write(2, strerror(errno), ft_strlen(strerror(errno)));
		write(2, "\n", 1);
	}
}

int	open_output_file_1(t_command *cmd, char *filename)
{
	int	fd;

	fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0)
	{
		reset_redir_error(1);
		cmd->redir_error = true;
		print_open_error(filename);
		cmd->file_output = NULL;
		return (0);
	}
	close(fd);
	cmd->file_output = filename;
	return (1);
}

int	open_output_file_0(t_command *cmd, char *filename)
{
	int	fd;

	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
	{
		reset_redir_error(1);
		cmd->redir_error = true;
		print_open_error(filename);
		cmd->file_output = NULL;
		return (0);
	}
	close(fd);
	cmd->file_output = filename;
	return (1);
}
