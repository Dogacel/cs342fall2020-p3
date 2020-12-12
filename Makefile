all: libsmemlib.a  app create_memory destroy_memory

libsmemlib.a:  smemlib.c
	gcc -pthread -Wall -c smemlib.c -lrt -g 
	ar -cvq libsmemlib.a smemlib.o
	ranlib libsmemlib.a

app: app.c libsmemlib.a
	gcc -Wall -o app app.c -L. -pthread -lsmemlib -lrt -g

create_memory: create_memory.c libsmemlib.a
	gcc -Wall -o create_memory create_memory.c -L. -pthread -lsmemlib -lrt -g

destroy_memory: destroy_memory.c libsmemlib.a
	gcc -Wall -o destroy_memory destroy_memory.c -L. -pthread -lsmemlib -lrt -g

clean: 
	rm -fr *.o *.a *~ a.out  app smemlib.o smemlib.a libsmemlib.a  create_memory destroy_memory
