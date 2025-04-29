#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libft/libft.h"

typedef struct s_node
{
	char	*data;
	struct	s_node *left;
	struct	s_node *right;
}	t_node;

void	print(t_node *tree)
{
	if (tree == NULL)
	{
		printf("the func print is fail\n");
		return;
	}
	print(tree->right);
	printf ("tree: %s\n", tree->data);
	print(tree->left);
}

int main()
{
	t_node *tree = NULL;
	t_node	*node = malloc(sizeof(t_node));
	if (!node)
		return (2);

	node->data = "hello";
	node->left = NULL;
	node->right = NULL;
	tree = node;

	t_node	*node1 = malloc(sizeof(t_node));
	if (!node1)
		return (2);
	node1->data = "i am";
	node1->left = NULL;
	node1->right = NULL;
	tree->left = node1;

	t_node	*node2 = malloc(sizeof(t_node));
	if (!node1)
		return (2);

	node2->data = "hicham";
	node2->left = NULL;
	node2->right = NULL;
	tree->right = node2;

	print(tree);
}

