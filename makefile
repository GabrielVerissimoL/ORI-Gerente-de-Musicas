CXX = g++
CXXFLAGS = -Wall -g

TARGET = bin/projeto

SRCS = $(shell find . -name "*.cpp")

INCLUDES = $(shell find . -name "*.hpp" -exec dirname {} \; | sort -u | sed 's/^/-I/')

OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	mkdir -p bin
	$(CXX) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@
	
clean:
	rm -f $(OBJS) $(TARGET)

re: clean $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: clean re run