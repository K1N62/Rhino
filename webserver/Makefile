CC = gcc
LD = gcc
INC_DIR = include
SRC_DIR = source
OBJ_DIR = objects
BIN_DIR = bin
LOG_DIR = log
CFLAGS 	= -c -Wall -m64 -I $(INC_DIR)
LDFLAGS =

PROG = $(BIN_DIR)/httpd
RM = /bin/rm
MKDIR = /bin/mkdir

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(SRCS:.c=.o))) -lpthread

# Compile all
all: dir_tree $(PROG)

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(PROG)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

dir_tree:
	$(MKDIR) -p $(OBJ_DIR) $(BIN_DIR) $(LOG_DIR)

#clean
clean:
	$(RM) -f $(PROG) $(OBJ_DIR)/*
