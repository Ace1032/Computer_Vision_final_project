CXX = g++
CXXFLAGS = `pkg-config --cflags --libs opencv4` -std=c++17
SRCS = $(wildcard *.cpp)
BINS = $(SRCS:.cpp=)

all: $(BINS)

%: %.cpp
	$(CXX) -o $@ $< $(CXXFLAGS)

clean:
	rm -f $(BINS)
