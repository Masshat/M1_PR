# TP9
#
# @author Ilyas Waye <ilyas.waye@gmail.com>
# @copyright (c) 2014, LaCamora
#
# @version 1.0
# @package waye/M1/PR

CC=gcc -Wall -ansi
BIN=bin
INC=include
LIB=lib
OBJ=obj
SRC=src
LPTHREAD = -lpthread


all: directories


directories: ${OBJ} ${BIN} ${LIB}

${OBJ}:
	mkdir ${OBJ}
${BIN}:
	mkdir ${BIN}
${LIB}:
	mkdir ${LIB}

# regles generales :
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -c -o $@ $< -I$(INC) 

$(BIN)/% : $(OBJ)/%.o
	$(CC) -o $@ $< $(LPTHREAD)
#fin regles generales

exo1 : $(BIN)/exo1
exo2 : $(BIN)/exo2
exo3 : $(BIN)/exo3
exo4 : $(BIN)/exo4
exo5 : $(BIN)/exo5
exo6 : $(BIN)/exo6
exo7 : $(BIN)/exo7

clean:
	rm -f ${OBJ}/* ${BIN}/* ${LIB}/*

cleanall:
	rm -rf ${OBJ} ${BIN} ${LIB}
	rm -f ${INC}/*~ ${SRC}/*~ *~

