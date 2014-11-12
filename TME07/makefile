# TP7 - Introduction aux sockets POSIX
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
LPTHREAD = -lpthread

all: directories exo1 

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
	$(CC) -o $@ $<
#fin regles generales

#exo 1 :
exo1 : $(BIN)/exo1
runexo1: exo1
	$(BIN)/exo1

#exo 2 :
exo2 : $(BIN)/recvfile $(BIN)/sendfile

#exo 3 :
exo3 : $(BIN)/env_client $(BIN)/env_server

#exo 4 :
exo4 : $(BIN)/ftpserver $(BIN)/ftpclient

clean:
	rm -f ${OBJ}/* ${BIN}/* ${LIB}/*

cleanall:
	rm -rf ${OBJ} ${BIN} ${LIB}
	rm -f ${INC}/*~ ${SRC}/*~ *~

