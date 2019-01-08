CC = gcc
PARAMS = -Wall -g -lm

all: clean run

clean: 
	rm -f *.txt *.o *.gch gms.exe

generation:
	${CC} ${PARAMS} -c generation.c generation.h

specimen:
	${CC} ${PARAMS} -c specimen.c specimen.h

run: generation specimen
	${CC} ${PARAMS} -o gms.exe main.c generation.o specimen.o
