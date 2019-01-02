CXX = g++
CXXFLAGS = -std=c++11

bin = test
Target: $(bin)

all: $(Target)

test: xmlConfTest.o ../lib/tinystr.o ../lib/tinyxml.o ../lib/tinyxmlerror.o ../lib/tinyxmlparser.o
	$(CXX) -g $^ -o $@ $(CXXFLAGS)

clean_files=$(bin) *.o *.d core.* ../lib/*.o
clean: 
	rm $(clean_files) -rf 