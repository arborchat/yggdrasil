CC=musl-gcc
CFLAGS=-static -g -pedantic -Wall -Wextra
MAIN_OBJ=ygg.o
OBJS=json-parser/json.o protocol.o
BUILD_OBJS=$(MAIN_OBJ) $(OBJS)
TEST_OBJS=./cutest-1.5/CuTest.o ygg_test.o test.o $(OBJS)

ygg: $(BUILD_OBJS)
	$(CC) $(CFLAGS) $(BUILD_OBJS) -o ygg

test: ygg-tester
	./ygg-tester

ygg-tester: $(TEST_OBJS)
	$(CC) $(CFLAGS) $(TEST_OBJS) -o ygg-tester

protocol.o: protocol.c protocol.h

ygg.o: ygg.c json-parser/json.h

json.o: json-parser/json.c json-parser/json.h

ygg_test.o: ygg_test.c

test.o: test.c

CuTest.o: ./cutest-1.5/CuTest.c ./cutest-1.5/CuTest.h

clean:
	rm $(OBJS) $(MAIN_OBJ) $(TEST_OBJS) ygg ygg-tester
