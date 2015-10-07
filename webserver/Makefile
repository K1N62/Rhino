CC = gcc
LD = gcc
INC_DIR = includes
SRC_DIR = src
OBJ_DIR = objects
CFLAGS = -x c -ggdb -Wall -I$(INC_DIR)
LDFLAGS =
SRCS = $(SRC_DIR)/globalConfigVariables.c $(SRC_DIR)/utility.c $(SRC_DIR)/config.c $(SRC_DIR)/log.c $(SRC_DIR)/httpd.c $(SRC_DIR)/request.h $(SRC_DIR)/pathHandler.c
OBJS = $(OBJ_DIR)/globalConfigVariables.o $(OBJ_DIR)/utility.o $(OBJ_DIR)/config.o $(OBJ_DIR)/log.o $(OBJ_DIR)/httpd.o $(OBJ_DIR)/request.o $(OBJ_DIR)/pathHandler.o -lpthread
# To include all files in dir
#SRCS = $(wildcard $(SRC_DIR)/*.c)
#OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(SRCS:.c=.o))) -lpthread
PROG = bin/httpd
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
