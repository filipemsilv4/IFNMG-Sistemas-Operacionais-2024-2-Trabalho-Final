CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2

TARGET = huffman

SRCS = main.cpp Huffman.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)

exeC: $(TARGET)
	valgrind ./$(TARGET) C input.txt output.huff

exeD: $(TARGET)
	valgrind ./$(TARGET) D output.huff output.txt
