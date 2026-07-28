##
## Makefile — Wolf3D
##

NAME    =	wolf3d

CC      =	gcc

SRC     =	src/main.c 				\
			src/core/platform.c 	\
			src/core/app.c			\
			src/core/session.c		\
			src/map/level.c			\
			src/player/player.c		\
			src/engine/raycaster.c	\
			src/engine/renderer.c	\

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
	rm -f bsp_test

re:	fclean all

test_bsp:
	$(CC) -o bsp_test src/map/level.c src/map/test_bsp.c
	./bsp_test

.PHONY:	all debug clean fclean re test_bsp
