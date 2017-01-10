
TARGET = test

CC = arm-none-linux-gnueabi-gcc


CFLAGS = -g -Wall -O0  
LIBS = -L./ -l_list 
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
