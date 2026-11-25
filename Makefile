all: obj/ build/ obj/main.c.o obj/constraints.c.o obj/physics.c.o obj/shape_generators.c.o obj/spatial.c.o obj/types.c.o build/climb 
OSMODE := l

obj/: 
ifeq (${OSMODE}, l)
	mkdir -p obj
else
	mkdir obj
endif

build/: 
ifeq (${OSMODE}, l)
	mkdir -p build
else
	mkdir build
endif

obj/main.c.o: src/main.c 
ifeq (${OSMODE}, l)
	${CC} src/main.c -c -o obj/main.c.o -Iengine/include -std=c23
else
	${CC} src/main.c -c -o obj/main.c.o -Iengine/include -std=c23
endif


obj/constraints.c.o: engine/src/constraints.c 
ifeq (${OSMODE}, l)
	${CC} engine/src/constraints.c -c -o obj/constraints.c.o -Iengine/include -std=c23
else
	${CC} engine/src/constraints.c -c -o obj/constraints.c.o -Iengine/include -std=c23
endif


obj/physics.c.o: engine/src/physics.c 
ifeq (${OSMODE}, l)
	${CC} engine/src/physics.c -c -o obj/physics.c.o -Iengine/include -std=c23
else
	${CC} engine/src/physics.c -c -o obj/physics.c.o -Iengine/include -std=c23
endif


obj/shape_generators.c.o: engine/src/shape_generators.c 
ifeq (${OSMODE}, l)
	${CC} engine/src/shape_generators.c -c -o obj/shape_generators.c.o -Iengine/include -std=c23
else
	${CC} engine/src/shape_generators.c -c -o obj/shape_generators.c.o -Iengine/include -std=c23
endif


obj/spatial.c.o: engine/src/spatial.c 
ifeq (${OSMODE}, l)
	${CC} engine/src/spatial.c -c -o obj/spatial.c.o -Iengine/include -std=c23
else
	${CC} engine/src/spatial.c -c -o obj/spatial.c.o -Iengine/include -std=c23
endif


obj/types.c.o: engine/src/types.c 
ifeq (${OSMODE}, l)
	${CC} engine/src/types.c -c -o obj/types.c.o -Iengine/include -std=c23
else
	${CC} engine/src/types.c -c -o obj/types.c.o -Iengine/include -std=c23
endif


build/climb: obj/main.c.o obj/constraints.c.o obj/physics.c.o obj/shape_generators.c.o obj/spatial.c.o obj/types.c.o 
ifeq (${OSMODE}, l)
	${CC} obj/main.c.o obj/constraints.c.o obj/physics.c.o obj/shape_generators.c.o obj/spatial.c.o obj/types.c.o -o build/climb -lSDL3 -lm
else
	${CC} obj/main.c.o obj/constraints.c.o obj/physics.c.o obj/shape_generators.c.o obj/spatial.c.o obj/types.c.o -o build/climb -lSDL3 -lm
endif


clean:
	rm -r obj
	rm -r build
.PHONY: clean


gencommands:
	mkdir emmgtemp
ifeq (${OSMODE}, l)
	clang src/main.c  -Iengine/include -std=c23 -MJ emmgtemp/0.json -fsyntax-only
else
	clang src/main.c  -Iengine/include -std=c23 -MJ emmgtemp/0.json -fsyntax-only
endif
ifeq (${OSMODE}, l)
	clang engine/src/constraints.c  -Iengine/include -std=c23 -MJ emmgtemp/1.json -fsyntax-only
else
	clang engine/src/constraints.c  -Iengine/include -std=c23 -MJ emmgtemp/1.json -fsyntax-only
endif
ifeq (${OSMODE}, l)
	clang engine/src/physics.c  -Iengine/include -std=c23 -MJ emmgtemp/2.json -fsyntax-only
else
	clang engine/src/physics.c  -Iengine/include -std=c23 -MJ emmgtemp/2.json -fsyntax-only
endif
ifeq (${OSMODE}, l)
	clang engine/src/shape_generators.c  -Iengine/include -std=c23 -MJ emmgtemp/3.json -fsyntax-only
else
	clang engine/src/shape_generators.c  -Iengine/include -std=c23 -MJ emmgtemp/3.json -fsyntax-only
endif
ifeq (${OSMODE}, l)
	clang engine/src/spatial.c  -Iengine/include -std=c23 -MJ emmgtemp/4.json -fsyntax-only
else
	clang engine/src/spatial.c  -Iengine/include -std=c23 -MJ emmgtemp/4.json -fsyntax-only
endif
ifeq (${OSMODE}, l)
	clang engine/src/types.c  -Iengine/include -std=c23 -MJ emmgtemp/5.json -fsyntax-only
else
	clang engine/src/types.c  -Iengine/include -std=c23 -MJ emmgtemp/5.json -fsyntax-only
endif
# not cross platform here sad i think
	echo [ > emmgtemp/[
	echo ] > emmgtemp/]
	cat emmgtemp/[ emmgtemp/*.json emmgtemp/] > compile_commands.json
	rm -r emmgtemp
