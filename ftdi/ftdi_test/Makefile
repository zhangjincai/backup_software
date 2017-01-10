all:
	arm-none-linux-gnueabi-gcc serial_test.c libftdi.a libusb-1.0.a lib_general.a -lpthread -lrt -o test 
	@#arm-none-linux-gnueabi-gcc stream_test.c libftdi.a libusb-1.0.a lib_general.a -lpthread -lrt -o test 
clean:
	@rm -f *.o test
