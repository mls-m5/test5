#automatically find files from wildcards
CPP_FILES+= $(wildcard src/*.cpp)
OBJECTS = $(CPP_FILES:.cpp=.o)  #find the associated o-filenames
CXXFLAGS = `sdl2-config --cflags` -std=c++11 -Iinclude/
CXXFLAGS+= -g
LIBS = `sdl2-config --libs`
TARGET = test5

#make sure to call the rule
all: ${TARGET} .depend

${TARGET}: ${OBJECTS}
	${CXX} ${CXXFLAGS} -o ${TARGET} ${OBJECTS} ${LIBS}

#rule for calculate dependencies
.depend: ${CPP_FILES} Makefile
	git submodule update --init #pull all submodules from server
	make -C matmake/
	@echo
	@echo Calculating dependencies
	matmake/matdep ${CPP_FILES} ${CXXFLAGS} > .depend
	
#include the dependencies
include .depend
