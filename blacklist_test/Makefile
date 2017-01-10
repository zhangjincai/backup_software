
TARGET = test

CROSS_COMPILE = arm-none-linux-gnueabi-
CC = $(CROSS_COMPILE)gcc
CFLAGS = -g -Wall -O0  
LIBS = -L./ -l_blacklist -l_general -lpthread -lrt -ldl
INCLUDES = -I./

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

%.o:%.c
	$(CC) -c $< $(CFLAGS) 

$(TARGET):$(OBJS)
	$(CC) $^ -o $@ $(LIBS) 

.PHONY:clean
clean:
	@$(RM) $(OBJS) $(TARGET)
