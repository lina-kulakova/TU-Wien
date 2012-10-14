NAME = matrix_addition.out
CFLAGS = -g -W -Wall -O3 -Wunused -Wfloat-equal -Wpointer-arith -Wwrite-strings -Wno-format \
	-Wcast-align -Wno-long-long -Wcast-align -pedantic -pedantic-errors -fstack-protector-all \
	-Wuninitialized -Wunused-but-set-variable -Woverloaded-virtual -funroll-loops -ffast-math -std=c++0x
CC = g++ -c
LD = g++
OBJS = main.o

${NAME}: ${OBJS}
	${LD} ${CFLAGS} ${OBJS} -o ${NAME}
main.o: main.cpp my_matrix.h
	${CC} ${CFLAGS} main.cpp

clean:
	rm -f ${OBJS} ${NAME}
