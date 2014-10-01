# TP2 - Processus légers (1)
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


all: directories thread_rand exo2 exo3 exo4 exo5


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

#exo 1 : 
thread_rand: $(BIN)/thread_rand
runexo1 : $(BIN)/thread_rand
	$(BIN)/thread_rand

#exo2 :
exo2: $(BIN)/exo2
runexo2 : $(BIN)/exo2
	$(BIN)/exo2

#exo3 :
exo3 : $(BIN)/exo3
runexo3 : $(BIN)/exo3
	$(BIN)/exo3

#exo4 :
exo4 : $(BIN)/exo4
runexo4 : $(BIN)/exo4
	$(BIN)/exo4

#exo5 :
exo5 : $(BIN)/exo5
runexo5 : $(BIN)/exo5
	$(BIN)/exo5

clean:
	rm -f ${OBJ}/* ${BIN}/* ${LIB}/*

cleanall:
	rm -rf ${OBJ} ${BIN} ${LIB}
	rm -f ${INC}/*~ ${SRC}/*~ *~

