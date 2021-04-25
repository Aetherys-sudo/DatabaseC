CFLAGS=-Wall -g -I
obj = switch.o hello.o arg_test.o database.o
exec = switch hello arg_test database
src = switch.c hello.c arg_test.c database.c

all: switch hello arg_test database

switch: switch.o
	gcc switch.c -o switch
	
hello: hello.o
	gcc hello.c -o hello

arg_test: arg_test.o
	gcc arg_test.c -o arg_test
	
database: database.o
	gcc database.c -o database

	

	
clean:
	rm $(obj) $(exec)
	



		
	
