CXX = g++
CXXFLAGS = -g -I/usr/include/jsoncpp

TARGET = bin/projeto

# 1. Encontra todos os .cpp do projeto
ALL_SRCS = $(shell find . -name "*.cpp")

# 2. Filtra e REMOVE os arquivos de teste/conversão que causam o "multiple definition of main"
SRCS = $(filter-out ./Data/Csv_to_bin.cpp, $(ALL_SRCS))

# Descobre os diretórios de inclusão automaticamente
INCLUDES = $(shell find . -name "*.hpp" -exec dirname {} \; | sort -u | sed 's/^/-I/')

OBJS = $(SRCS:.cpp=.o)

LIBS = -ldrogon -ltrantor -ljsoncpp -luuid -lssl -lcrypto -lz -lpthread

$(TARGET): $(OBJS)
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@
	
clean:
	rm -f $(TARGET)
	find . -name "*.o" -delete

re: clean $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: clean re run