##
## Makefile — Wolf3D
##

NAME    =	wolf3d

CC      =	gcc

SRC     =	$(shell find src -name '*.c')

OBJ     =	$(SRC:.c=.o)

CFLAGS  =	-std=c11 -Wall -Wextra -Iinclude

LDFLAGS =	-lcsfml-graphics -lcsfml-window -lcsfml-audio -lcsfml-system -lm

all:	$(NAME)

$(NAME):	$(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(LDFLAGS)

debug:	CFLAGS += -g3 -fsanitize=address
debug:	LDFLAGS += -fsanitize=address
debug:	re

clean:
	rm -f $(OBJ)

fclean:	clean
	rm -f $(NAME)

re:	fclean all

.PHONY:	all debug clean fclean re
