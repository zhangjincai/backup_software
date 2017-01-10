TARGET = crypto

COMPILER_PATH := arm-none-linux-gnueabi-

CC = $(COMPILER_PATH)gcc
#CC = gcc
AR = $(COMPILER_PATH)ar


#目标自动依赖
SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=%.o)

#库名称
LIB_NAME = lib_$(TARGET).a

LDFLAGS = -lpthread -lm -lstdc++
CFLAGS = -Wall -Werror -g -D_GNU_SOURCE


#编译库
all: $(OBJS) lib

$(OBJS): %.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $< 

lib: $(OBJS)
	$(AR) rcu $(LIB_NAME) $(OBJS) 

clean:
	@rm -f *.o
	@rm -f $(OBJS)
	@rm -f $(LIB_NAME)



.PHONY: all clean 

