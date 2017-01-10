

PROJECT_4G_PATH=/mysoftware/others/glib_test/glib_test_new/


RELEASE_PATH=/home/dengjs/dowork/BicycleProject/application
LIBRARY_PATH=$(PROJECT_4G_PATH)/library



LDFLAGS = 
CFLAGS = 
LIB_DIR =
INCLUDE_DIR =

LDFLAGS += -lm -lrt -lstdc++ -Wl -ldl -lz -lresolv
CFLAGS += -Wall -g -D_GNU_SOURCE -pthread


CC = arm-none-linux-gnueabi-gcc
AR = arm-none-linux-gnueabi-ar
STRIP = arm-none-linux-gnueabi-strip

INCLUDE_DIR += -I${LIBRARY_PATH}/glib/include/glib-2.0 -I${LIBRARY_PATH}/glib/lib/glib-2.0/include -I${LIBRARY_PATH}/libffi/lib/libffi-3.0.1/include
LIB_DIR += -L${LIBRARY_PATH}/glib/lib -lglib-2.0 -lgio-2.0 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -pthread -L ${LIBRARY_PATH}/libffi/lib -lffi 


#LIB_DIR += ${LIBRARY_PATH}/lib_general.a


CFLAGS += $(INCLUDE_DIR)
LDFLAGS += $(LIB_DIR)


TARGET=glib_test


SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=%.o)

BINS = $(TARGET)

all: $(OBJS) $(BINS)

$(OBJS): %.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
$(BINS): $(OBJS)
	@$(CC) -o $@  $^  -Xlinker --start-group $(LDFLAGS) -Xlinker --end-group
	
clean:
	@rm -f $(BINS) $(OBJS)
	
install:
	@cp -rf $(BINS) $(RELEASE_PATH)		

.PHONY: all clean install


