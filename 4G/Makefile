EXEC    = main
OBJS    = main.o 4g.o
CROSS   = arm-none-linux-gnueabi-
CC      = $(CROSS)gcc
STRIP   = $(CROSS)strip
CFLAGS  =  -g -o0 
CLIBRARY= -L ./lib -l_general -lpthread -lrt

all:	clean $(EXEC)

$(EXEC):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)  $(CLIBRARY) 
#	@$(CC) -o $@	$^  -Xlinker --start-group $(LDFLAGS) -Xlinker --end-group
#	$(STRIP) $@
	rm *.o
clean:
	rm -rf $(EXEC)  
