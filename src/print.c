
#include "../include/minishell.h"

void print_token(t_token *token)
{
	int i = 0;
	while (token)
	{
		printf("the value : {%s}", token->av);
		if (token->type == TOKEN_HERDOC)
		{
			if (token->info == 1)
				printf(" :   {True} : ");
			else
				printf(" :   {false} : ");
			printf ("TOKEN_HERDOC\n");
		}
		else if (token->type == TOKEN_REDIR_APPEND)
		{
			if (token->info == 1)
				printf(" : {True} : ");
			else
				printf(" : {false} : ");
			printf ("TOKEN_REDIR_APPEND\n");
		}
		else if (token->type == TOKEN_REDIR_IN)
		{
			if (token->info == 1)
				printf(" : {True} : ");
			else
				printf(" : {false} : ");
			printf ("TOKEN_REDIR_IN\n");
		}
		else if (token->type == TOKEN_REDIR_OUT)
		{
			if (token->info == 1)
				printf(" : {True} : ");
			else
				printf(" : {false} : ");
			printf ("TOKEN_REDIR_OUT\n");
		}
		else if (token->type == TOKEN_PIPE)
		{
			if (token->info == 1)
				printf(" : {True} : ");
			else
				printf(" : {false} : ");
			printf ("TOKEN_PIPE\n");
		}
		else
		{
			if (token->info == 1)
				printf(" : {True} : ");
			else
				printf(" : {false} : ");
			printf ("TOKEN_WORD\n");
		}
		i++;
		token = token->next;
	}
}


void print_commands(t_command *cmd)
{
	int i;
	int cmd_num = 1;

	while (cmd)
	{
		printf("-------- Command %d --------\n", cmd_num++);
		
		// Print args
		if (cmd->args)
		{
			printf("Args: ");
			for (i = 0; cmd->args[i]; i++)
				printf("'%s' ", cmd->args[i]);
			printf("\n");
		}
		else
			printf("Args: (none)\n");

		if (cmd->file_input)
			printf("Input File: '%s'\n", cmd->file_input);
		
		if (cmd->file_output)
		{
			printf("Output File: '%s'\n", cmd->file_output);
			printf("Append: %s\n", cmd->append ? "Yes" : "No");
		}

		if (cmd->herdoc)
			printf("Heredoc: '%s'\n", cmd->herdoc);
		printf("\n");
		cmd = cmd->next;
	}
}


