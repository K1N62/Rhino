CC = gcc
LD = gcc
INC_DIR = include
SRC_DIR = src
OBJ_DIR = objects
BIN_DIR = bin
CFLAGS = -c -Wall -m64 -I$(INC_DIR)
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
	$(MKDIR) $(OBJ_DIR) $(BIN_DIR)

#clean
clean:
	$(RM) $(PROG) $(OBJ_DIR)/$(OBJS)
