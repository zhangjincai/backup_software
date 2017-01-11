
TARGET = test

CROSS_COMPILE = arm-none-linux-gnueabi-
CC = $(CROSS_COMPILE)gcc
#CC = gcc


CFLAGS = -g -Wall -O0  
LIBS = -L./ -l_network_check 
INCLUDES = -I./

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

%.o:%.c
	$(CC) -c $< $(CFLAGS) $(INCLUDES)

$(TARGET):$(OBJS)
	$(CC) $^ -o $@ $(INCLUDES) $(LIBS) 

.PHONY:clean

clean:
	@$(RM) $(OBJS) $(TARGET)
