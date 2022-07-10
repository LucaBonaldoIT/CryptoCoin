CXX=g++

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
EXE := $(BIN_DIR)/main

SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:.cpp=.o)

CXXFLAGS := -Iinclude -MMD -MP -Wall
LDFLAGS  := -Llib
LDLIBS   := -lcryptopp

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDFLAGS) $(LDLIBS)

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

-include $(OBJ:.o=.d)