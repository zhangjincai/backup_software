
TARGET = test

CROSS_COMPILE = arm-none-linux-gnueabi-
CC = $(CROSS_COMPILE)gcc
CFLAGS = -g -Wall -O0  
LIBS = -L./lib -lsqlite3 -lpthread -ldl
INCLUDES = -I./include

SRCS = $(wildcard ./src/*.c)
OBJS = $(SRCS:.c=.o)

%.o:%.c
	$(CC) -c $< $(CFLAGS) $(INCLUDES)
	@mv *.o src/

$(TARGET):$(OBJS)
	$(CC) $^ -o $@ $(INCLUDES) $(LIBS) 
	

.PHONY:clean
clean:
	@$(RM) $(OBJS) $(TARGET)
