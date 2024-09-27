.PHONY:all clean 

all:
	gcc -o checkout checkout_manager.c -lrt -lpthread 
	gcc -o order order_manager.c -g 
clean:
	rm -rf checkout order order_fifo
