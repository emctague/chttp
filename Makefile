CC=gcc
CFLAGS=-Wall -Werror -g3
LFLAGS=
OBJECTS=main.o error.o http.o request.o socket.o
BINARY=chttp

$(BINARY): $(OBJECTS)
	$(CC) $(OBJECTS) $(LFLAGS) -o $(BINARY)

%.o: %.c %.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm *.o $(BINARY)

