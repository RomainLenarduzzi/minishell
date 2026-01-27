# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: shell <shell@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/01/27 18:41:00 by shell             #+#    #+#              #
#    Updated: 2026/01/27 18:41:00 by shell            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror -I./includes
LDFLAGS = -lreadline

# Source files
SRCS = srcs/main.c \
		srcs/parsing/tokenizer.c \
		srcs/parsing/parser.c \
		srcs/parsing/free_parsing.c \
		srcs/execution/execute.c \
		srcs/execution/builtin_check.c \
		srcs/execution/path.c \
		srcs/execution/heredoc.c \
		srcs/builtins/echo.c \
		srcs/builtins/cd.c \
		srcs/builtins/pwd.c \
		srcs/builtins/export.c \
		srcs/builtins/unset.c \
		srcs/builtins/env.c \
		srcs/builtins/exit.c \
		srcs/environment/env_init.c \
		srcs/environment/env_utils.c \
		srcs/environment/env_free.c \
		srcs/environment/env_expansion.c \
		srcs/signals/signals.c \
		srcs/utils/string_utils.c \
		srcs/utils/array_utils.c \
		srcs/utils/ft_strdup.c \
		srcs/utils/ft_strjoin.c

OBJS = $(SRCS:.c=.o)

# Colors
GREEN = \033[0;32m
RED = \033[0;31m
RESET = \033[0m

# Rules
all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(NAME)
	@echo "$(GREEN)✓ Minishell compiled successfully!$(RESET)"

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "$(GREEN)✓ Compiled: $<$(RESET)"

clean:
	@rm -f $(OBJS)
	@echo "$(RED)✗ Object files removed$(RESET)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)✗ Minishell removed$(RESET)"

re: fclean all

.PHONY: all clean fclean re
