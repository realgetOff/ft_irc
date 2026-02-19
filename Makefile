CC      = c++
NAME    = ircserv
OBJ_DIR = objs

SRCS    = srcs/main.cpp \
		  srcs/server/server.cpp \
          srcs/server/server_ops.cpp \
		  srcs/channel/channel.cpp \
          srcs/utils/signal.cpp \
          srcs/common/client.cpp \
		  srcs/utils/tokenizer.cpp \
		  srcs/commands/commands.cpp \
		  srcs/commands/main_commands.cpp \
		  srcs/commands/utils_commands.cpp \
		  srcs/commands/channel_commands.cpp \
		  srcs/commands/handle_commands.cpp

OBJS    = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

CFLAGS  = -I ./includes -Wall -Wextra -Werror --std=c++98 -g 
LDFLAGS =

all: $(NAME)

bot: 
	make -C srcs/bot/

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(NAME)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)/srcs
	rm -rf $(OBJ_DIR)/common
	make clean -C srcs/bot/

fclean: clean
	rm -f $(NAME)
	@if [ ! -d "$(OBJ_DIR)" ] || [ -z "$$(ls -A $(OBJ_DIR))" ]; then rm -rf $(OBJ_DIR); fi
	make fclean -C srcs/bot/

run: all
	./$(NAME) $(ARGS)

re: fclean all

.PHONY: all clean fclean re run
