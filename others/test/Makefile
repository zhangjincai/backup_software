all:
	#arm-none-linux-gnueabi-gcc memcheck.c -g -o0 -o test  
	arm-none-linux-gnueabi-gcc  -DMEMWATCH -DMW_STDIO memcheck.c memwatch.c -o test
clean:
	rm -f *.o test
