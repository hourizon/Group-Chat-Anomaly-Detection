CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
TARGET = bayes_classifier
SRC = src/main.cpp src/bayes.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) data/*.dat

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run