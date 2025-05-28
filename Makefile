
NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror -g

SRC = src/main.c	src/Handle_command.c	src/Handle_token.c	src/Helper.c  src/prompt.c

LIBFT_SRC = my_libft/ft_isdigit.c my_libft/ft_isalnum.c my_libft/ft_isascii.c my_libft/ft_isprint.c my_libft/ft_strlen.c my_libft/ft_memset.c my_libft/ft_isalpha.c \
	my_libft/ft_bzero.c my_libft/ft_memcpy.c my_libft/ft_memmove.c my_libft/ft_strlcpy.c my_libft/ft_strlcat.c my_libft/ft_toupper.c my_libft/ft_tolower.c \
	my_libft/ft_strchr.c my_libft/ft_strrchr.c my_libft/ft_strncmp.c my_libft/ft_memchr.c my_libft/ft_memcmp.c my_libft/ft_strnstr.c my_libft/ft_atoi.c \
	my_libft/ft_calloc.c my_libft/ft_strdup.c my_libft/ft_substr.c my_libft/ft_strjoin.c my_libft/ft_strtrim.c my_libft/ft_split.c my_libft/ft_itoa.c \
	my_libft/ft_putchar_fd.c my_libft/ft_putendl_fd.c my_libft/ft_putnbr_fd.c my_libft/ft_putstr_fd.c my_libft/ft_strmapi.c \
	my_libft/ft_lstnew_bonus.c my_libft/ft_lstadd_back_bonus.c my_libft/ft_lstadd_front_bonus.c my_libft/ft_lstsize_bonus.c \
	my_libft/ft_lstclear_bonus.c my_libft/ft_lstlast_bonus.c my_libft/ft_lstdelone_bonus.c my_libft/ft_lstiter_bonus.c

OBJ = $(SRC:.c=.o)
LIBFT_OBJ = $(LIBFT_SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ) $(LIBFT_OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LIBFT_OBJ) -o $(NAME) -lreadline

clean:
	rm -f $(OBJ) $(LIBFT_OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
.INTERMEDIATE: $(LIBFT_OBJ) $(OBJ)
