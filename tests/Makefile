##
## EPITECH PROJECT, 2022
## B-CCP-400-LYN-4-1-theplazza-jules.dutel
## File description:
## Makefile
##

CC = g++

CPPFLAGS = -std=c++20 -Wall -Wextra -Werror -g
CFLAGS = --coverage
LDFLAGS = -lcriterion

RM = rm -f

TARGET = unit_tests

SRCS = 	$(wildcard *.cpp) \
		$(wildcard ../*.cpp)

SRCS := $(filter-out ../main.cpp, $(SRCS))

OBJS = $(SRCS:.cpp=.o)

all	:	$(TARGET)
		./$(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(TARGET)
	$(RM) $(wildcard ../*.gcno)
	$(RM) $(wildcard ../*.gcda)
	$(RM) $(wildcard *.gcno)
	$(RM) $(wildcard *.gcda)

re: fclean all

.PHONY: all clean fclean re
