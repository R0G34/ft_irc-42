NAME    := ircserv
OBJ_DIR := obj
DIR_INCLUDES := includes
CC      := c++
CFLAGS  := -Wall -Wextra -Werror -std=c++98 -I$(DIR_INCLUDES)

VPATH := src src/cmds

SRC     :=  main.cpp \
            Client.cpp \
            Server.cpp \
            Channel.cpp \
            utils.cpp \
            ServerCommands.cpp \
            CmCAP.cpp \
            CmInvite.cpp \
            CmJoin.cpp \
            CmKick.cpp \
            CmList.cpp \
            CmMode.cpp \
            CmNames.cpp \
            CmNick.cpp \
            CmPart.cpp \
            CmPass.cpp \
            CmTopic.cpp \
            CmUser.cpp \
            CmPrivmsg.cpp \
            CmWho.cpp \
            CmQuit.cpp \
            CmNotice.cpp \

OBJ     := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

DEPS := includes/Client.hpp \
        includes/Server.hpp \
        includes/irc.hpp \
        includes/Channel.hpp

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o $(NAME)

$(OBJ_DIR)/%.o: %.cpp $(DEPS)
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
