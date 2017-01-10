EXEC    = bicycle_test
OBJS    =  bicycle.o bicycle_test.o gps.o wifi.o 

CROSS   = arm-none-linux-gnueabi-
CC      = $(CROSS)gcc
STRIP   = $(CROSS)strip
CFLAGS  =  -g -o0 -D_GNU_SOURCE

CLIBRARY = -L ./lib -l_general -l_ctos -l_wireless -lusb-1.0 -lftdi -lserialnumber -l_crypto
CLIBRARY+= -lpthread -lm -lstdc++ -ldl -lrt

all:	clean $(EXEC)

$(EXEC):$(OBJS)
	$(CC) $(CFLAGS)  -o $@ $(OBJS) -Xlinker --start-group $(CLIBRARY) -Xlinker --end-group
	$(STRIP) $@

clean:
	rm -rf $(EXEC) *.o
