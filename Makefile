CC=gcc
SRC=src/webserver.c

webserver:
		$(CC) $(SRC) -o webserver
