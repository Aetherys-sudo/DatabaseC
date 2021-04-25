#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define MAX_DATA 512
#define MAX_ROWS 100

typedef struct Address
{
	int id;
	int set;
	char name[MAX_DATA];
	char email[MAX_DATA];
	
} Address;

typedef struct Database
{
	Address rows[MAX_ROWS];
} Database;

typedef struct Connection
{
	FILE *file;
	Database *db;
} Connection;

void err_p(const char* message)
{
	if (errno)
	{	
		perror(message);
	}
	else
	{
		printf("ERROR: %s\n", message);
	}
	exit(1);
}

void print_Address(Address *addr)
{
	printf("%d %s %s\n", addr->id, addr->name, addr->email);
}

void load_DB(Connection *conn)
{
	int rc = fread(conn->db, sizeof(Database), 1, conn->file);
	
	if (rc != 1)
	{
		err_p("Failed to load database.");
	}
}

Connection *open_DB(const char *filename, char mode)
{
	Connection *conn = malloc(sizeof(Connection));
	
	if (!conn)
	{
		err_p("Memory error.");
	}
	
	conn->db = malloc(sizeof(Database));
	
	if (!conn->db)
	{
		err_p("Memory error.");
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
		err_p("Failed to open file.");
	}
	
	return conn;

}

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

void write_DB(Connection *conn)
{
	rewind(conn->file);
	int rc = fwrite(conn->db, sizeof(Database), 1, conn->file);
	if (rc != 1)
	{
		err_p("Failed to write database.");
	}
	
	rc = fflush(conn->file);
	
	if (rc == -1)
	{
		err_p("Cannot flush database.");
	}
}		

void create_DB(Connection *conn)
{
	int i = 0;
	
	for (i = 0; i < MAX_ROWS; i ++)
	{
		Address addr = {.id = i, .set = 0};
		conn->db->rows[i] = addr;
	}
}

void set_DB(Connection *conn, int id, const char *name, const char *email)
{
	Address *addr = &conn->db->rows[id];
	
	if (addr->set)
	{
		err_p("Already set.");
	}
	
	addr->set = 1;
	
	char *res = strncpy(addr->name, name, MAX_DATA);
	
	if (!res)
	{
		err_p("Name copy failed.");
	}
	
	res = strncpy(addr->email, email, MAX_DATA);
	
	if (!res)
	{
		err_p("Email copy failed.");
	}
}

void get_DB(Connection *conn, int id)
{
	Address *addr = &conn->db->rows[id];
	
	if (addr->set)
	{
		print_Address(addr);
	}
	else
	{
		err_p("ID is not set.");
	}
}

void del_DB(Connection *conn, int id)
{
	Address addr = {.id = id, .set = 0};
	conn->db->rows[id] = addr;
} 

void list_DB(Connection *conn, int id)
{
	int i = 0;
	Database *db = conn->db;
	
	for (i = 0; i < MAX_ROWS; i ++)
	{
		Address *cur = &db->rows[i];
		
		if (cur->set)
		{
			print_Address(cur);
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		err_p("USAGE: database <dbfile> <action> [action params]");
	}
	
	char *filename = argv[1];
	char action = argv[2][0];
	Connection *conn = open_DB(filename, action);
	int id = 0;
	
	if (argc > 3)
	{
		id = atoi(argv[3]);
	}
	
	if (id >= MAX_ROWS)
	{
		err_p("There's not that many records.");
	}
	
	switch(action)
	{
		case 'c':
			create_DB(conn);
			write_DB(conn);
			break;
		
		case 'g':
			if (argc != 4)
			{
				err_p("Need an id to get.");
			}
			get_DB(conn, id);
			break;
		
		case 's':
			if (argc != 6)
			{
				err_p("Need id, name, email to set.");
			}
			set_DB(conn, id, argv[4], argv[5]);
			write_DB(conn);
			break;
		
		case 'd':
			if (argc != 4)
			{
				err_p("Need an ID to delete.");
			}
			del_DB(conn, id);
			write_DB(conn);
			break;
		
		case 'l':
			list_DB(conn, id);
			break;
		
		default:
			err_p("Invalid action: c = create, g = get, s = set, d = del, l = list");
	}
		
		close_DB(conn);
		
		return 0;
}
