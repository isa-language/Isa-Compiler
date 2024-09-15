TARGET = ./build/isa

CXX = clang++
CXXFLAGS = -g -O3
LDFLAGS = `llvm-config --cxxflags --ldflags --system-libs --libs core orcjit native`

SRC_DIR = src
BUILD_DIR = build

SRCS = main.cpp $(SRC_DIR)/lexer.cpp $(SRC_DIR)/err.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDFLAGS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(SRC_DIR)/*.o $(TARGET)

.PHONY: clean

