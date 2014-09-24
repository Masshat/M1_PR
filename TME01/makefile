# TP1 - Prise en main, Processus, Signaux
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


all: directories max_calcul nfork exo3 processus_creator exo5 exo6 exo7


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
$(BIN)/max_calcul: $(OBJ)/max_calcul.o $(OBJ)/max_func.o
	$(CC) -o $@ $^
max_calcul: $(BIN)/max_calcul
runexo1: $(BIN)/max_calcul
	$(BIN)/max_calcul 4 8 12 7

#exo 2 :
nfork: $(BIN)/nfork
runexo2: $(BIN)/nfork
	$(BIN)/nfork

#exo 3 :
exo3: $(BIN)/exo3
runexo3 : $(BIN)/exo3
	$(BIN)/exo3

#exo 4 :
exo4: $(BIN)/exo4
runexo4: $(BIN)/exo4
	$(BIN)/exo4

#exo 5 :
exo5: $(BIN)/exo5
runexo5: $(BIN)/exo5
	$(BIN)/exo5

#exo 6 :
exo6: $(BIN)/exo6
runexo6: $(BIN)/exo6
	$(BIN)/exo6

#exo 7 :
exo7: $(BIN)/exo7
runexo7: $(BIN)/exo7
	$(BIN)/exo7

clean:
	rm -f ${OBJ}/* ${BIN}/* ${LIB}/*

cleanall:
	rm -rf ${OBJ} ${BIN} ${LIB}
	rm -f ${INC}/*~ ${SRC}/*~ *~

