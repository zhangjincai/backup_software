
TARGET = test

CROSS_COMPILE = arm-none-linux-gnueabi-
CC = $(CROSS_COMPILE)gcc

CFLAGS = -g -Wall -O0  
LIBS = -L./ -l_general -lpthread -lrt
INCLUDES = -I./

#SRCS = $(wildcard *.c)
SRCS = tcp_test.c
OBJS = $(SRCS:.c=.o)

%.o:%.c
	$(CC) -c $< $(CFLAGS) $(INCLUDES)

$(TARGET):$(OBJS)
	$(CC) $^ -o $@ $(INCLUDES) $(LIBS) 

.PHONY:clean
clean:
	@$(RM) $(OBJS) $(TARGET)
