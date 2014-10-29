# TP_06 - Gestion de Fichiers
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


all: directories tme6exo1 exo1 mycp exo3 exo4 exo5 entended-grep


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
tme6exo1 : $(BIN)/tme6exo1
runtme6exo1 : tme6exo1
	$(BIN)/tme6exo1
exo1 : $(BIN)/exo1
runexo1 : exo1
	$(BIN)/exo1

#exo 2 :
mycp: $(BIN)/mycp

#exo 3 :
exo3: $(BIN)/exo3

#exo 4 :
exo4 : $(BIN)/exo4
runexo4 : exo4
	$(BIN)/exo4

#exo 5 :
exo5 : $(BIN)/exo5C $(BIN)/exo5POSIX

#exo 6 :
extended-grep: $(BIN)/extended-grep

clean:
	rm -f ${OBJ}/* ${BIN}/* ${LIB}/*

cleanall:
	rm -rf ${OBJ} ${BIN} ${LIB}
	rm -f ${INC}/*~ ${SRC}/*~ *~

