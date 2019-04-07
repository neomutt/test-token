NEO	?= ../neo
CC	= gcc
RM	= rm -fr

SRC	+= main.c
SRC	+= filter.c
SRC	+= myvar.c
SRC	+= token.c
SRC	+= vars.c
SRC	+= window.c

OBJ	+= $(SRC:%.c=%.o)

OUT	= token

CFLAGS	+= -DDEBUG
CFLAGS	+= -Wall
CFLAGS	+= -Wno-unused-parameter
CFLAGS	+= -g
CFLAGS	+= -O0
CFLAGS	+= -I$(NEO)
CFLAGS	+= -fno-omit-frame-pointer

LDFLAGS	+= -L$(NEO) -lconfig -lmutt

# CFLAGS	+= -fsanitize=undefined
# LDFLAGS	+= -fsanitize=undefined

# CFLAGS	+=-fsanitize=address -fsanitize-recover=address
# LDFLAGS	+=-fsanitize=address -fsanitize-recover=address

all:	$(NEO) $(OBJ) $(OUT)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT):	$(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	$(RM) $(OBJ) $(OUT)

test:	$(OUT) force
	-./$(OUT) \
		"apple banana cherry" \
		"apple banana # cherry"

force:
