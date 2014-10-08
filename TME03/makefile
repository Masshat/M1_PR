# TP3 - Processus légers (2)
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


all: directories file_converter file_converter_2 exo3 exo4 exo5


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
file_converter: $(BIN)/file_converter
runexo1 : $(BIN)/file_converter
	$(BIN)/file_converter "src/test/file1.txt" "src/test/file2.txt"

#exo 2 :
file_converter_2 : $(BIN)/file_converter_2
runexo2 : $(BIN)/file_converter_2
	$(BIN)/file_converter_2 "src/test/file1.txt" "src/test/file2.txt" "src/test/file3.txt" "src/test/file4.txt"

#exo 3 :
$(BIN)/exo3 : $(OBJ)/thread_stack.o $(OBJ)/exo3.o
	$(CC) -o $@ $^ $(LPTHREAD)
exo3 : $(BIN)/exo3
runexo3: $(BIN)/exo3
	$(BIN)/exo3

#exo 4 :
exo4: $(BIN)/exo4
runexo4: $(BIN)/exo4
	$(BIN)/exo4

#exo 5 :
exo5: $(BIN)/exo5
runexo5: $(BIN)/exo5
	$(BIN)/exo5

clean:
	rm -f ${OBJ}/* ${BIN}/* ${LIB}/*

cleanall:
	rm -rf ${OBJ} ${BIN} ${LIB}
	rm -f ${INC}/*~ ${SRC}/*~ *~

