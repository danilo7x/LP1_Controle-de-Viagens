CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
SERVER_FLAGS = -std=c++17 -Iinclude -Ilib -O2
SRC = $(wildcard src/*.cpp)
TARGET = controlador
SERVER = servidor

# 'make' compila os dois. 'make controlador' so o terminal. 'make servidor' so o web.
all: $(TARGET) $(SERVER)

$(TARGET): $(SRC) main.cpp
	$(CXX) $(CXXFLAGS) $(SRC) main.cpp -o $(TARGET)

$(SERVER): $(SRC) servidor.cpp
	$(CXX) $(SERVER_FLAGS) $(SRC) servidor.cpp -o $(SERVER) -lpthread

clean:
	rm -f $(TARGET) $(SERVER) $(TARGET).exe $(SERVER).exe src/*.o *.o

run: $(TARGET)
	./$(TARGET)

web: $(SERVER)
	./$(SERVER)

.PHONY: all clean run web

