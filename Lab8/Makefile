#studentnum: 6973
PORT=56973
CFLAGS = -DPORT=$(PORT) -g -Wall -std=gnu99

all: readserver randclient bufserver bufserver_verbose

readserver: readserver.o socket.o
	gcc ${CFLAGS} -o $@ $^

randclient: randclient.o socket.o
	gcc ${CFLAGS} -o $@ $^

bufserver: bufserver.o socket.o
	gcc ${CFLAGS} -o $@ $^

bufserver_verbose: bufserver_verbose.o socket.o
	gcc ${CFLAGS} -o $@ $^

%.o: %.c socket.h
	gcc ${CFLAGS} -c $<

clean:
	rm *.o readserver randclient bufserver
