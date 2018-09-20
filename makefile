CC=musl-gcc
CFLAGS=-static -g -pedantic -Wall -Wextra
OBJS=ygg.o json.o

ygg: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o ygg

ygg.o: ygg.c json.h

json.o: json.c json.h

clean:
	rm $(OBJS) ygg
