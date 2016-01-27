CC = gcc
LD = gcc
INC_DIR = include
SRC_DIR = source
OBJ_DIR = objects
BIN_DIR = bin
LOG_DIR = log
CFLAGS 	= -c -Wall -m64 -I$(INC_DIR)
LDFLAGS =

LEX			= configScanner
BISON		= configParser

PROG = $(BIN_DIR)/httpd
RM = /bin/rm
MKDIR = /bin/mkdir

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(SRCS:.c=.o))) -lpthread

# Compile all
all: dir_tree bison flex $(PROG)

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(PROG)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

dir_tree:
	$(MKDIR) -p $(OBJ_DIR) $(BIN_DIR) $(LOG_DIR)

# create parser
bison:
	bison -o $(SRC_DIR)/$(BISON).c --defines=$(INC_DIR)/$(BISON).h $(SRC_DIR)/$(BISON).y

# create scanner
flex:
	flex -o $(SRC_DIR)/$(LEX).c --header-file=$(INC_DIR)/$(LEX).h $(SRC_DIR)/$(LEX).l

#clean
clean:
	$(RM) $(PROG) $(OBJ_DIR)/$(OBJS)
