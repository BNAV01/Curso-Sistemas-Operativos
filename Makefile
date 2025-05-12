CXX = g++
CXXFLAGS = -Wall -std=c++17
TARGET = LAB2_Navarrete_Cartagena
SRC = LAB2_Navarrete_Cartagena.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)

