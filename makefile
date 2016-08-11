CC = g++
CFLAGS = std=c++1y -Wall

D_SRC=src
CPP = $(addprefix $(D_SRC)/,base.cpp reg.cpp)

D_INC = $(D_SRC)/inc
H = $(addprefix $(D_INC)/,reg.h util.h)

D_BIN = bin
EXE=$(D_BIN)/cyan-alias

D_OBJ = $(BIN)/obj
OBJ=$(addprefix $D_OBJ/,base.o reg.o)

all: $(EXE)

$(EXE): $(OBJ) | $(D_BIN)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ)

$(D_OBJ)/%.o: $(D_SRC)/%.cpp $(H)
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJ): | $(D_OBJ)

$(D_OBJ): | $(D_BIN)
	mkdir $(D_OBJ)

$(D_BIN):
	mkdir $(D_BIN)

.PHONY: clean
clean:
	rm $(D_BIN)/*
	rm $(D_OBJ)/*
