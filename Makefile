CXX = g++
CXXF = -Wall -std=c++11 -O2
LDF = -lSDL2 -lGLEW -lGL

TARGET = KBM

SOURCES = src/main.cpp \
          src/shader.cpp \
          src/camera.cpp \
          src/cursor.cpp \
          src/texture.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDF)

%.o: %.cpp
	$(CXX) $(CXXF) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all run clean