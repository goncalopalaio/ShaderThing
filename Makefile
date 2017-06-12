# Flags
CFLAGS = -I/usr/local/include -pedantic -O2  -Wno-deprecated -Wno-vla-extension

CC = gcc
#LIBS := -lglfw3 -lopengl32 -lm -lGLU32 -lGLEW32
#LIBS := -lglfw -lGL -lm -lGLU -lGLEW
LIBS := -lglfw3 -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lm -lGLEW -L/usr/local/lib

all: 
	$(CC) $(CFLAGS) -o main main.c $(LIBS)

clean:
	@rm -rf $(OBJS) main