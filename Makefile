main: main.o scheduler.o process.o
	gcc -std=c99 main.o scheduler.o process.o -o main
main.o: main.c Makefile
	gcc -std=c99 main.c -c
scheduler.o: scheduler.c scheduler.h
	gcc -std=c99 scheduler.c -c
process.o: process.c process.h
	gcc -std=c99 process.c -c
clean:
	rm -f *o main
run:
	sudo ./main
