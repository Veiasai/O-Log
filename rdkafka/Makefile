CC = gcc
CXX = g++
CFLAGS = -lrdkafka -lz -lpthread -lrt 
CXXFLAGS = -std=c++11  -lrdkafka++ -lz -lpthread -lrt -ljsoncpp

bin = monitor
Target: $(bin)

all: $(Target)

monitor: main.o  myConsumer.o myProducer.o demo_processor.o utils.o confLoader.o lib/tinystr.o lib/tinyxml.o lib/tinyxmlerror.o lib/tinyxmlparser.o
	$(CXX) -g $^ -o $@ $(CXXFLAGS)

clean_files=$(bin) *.o *.d core.* monitor *.so* *.a
clean: 
	rm $(clean_files) -rf 
