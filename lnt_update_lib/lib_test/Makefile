
TARGET = test

CROSS_COMPILE = arm-none-linux-gnueabi-
CC = $(CROSS_COMPILE)gcc
CFLAGS = -g -Wall -O0  
LIBS = -L./libs -l_lnt_update -lpthread -lrt
INCLUDES = -I./includes

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

%.o:%.c
	$(CC) -c $< $(CFLAGS) $(INCLUDES)

$(TARGET):$(OBJS)
	$(CC) $^ -o $@ $(INCLUDES) $(LIBS) 

.PHONY:clean
clean:
	@$(RM) $(OBJS) $(TARGET)
