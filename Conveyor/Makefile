CC = gcc
CXX = g++
CFLAGS = -lz -lpthread -lrt -lrdkafka
CXXFLAGS = -std=c++17 -lz -lpthread -lrt -lrdkafka++

bin = conveyor
Target: $(bin)

all: $(Target)

conveyor: main.o  OnlineBackuper.o DiskBackuper.o utils.o ConfLoader.o RegexFilter.o Worker.o QuotaManager.o OnlineRecover.o DiskRecover.o lib/tinystr.o lib/tinyxml.o lib/tinyxmlerror.o lib/tinyxmlparser.o
	$(CXX) -g $^ -o $@ $(CXXFLAGS)

clean_files=$(bin) *.o *.d core.* monitor *.so* *.a
clean: 
	rm $(clean_files) -rf 
