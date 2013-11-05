
CC = g++ 
OPT = -I. -I/usr/include/glm -ggdb
LIBOPT = -lglut -lGL -lGLEW

all : viewer

viewer : viewer.o shader.o buffer.o program.o vertexarray.o texture.o
	$(CC) -o viewer viewer.o shader.o buffer.o program.o vertexarray.o texture.o $(LIBOPT)

%.o: %.c *.h Makefile
	$(CC) $(OPT) -c -o $@ $< 

clean : 
	rm *.o viewer

