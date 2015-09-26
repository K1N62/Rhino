CC = gcc
LD = gcc
INC_DIR = includes
SRC_DIR = src
OBJ_DIR = objects
CFLAGS = -x c -g -Wall -I$(INC_DIR)
LDFLAGS =
SRCS = $(SRC_DIR)/config.c $(SRC_DIR)/httpd.c $(SRC_DIR)/utility.c
OBJS = $(OBJ_DIR)/config.o $(OBJ_DIR)/httpd.o $(OBJ_DIR)/utility.o
# To include all files in dir
#SRCS = $(wildcard $(SRC_DIR)/*.c)
#OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(SRCS:.c=.o)))
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
