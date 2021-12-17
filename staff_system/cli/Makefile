objs :=  ./bsp/client.o\
				 ./bsp/viewer.o\
				 ./bsp/operate_func.o\
				 main.o

cli:$(objs)
	gcc $(objs) -o cli

%.o:%.c
	gcc -c $< -o $@

clean:
	rm -rf cli $(objs)