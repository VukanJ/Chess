CC = g++
CFLAGS = -std=c++17 -Wpedantic -Ofast -O3
NAME = engine
OBJ = $(shell find . -type f -iname "*.cpp" | sed 's/\.cpp$$/.o/1')
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
