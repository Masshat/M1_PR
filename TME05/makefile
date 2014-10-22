# TP5 - IPC Posix
#
# @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
# @copyright (c) 2014, toumlilt
#
# @version 1.0
# @package toumlilt/M1/PR

CC=gcc -Wall -ansi
BIN=bin
INC=include
LIB=lib
OBJ=obj
SRC=src

LRT=-lrt
LPTHREAD=-lpthread


all: directories exo1 exo2 exo3


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
	$(CC) -o $@ $< $(LRT) $(LPTHREAD)
#fin regles generales

#exo 1 :
exo1: $(BIN)/exo1
runexo1: $(BIN)/exo1
	$(BIN)/exo1

#exo2 :
exo2: $(BIN)/exo2
runexo2: $(BIN)/exo2
	$(BIN)/exo2

#exo3 :
exo3 : $(BIN)/chat_client $(BIN)/chat_server

clean:
	rm -f ${OBJ}/* ${BIN}/* ${LIB}/*

cleanall:
	rm -rf ${OBJ} ${BIN} ${LIB}
	rm -f ${INC}/*~ ${SRC}/*~ *~

