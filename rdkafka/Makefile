CC = gcc
CXX = g++
CFLAGS = -lrdkafka -lz -lpthread -lrt
CXXFLAGS = -std=c++11  -lrdkafka++ -lz -lpthread -lrt

bin = Demo example
Target: $(bin)
all: $(Target)

Demo: Demo.o
	$(CXX) -g $^ -o $@ $(CXXFLAGS)

example: example.o
	$(CC) -g $^ -o $@ $(CFLAGS)

clean_files=$(bin) *.o *.d
clean: 
	rm $(clean_files) -rf 