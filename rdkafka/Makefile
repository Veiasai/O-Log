CC = gcc
CXX = g++
CFLAGS = -lrdkafka -lz -lpthread -lrt 
CXXFLAGS = -std=c++11 -lz -lpthread -lrt #-ljsoncpp -lrdkafka++

bin = monitor
Target: rdkafka $(bin)

all: $(Target)

monitor: main.o  myConsumer.o myProducer.o demo_processor.o utils.o confLoader.o lib/tinystr.o lib/tinyxml.o lib/tinyxmlerror.o lib/tinyxmlparser.o link_lib/librdkafka++.so link_lib/librdkafka.so
	$(CXX) -g $^ -o $@ $(CXXFLAGS)

rdkafka:
	cp ./librdkafka-PrintPoint/src/librdkafka.a ./link_lib/
	cp ./librdkafka-PrintPoint/src/librdkafka.so.1 ./link_lib/
	ln -s librdkafka.so.1 ./link_lib/librdkafka.so -f
	cp ./librdkafka-PrintPoint/src-cpp/librdkafka++.a ./link_lib/
	cp ./librdkafka-PrintPoint/src-cpp/librdkafka++.so.1 ./link_lib/
	ln -s librdkafka++.so.1 ./link_lib/librdkafka++.so -f

clean_files=$(bin) *.o *.d core.* monitor *.so* *.a
clean: 
	rm $(clean_files) -rf 
