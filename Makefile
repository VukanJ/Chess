CC = g++
CFLAGS = -std=c++17 -fms-extensions -Ofast -O3 -fpermissive
LFLAGS = -lsfml-window -lsfml-graphics -lsfml-system
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
