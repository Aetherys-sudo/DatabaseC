#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define MAX_DATA 512
#define MAX_ROWS 100

// definining data structures necessary for the database
typedef struct Address
{
	int id;
	int set;
	char name[MAX_DATA];
	char email[MAX_DATA];
	
} Address;

typedef struct Database
{
	int data;
	int m_rows;
	Address rows[MAX_ROWS];
} Database;

typedef struct Connection
{
	FILE *file;
	Database *db;
} Connection;

//function that closes the file and frees the allocated memory
void close_DB(Connection *conn)
{
	if (conn)
	{
		if (conn->file)
		{
			fclose(conn->file);
		}
		if (conn->db)
		{
			free(conn->db);
		}
		
		free(conn);
	}
}

//error parsing function that also deallocates memory and closes the database file
void err_p(Connection *conn, const char* message)
{
	if (errno)
	{	
		
		perror(message);
		if (conn)
		{
			close_DB(conn);
		}
	}
	else
	{
		printf("ERROR: %s\n", message);
		if (conn)
		{
			close_DB(conn);
		}
	}
	exit(1);
}

//self-explanatory, prints the database entry
void print_Address(Address *addr)
{
	printf("%d %s %s\n", addr->id, addr->name, addr->email);
}

//reads the database from file and inserts it in memory
void load_DB(Connection *conn)
{
	int rc = fread(conn->db, sizeof(Database), 1, conn->file);
	
	if (rc != 1)
	{
		err_p(conn, "Failed to load database.");
	}
}

//function that allocates the necessary memory and opens file in either read or write, depending on the flag chosen by the user
Connection *open_DB(const char *filename, char mode)
{
	Connection *conn = malloc(sizeof(Connection));
	
	if (!conn)
	{
		err_p(conn, "Memory error.");
	}
	
	conn->db = malloc(sizeof(Database));
	
	if (!conn->db)
	{
		err_p(conn, "Memory error.");
	}
	
	if (mode == 'c')
	{
		conn->file = fopen(filename, "w");
	}
	else
	{
		conn->file = fopen(filename, "r+");
		
		if (conn->file)
		{
			load_DB(conn);
		}
	}
	
	if (!conn->file)
	{
		err_p(conn, "Failed to open file.");
	}
	
	return conn;

}	


//function that passes the database in a file, basically saving it
void write_DB(Connection *conn)
{
	//rewind goes to the beginning of the file
	rewind(conn->file);
	int rc = fwrite(conn->db, sizeof(Database), 1, conn->file);
	if (rc != 1)
	{
		err_p(conn, "Failed to write database.");
	}
	
	rc = fflush(conn->file);
	
	if (rc == -1)
	{
		err_p(conn, "Cannot flush database.");
	}
}		

//function that creates the database entries, set=0 meaning that the field is free and ready to be loaded
//also sends information about the data size and the database row limit
// *REQUIRES FURTHER IMPLEMENTATION*
void create_DB(Connection *conn, int data, int m_rows)
{
	int i = 0;
	
	for (i = 0; i < MAX_ROWS; i ++)
	{
		Address addr = {.id = i, .set = 0};
		conn->db->rows[i] = addr;
		conn->db->data = data;
		conn->db->m_rows = m_rows;
	}
}

//checks if the field is set, if it's not it will fill the address field with the provided information
void set_DB(Connection *conn, int id, const char *name, const char *email)
{
	Address *addr = &conn->db->rows[id];
	
	if (addr->set)
	{
		err_p(conn, "Already set.");
	}
	
	addr->set = 1;
	
	char *res = strncpy(addr->name, name, MAX_DATA);
	
	if (!res)
	{
		err_p(conn, "Name copy failed.");
	}
	
	res = strncpy(addr->email, email, MAX_DATA);
	
	if (!res)
	{
		err_p(conn, "Email copy failed.");
	}
}

//receives an ID as input and prints the entry located at that ID
void get_DB(Connection *conn, int id)
{
	Address *addr = &conn->db->rows[id];
	
	if (addr->set)
	{
		print_Address(addr);
	}
	else
	{
		err_p(conn, "ID is not set.");
	}
}

//resets the entry at the given ID
void del_DB(Connection *conn, int id)
{
	Address addr = {.id = id, .set = 0};
	conn->db->rows[id] = addr;
} 

//prints all of the database if the entry is valid, also prints data size and max rows 
// *FURTHER WORK REQUIRED*
void list_DB(Connection *conn)
{
	int i = 0;
	Database *db = conn->db;
	
	for (i = 0; i < MAX_ROWS; i ++)
	{
		Address *cur = &db->rows[i];
		
		if (cur->set)
		{
			printf("Database has a maximum data size of %d bytes and a maximum of %d rows.\n", db->data, db->m_rows);
			print_Address(cur);
		}
	}
}

void find_DB(Connection *conn)
{
	
}

//main function, argument parsing
int main(int argc, char *argv[])
{
	//checks validity of argument input
	//program has no way of checking if the inserted arguments are valid or not
	// *FURTHER WORK REQUIRED*
	if (argc < 3)
	{
		err_p(NULL, "USAGE: database <dbfile> <action> <data_size> <rows> [action params]");
	}
	
	//static implementation of argument parsing, needs a rework
	char *filename = argv[1];
	char action = argv[2][0];
	Connection *conn = open_DB(filename, action);
	int id = 0;
	if (argc < 4)
	{
		err_p(conn, "Data size and number of rows required.");
	}
	int data = atoi(argv[3]);
	int	m_rows = atoi(argv[4]);
	
	if (argc > 5)
	{
		id = atoi(argv[5]);
	}
	
	if (id >= MAX_ROWS)
	{
		err_p(conn, "There's not that many records.");
	}
	
	//small scale menu for the application, needs to be improved upon
	switch(action)
	{
		case 'c':
			create_DB(conn, data, m_rows);
			write_DB(conn);
			break;
		
		case 'g':
			if (argc != 6)
			{
				err_p(conn, "Need an id to get.");
			}
			get_DB(conn, id);
			break;
		
		case 's':
			if (argc != 8)
			{
				err_p(conn, "Need id, name, email to set.");
			}
			set_DB(conn, id, argv[6], argv[7]);
			write_DB(conn);
			break;
		
		case 'd':
			if (argc != 6)
			{
				err_p(conn, "Need an ID to delete.");
			}
			del_DB(conn, id);
			write_DB(conn);
			break;
		
		case 'l':
			list_DB(conn);
			break;
		
		default:
			err_p(conn, "Invalid action: c = create, g = get, s = set, d = del, l = list");
	}
		
		close_DB(conn);
		
		return 0;
}
