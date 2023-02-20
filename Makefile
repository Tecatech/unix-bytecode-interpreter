CC = clang
CCFLAGS = -Wall -Wextra -g -std=c11
SRCS = mac.c

all:
	${CC} ${SRCS} ${CCFLAGS} -o mac