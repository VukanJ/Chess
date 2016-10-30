CC = g++
CFLAGS = -std=c++17 -Wpedantic
LFLAGS = -lsfml-window -lsfml-graphics -lsfml-system
NAME = chess2
OBJ = Main.o ZobristHash.o Testing.o gui.o AI.o Board.o

compile: ${OBJ} EXE

Main.o: Main.cpp
	@echo Compiling $< ...
	@${CC} -c -g $< ${CFLAGS}

AI.o: AI.cpp AI.h
	@echo Compiling $< ...
	@${CC} -c -g $< ${CFLAGS}

Board.o: Board.cpp Board.h
	@echo Compiling $< ...
	@${CC} -c -g $< ${CFLAGS}

gui.o: gui.cpp gui.h
	@echo Compiling $< ...
	@${CC} -c -g $< ${CFLAGS}

Testing.o: Testing.cpp Testing.h
	@echo Compiling $< ...
	@${CC} -c -g $< ${CFLAGS}

ZobristHash.o: ZobristHash.cpp ZobristHash.h
	@echo Compiling $< ...
	@${CC} -c -g $< ${CFLAGS}

EXE: ${OBJ}
	@echo Linking ...
	@${CC} -o ${NAME} ${OBJ} ${LFLAGS}

clean:
	@rm *.o
