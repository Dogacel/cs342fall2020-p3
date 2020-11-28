all: libsmemlib.a  app

libsmemlib.a:  smemlib.c
	gcc -Wall -c smemlib.c
	ar -cvq libsmemlib.a smemlib.o
	ranlib libsmemlib.a

app: app.c
	gcc -Wall -o app app.c -L. -lsmemlib -lpthread

clean: 
	rm -fr *.o *.a *~ a.out  app smemlib.o smemlib.a libsmemlib.a
