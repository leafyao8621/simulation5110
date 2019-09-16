CPP = g++
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)
BIN = main

%.o: %.cpp
	$(CPP) -g -c $< -o $@

$(BIN) : $(OBJ)
	$(CPP) $(OBJ) -o $(BIN)

.PHONY: clean
clean: $(BIN)
	@rm $(OBJ) $(BIN)
