CC = gcc

all: clean run

clean: 
	rm -f *.o *.gch run.exe

generation:
	${CC} -Wall -g -lm -c generation.c generation.h

specimen:
	${CC} -Wall -g -lm -c specimen.c specimen.h

run: generation specimen
	${CC} -Wall -g -lm -o run main.c generation.o specimen.o
