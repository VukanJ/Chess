CC = g++
CFLAGS = -std=c++11 -fms-extensions -Ofast -O3 -fpermissive
LFLAGS = -lsfml-window -lsfml-graphics -lsfml-system
NAME = chess2
OBJ = Main.o ZobristHash.o Testing.o AI.o gui.o Board.o Data.o Move.o
DEBUG = -g

compile: ${OBJ} EXE

Main.o: Main.cpp
	@echo Compiling $< ...
	@${CC} -c ${DEBUG} $< ${CFLAGS}

AI.o: AI.cpp AI.h
	@echo Compiling $< ...
	@${CC} -c ${DEBUG} $< ${CFLAGS}

Board.o: Board.cpp Board.h
	@echo Compiling $< ...
	@${CC} -c ${DEBUG} $< ${CFLAGS}

gui.o: gui.cpp gui.h
	@echo Compiling $< ...
	@${CC} -c ${DEBUG} $< ${CFLAGS}

Testing.o: Testing.cpp Testing.h
	@echo Compiling $< ...
	@${CC} -c ${DEBUG} $< ${CFLAGS}

ZobristHash.o: ZobristHash.cpp ZobristHash.h
	@echo Compiling $< ...
	@${CC} -c ${DEBUG} $< ${CFLAGS}

Data.o: Data.cpp Data.h
	@echo Compiling $< ...
	@${CC} -c ${DEBUG} $< ${CFLAGS}

Move.o: Move.cpp Move.h
	@echo Compiling $< ...
	@${CC} -c ${DEBUG} $< ${CFLAGS}

EXE: ${OBJ}
	@echo Linking ...
	@${CC} -o ${NAME} ${OBJ} ${LFLAGS}

clean:
	@rm *.o

analyse:
	@echo "valgrind --tool=callgrind ./${NAME}"
	@echo "kcachegrind callgrind.X"
	@echo "rm callgrind.X"
