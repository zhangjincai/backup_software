TARGET = crypto

COMPILER_PATH := arm-none-linux-gnueabi-

CC = $(COMPILER_PATH)gcc
AR = $(COMPILER_PATH)ar

SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=%.o)

LIB_NAME = lib_$(TARGET).a

LDFLAGS = -lpthread 
CFLAGS = -Wall -g -D_GNU_SOURCE

LIBS = -L./libs -l_general -lpthread -lrt
INCLUDES = -I./includes


all: $(OBJS) lib


$(OBJS): %.o:%.c
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBS) -c -o $@ $< 

lib: $(OBJS)
	$(AR) rcu $(LIB_NAME) $(OBJS) general.o

clean:
	@rm -f *.o
	@rm -f $(OBJS)
	@rm -f $(LIB_NAME)



.PHONY: all clean 

