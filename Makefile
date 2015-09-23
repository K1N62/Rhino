CC = gcc
LD = gcc
INC_DIR = includes
SRC_DIR = src
OBJ_DIR = objects
CFLAGS = -g -Wall -I$(INC_DIR)
LDFLAGS =
SRCS = $(SRC_DIR)/httpd.c
OBJS = $(OBJ_DIR)/httpd.o
PROG = httpd
RM = /bin/rm

# Compile all
all: $(PROG)

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(PROG)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

#clean
clean:
	$(RM) $(PROG) $(OBJS)
