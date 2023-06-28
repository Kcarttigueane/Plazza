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

# ! Add color to output
COLOR_RESET   = \033[0m
COLOR_PENDING = \033[01;33m
COLOR_SUCCESS = \033[01;32m
COLOR_DEFAULT = \033[01;36m
COLOR_CYAN    = \033[0;36m
GREEN         = \033[0;32m
COLOR_BLUE    = \033[0;34m
COLOR_YELLOW  = \033[0;33m

all: $(NAME)

$(NAME): $(OBJS)
	@printf "$(COLOR_CYAN)Plazza $(COLOR_YELLOW)➜  $(COLOR_PENDING)Linking...$(COLOR_RESET)\n"
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
	@printf "$(COLOR_CYAN)Plazza $(COLOR_BLUE)➜  $(COLOR_SUCCESS)Done.$(COLOR_RESET)\n"

-include $(DEPS)

%.o: %.cpp
	@printf "$(COLOR_CYAN)Plazza $(COLOR_BLUE)➜  $(GREEN)「✔」 $(COLOR_DEFAULT)Compiling $<...$(COLOR_RESET)\n"
	@$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

clean:
	@printf "$(COLOR_CYAN)Plazza $(COLOR_YELLOW)➜  $(COLOR_PENDING)Cleaning...$(COLOR_RESET)\n"
	@$(RM) $(OBJS)
	@$(RM) $(DEPS)
	@$(RM) NamedPipes/*
	@$(RM) log.txt
	@printf "$(COLOR_CYAN)Plazza $(COLOR_BLUE)➜  $(COLOR_SUCCESS)Done.$(COLOR_RESET)\n"

fclean: clean
	@printf "$(COLOR_CYAN)Plazza $(COLOR_YELLOW)➜  $(COLOR_PENDING)Full cleaning...$(COLOR_RESET)\n"
	@$(RM) $(NAME)
	@$(RM) $(wildcard core*)
	@printf "$(COLOR_CYAN)Plazza $(COLOR_BLUE)➜  $(COLOR_SUCCESS)Done.$(COLOR_RESET)\n"

tests_run:
	make -C tests re

re: fclean all

.PHONY: all clean fclean re
