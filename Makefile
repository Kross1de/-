CXX =g++
CXXF=-Wall -std=c++11 -O2
LDF = -lSDL2 -lGLEW -lGL

TARGET=sgustok

SOURCES=src/index.cpp \

OBJECTS=$(SOURCES:.cpp=.o)

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