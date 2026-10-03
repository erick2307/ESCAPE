SOURCES = $(wildcard src/*.cpp)
BIN_DIR = bin

all: $(BIN_DIR)/sarsa

$(BIN_DIR)/sarsa: $(SOURCES) | $(BIN_DIR)
	g++ -std=c++17 $^ -o $@

debug: | $(BIN_DIR)
	g++ -std=c++17 -g $(SOURCES) -o bin/sarsa

profile: | $(BIN_DIR)
	g++ -std=c++17 -pg -g $(SOURCES) -o bin/sarsa


# Rule to create the bin folder if it does not exist
$(BIN_DIR):
	mkdir -p $@
