CC = g++
CFLAGS = -std=c++17 -fms-extensions -Ofast -O3 -fpermissive
LFLAGS = -lsfml-window -lsfml-graphics -lsfml-system
NAME = engine
OBJ = Main.o ZobristHash.o Testing.o AI.o gui.o Board.o Data.o Move.o
DEBUG = #-g

compile: ${OBJ} EXE

%.o: %.cpp
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
