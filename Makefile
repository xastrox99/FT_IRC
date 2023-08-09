NAME = ircserv

SRCS = parssing.cpp server.cpp Message.cpp client.cpp channel.cpp mode.cpp joinChannel.cpp privmsg.cpp TOPIC.cpp invite.cpp kick.cpp bot.cpp
SRC_PATHS = $(addprefix SRCS/,$(SRCS))

HEADERS = headerfile.hpp Message.hpp client.hpp channel.hpp
HEADER_PATHS = $(addprefix INC/,$(HEADERS))

OBJS = $(addprefix OBJS/,$(SRCS:.cpp=.o))

CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

OBJS/%.o: SRCS/%.cpp $(HEADER_PATHS) | OBJS
	$(CXX) $(CXXFLAGS) -c $< -o $@

OBJS:
	mkdir -p OBJS

all: $(NAME)

clean:
	rm -rf OBJS

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
