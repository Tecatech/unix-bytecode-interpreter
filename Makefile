CC = clang
CFLAGS = -Wall -Wextra -g -std=c11
SRCS = mac.c

all:
	${CC} ${SRCS} ${CFLAGS} -o mac