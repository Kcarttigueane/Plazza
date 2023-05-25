##
## EPITECH PROJECT, 2022
## Concurrent_programming
## File description:
## Makefile
##

CXX ?= g++
RM = rm -f

NAME = plazza

SRCDIR = ./src
UTILSDIR = $(SRCDIR)/utils
MAIN_FILE = ./main.cpp

SRCS = 	$(wildcard $(SRCDIR)/methods/*.cpp) \
		$(wildcard $(SRCDIR)/reception/*.cpp) \
       	$(wildcard $(SRCDIR)/*.cpp) \
       	$(wildcard $(UTILSDIR)/*.cpp) \
       	$(MAIN_FILE)

OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

CXXFLAGS += -std=c++20 -Wall -Wextra -I ./include -g3

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

-include $(DEPS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

clean:
	$(RM) $(OBJS)
	$(RM) $(DEPS)
	$(RM) NamedPipes/*
	$(RM) log.txt

fclean: clean
	$(RM) $(NAME)
	$(RM) $(wildcard core*)

tests_run:
	make -C tests re

re: fclean all

.PHONY: all clean fclean re
