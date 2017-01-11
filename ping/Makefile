TARGET = network_check

COMPILER_PATH := arm-none-linux-gnueabi-

CC = $(COMPILER_PATH)gcc
#CC = gcc

AR = $(COMPILER_PATH)ar

SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=%.o)

LIB_NAME = lib_$(TARGET).a
  
CFLAGS = -Wall -g -D_GNU_SOURCE

INCLUDES = -I./


all: $(OBJS) lib


$(OBJS): %.o:%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $< 

lib: $(OBJS)
	$(AR) rcu $(LIB_NAME) $(OBJS)
	cp $(LIB_NAME) ./lib_test
	
clean:
	@rm -f *.o
	@rm -f $(OBJS)
	@rm -f $(LIB_NAME)


.PHONY: all clean 

