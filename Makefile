CXX = g++
CXXFLAGS = -std=c++11 -Wall
LDFLAGS = -lpthread

BUILD_DIR = ./build

TARGET = ContarPalabras
TEST_TARGET = UnitTests
TEST_CONC = TestsConcurrencia
TEST_JULI = TestsJuli
TEST_TIEMPO = TestDeTiempo
OBJECTS = HashMapConcurrente.o CargarArchivos.o

all: build $(BUILD_DIR)/$(TARGET)

test: build $(BUILD_DIR)/$(TEST_TARGET)
	$(BUILD_DIR)/$(TEST_TARGET)

test_concurrencia: build $(BUILD_DIR)/$(TEST_CONC)
	$(BUILD_DIR)/$(TEST_CONC)

test_juli: build $(BUILD_DIR)/$(TEST_JULI)
	$(BUILD_DIR)/$(TEST_JULI)

test_tiempo: build $(BUILD_DIR)/$(TEST_TIEMPO)
	$(BUILD_DIR)/$(TEST_TIEMPO)
	python3 graph.py


$(BUILD_DIR)/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDFLAGS)

$(BUILD_DIR)/%: src/%.cpp $(OBJECTS:%=$(BUILD_DIR)/%)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: all test build clean debug

build:
	@mkdir -p $(BUILD_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

clean:
	-@rm -rvf $(BUILD_DIR)/*
