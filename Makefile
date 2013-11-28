

all:
	gcc -Wall -g -o2 th.c ctx.S -o th.exe -lpthread -lrt

clean:
	rm -rf *.o a.out *.exe

