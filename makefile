CFLAGS=-Wall -g
	
database: database.o
	gcc $(CFLAGS) database.c -o database

	

	
clean:
	rm database.o database
	



		
	
