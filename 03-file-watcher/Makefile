.PHONY:all clean my_objs my_shared install

my_objs:
	gcc -c ./main.c -o ./objs/main.o -I ./inc -pthread
	gcc -c -fPIC ./src/logger.c -o ./objs/logger.o -I ./inc -pthread
	gcc -c -fPIC ./src/event_handle.c -o ./objs/event_handle.o -I ./inc -pthread
my_shared:
	gcc -shared ./objs/logger.o ./objs/event_handle.o -o ./libs/shared/libmd.so -pthread
install:
	cp -f ./libs/shared/libmd.so /usr/lib 
all:my_objs my_shared install
	gcc ./objs/main.o -L./libs/shared -lmd -o ./bin/use-shared-library -pthread
clean:
	rm -rf ./objs/*
	rm -rf ./libs/shared/libmd.so
	rm -rf ./bin/use-shared-library