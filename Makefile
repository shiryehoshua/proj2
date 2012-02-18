##
## Makefile for Project 2
##

# Compiler settings
CC     ?= cc

GLFW_DIR_MACLAB = /opt/gfx
IS_MACLAB = $(wildcard $(GLFW_DIR_MACLAB))

ifeq ($(strip $(IS_MACLAB)), $(GLFW_DIR_MACLAB)) 
CFLAGS ?= -Wall -O2 -g -I/opt/gfx/include
LFLAGS = -L/opt/gfx/lib -lglfw -framework Cocoa -framework OpenGL ../lib/libAntTweakBar.dylib \
				 ../lib/libpng15.a
else 
CFLAGS ?= -Wall -O2 -g -I/usr/local/include/ `pkg-config --cflags libpng`
LFLAGS = -L/usr/local/lib -lglfw -framework Cocoa -framework OpenGL `pkg-config --libs libpng` \
				 /usr/local/lib/libAntTweakBar.dylib
endif 

OBJS = callbacks.o matrixFunctions.o proj2.o spotGeomMethods.o spotGeomShapes.o spotImage.o \
			 spotUtils.o
INCLUDES = spot.h callbacks.h matrixFunctions.h spot.h spotMacros.h types.h

all: proj2 $(OBJS)

%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c -o $@ $< 

proj2: $(OBJS) $(INCLUDES)
	$(CC) $(OBJS) $(LFLAGS) -o proj2

clean:
	rm -rf proj2 proj2.dSYM $(OBJS)

run: proj2
	./proj2
