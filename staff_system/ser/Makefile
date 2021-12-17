objs :=  ./bsp/mysqlite.o \
				./bsp/server.o \
				main.o

ser:$(objs)
	gcc $(objs) -o ser -lsqlite3 -pthread


mysqlite.o:mysqlite.c
	gcc -c ./bsp/mysqlite.c -o mysqlite.o -lsqlite3

server.o:server.c
	gcc -c server.c -o server.o -pthread

main.o:main.c
	gcc -c main.c -o main.o

clean:
	rm -rf ser $(objs)
	
