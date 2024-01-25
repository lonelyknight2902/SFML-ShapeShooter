CXX := g++
OUTPUT := sfmlgame

SFML_DIR := .

CXX_FLAGS := -O3 -std=c++17
INCLUDES := -I./src -I$(SFML_DIR)/include
LDFLAGS := -O3 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -L$(SFML_DIR)

SRC_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(SRC_FILES:.cpp=.o)

all:$(OUTPUT)

$(OUTPUT): $(OBJ_FILES) Makefile
	$(CXX) $(OBJ_FILES) $(LDFLAGS) -o ./bin/$@

.cpp.o:
	$(CXX) -c $(CXX_FLAGS) $(INCLUDES) $< -o $@

clean:
	rm -f $(OBJ_SFMLGAME) ./bin/sfmlgame

run: $(OUTPUT)
	cd bin && ./sfmlgame && cd ..