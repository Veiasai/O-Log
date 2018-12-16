CC=gcc
CFLAGS=-lrdkafka -lz -lpthread -lrt

# Demo: Demo.o
# 	$(CC) -g $^ -o $@

example: example.o
	cc -g $^ -o $@ $(CFLAGS)