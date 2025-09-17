# Compiler
CXX = g++
CXXFLAGS = -Wall -std=c++17

# Target executable
TARGET = server

# Source files
SRCS = main.cpp server.cpp HttpParser.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Header files
DEPS = server.h HttpParser.h

# Default rule
all: $(TARGET)

# How to build the target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# How to build object files
%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up compiled files
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
