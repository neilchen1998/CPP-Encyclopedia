CXX = g++

INCLUDE = -I./include/

CXXFLAGS = -Wall -std=c++2b
CXXFLAGS += -I/opt/homebrew/include
CXXFLAGS += $(INCLUDE)
LDFLAGS =
LDFLAGS += -L/opt/homebrew/lib -lfmt

BUILD_DIR := _build

TARGET = $(BUILD_DIR)/app

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(BUILD_DIR)/main.o
	@$(CXX) $(CXXFLAGS) -o $(TARGET) $(BUILD_DIR)/main.o $(LDFLAGS)

$(BUILD_DIR)/main.o: main.cc
	@mkdir -p $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -c main.cc -o $(BUILD_DIR)/main.o

run: $(TARGET)
	./$(TARGET)

clean:
	@rm -rf $(BUILD_DIR)
