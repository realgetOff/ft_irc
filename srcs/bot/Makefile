# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bot <bot@student.42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/01/19 18:00:00 by bot               #+#    #+#              #
#    Updated: 2026/01/19 18:00:00 by bot              ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = bot
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g
SRC = Bot.cpp main.cpp signal_bot.cpp gamestate.cpp
OBJ = $(SRC:.cpp=.o)

all: $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

