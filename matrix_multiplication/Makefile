NAME = matrix_multiptication.out
CFLAGS = -W -Wall -O3 -Wunused -Wfloat-equal -Wpointer-arith \
         -Wwrite-strings -Wno-format \
	 -Wcast-align -Wno-long-long -Wcast-align -pedantic \
	 -pedantic-errors -fstack-protector-all \
	 -Wuninitialized -Wunused-but-set-variable \
	 -Woverloaded-virtual -ffast-math
CC = g++ -c -pthread
LD = g++ -pthread
OBJS = main.o

${NAME}: ${OBJS}
	${LD} ${CFLAGS} ${OBJS} -o ${NAME}
main.o: main.cpp my_matrix.h
	${CC} ${CFLAGS} main.cpp

clean:
	rm -f ${OBJS} ${NAME}
