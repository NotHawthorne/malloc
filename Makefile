# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: alkozma <marvin@42.fr>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/02/11 15:35:39 by alkozma           #+#    #+#              #
#    Updated: 2020/02/18 00:34:42 by alkozma          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif
EXECNAME = malloc_test
SRC = free.c ft_memcpy.c util.c malloc.c realloc.c zone.c print.c ft_memset.c
OBJ = $(patsubst %.c,%.o,$(SRC))
HEADERS = malloc.h
NAME = libft_malloc_$(HOSTTYPE).so
NAMELINK = libft_malloc.so

TEST_C = test/test1.c

CXX = clang
CXXFLAGS = -Wall -Wextra -Werror -mmacosx-version-min=10.5 -O2

all: $(NAME)

$(NAME):
	$(CXX) $(CXXFLAGS) $(SRC) -o $(NAME) -shared -fPIC
	ln -s $(NAME) $(NAMELINK)

clean:
	/bin/rm -f $(OBJ)

fclean: clean
	/bin/rm -f $(NAME)
	/bin/rm -f $(NAMELINK) $(EXECNAME)

malloc_test: fclean
	$(CXX) $(CXXFLAGS) $(SRC) $(TEST_C) -o $(EXECNAME)

re: fclean all
