
NAME	=	BioSys

SRC	=	main.cpp \
		graphic.cpp \
		plot.cpp \
		Cell.cpp \
		boolParser.cpp \
		tools.cpp \
		tinyxml2.cpp

CC	=	clang++

OBJ	=	$(SRC:.cpp=.o)

CFLAGS =	-W -Wall -Wextra -ggdb

LDFLAGS =	-lSDL -lm -lxml2

$(NAME):	$(OBJ)
		$(CC) $(OBJ) $(CFLAGS) -o $(NAME) $(LDFLAGS)

clean:
		rm -f $(OBJ)

fclean:		clean
		rm -f $(NAME)

re:		fclean $(NAME)

%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@