# TP8 - Programmation avancée avec les sockets POSIX
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
exo1 : $(BIN)/ftpserver $(BIN)/ftpclient

#exo 2 :
exo2: $(BIN)/mychat
runexo2: exo2
	$(BIN)/mychat "225.0.0.10" "4567" "Toto"

#exo 3 :
exo3: $(BIN)/ponger $(BIN)/sonar

#exo 4 :
exo4: $(BIN)/connectclient $(BIN)/connectserver

clean:
	rm -f ${OBJ}/* ${BIN}/* ${LIB}/*

cleanall:
	rm -rf ${OBJ} ${BIN} ${LIB}
	rm -f ${INC}/*~ ${SRC}/*~ *~

