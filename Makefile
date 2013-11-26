

all:
	gcc -g -o2 th.c -o th.exe -lpthread -lrt

clean:
	rm -rf *.o a.out *.exe

