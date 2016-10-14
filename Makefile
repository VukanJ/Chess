CC = g++
CFLAGS = -std=c++14 -Wpedantic
LFLAGS = -lsfml-window -lsfml-graphics -lsfml-system
NAME = chess2
OBJ = Main.o ZobristHash.o Testing.o gui.o AI.o Board.o

compile: ${OBJ} EXE

Main.o: Main.cpp
	@echo Compiling Main.cpp ...
	@${CC} -c Main.cpp ${CFLAGS}

AI.o: AI.cpp AI.h
	@echo Compiling AI.cpp ...
	@${CC} -c AI.cpp ${CFLAGS}

Board.o: Board.cpp Board.h
	@echo Compiling Board.cpp ...
	@${CC} -c Board.cpp ${CFLAGS}

gui.o: gui.cpp gui.h
	@echo Compiling gui.cpp ...
	@${CC} -c gui.cpp ${CFLAGS}

Testing.o: Testing.cpp Testing.h
	@echo Compiling Testing.cpp ...
	@${CC} -c Testing.cpp ${CFLAGS}

ZobristHash.o: ZobristHash.cpp ZobristHash.h
	@echo Compiling ZobristHash.cpp ...
	@${CC} -c ZobristHash.cpp ${CFLAGS}

EXE: ${OBJ}
	@echo Linking ...
	@${CC} -o ${NAME} ${OBJ} ${LFLAGS}

clean:
	@rm *.o
