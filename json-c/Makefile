
TARGET=test

CROSS_COMPILE = arm-none-linux-gnueabi-
#CC = $(CROSS_COMPILE)gcc

CC = gcc
CFLAGS = -g -D__STRICT_ANSI__

#LIBS = -L/usr/local/json-c-0.9forarm/lib/ -ljson -l_crypto
#INCLUDES = -I/usr/local/json-c-0.9forarm/include/json/
#LIBS = -L/usr/local/lib/ -ljson -lcurl
#INCLUDES = -I/usr/local/include/json/

LIBS = -L./libs -l_json -lcurl -l_crypto 
INCLUDES = -I./includes

#SRCS = $(wildcard *.c)
#SRCS = ftp.c
SRCS = test.c

OBJS = $(SRCS:.c=.o)

%.o:%.c
	$(CC) -c $< $(CFLAGS) $(INCLUDES)

$(TARGET):$(OBJS)
	$(CC) $^ -o $@ $(INCLUDES) $(LIBS) 

.PHONY:clean
clean:
	@$(RM) $(OBJS) $(TARGET)
