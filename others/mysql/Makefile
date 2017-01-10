
TARGET=test

#CROSS_COMPILE = arm-none-linux-gnueabi-
#CC = $(CROSS_COMPILE)gcc
CC = gcc
CFLAGS = -g -Wall -O0  
LIBS = -L/mysql-5.0.95/lib/mysql/ -lmysqlclient
#-lpthread -lrt
INCLUDES = -I/mysql-5.0.95/include/mysql/

#SRCS = $(wildcard *.c)
SRCS = 03images.c
OBJS = $(SRCS:.c=.o)

%.o:%.c
	$(CC) -c $< $(CFLAGS) $(INCLUDES)

$(TARGET):$(OBJS)
	$(CC) $^ -o $@ $(INCLUDES) $(LIBS) 

.PHONY:clean
clean:
	@$(RM) $(OBJS) $(TARGET)
