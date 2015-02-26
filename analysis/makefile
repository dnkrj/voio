CFLAGS = $(shell pkg-config --cflags opencv)
LIBS = $(shell pkg-config --libs opencv) 
LINK=-lgif  gifs/gifcv.o
LINKLOCAL = gifs/gifcv.o libgif.so.7
OUTPUT= -o mainOutput.o
CC = g++ -std=c++0x

compile: SimpleFaceStrategy.cpp LazyStrategy.cpp Entry.cpp
	$(CC) Entry.cpp SimpleFaceStrategy.cpp LazyStrategy.cpp $(CFLAGS) $(LIBS) $(LINK) $(OUTPUT)

compile-local: SimpleFaceStrategy.cpp LazyStrategy.cpp Entry.cpp
	$(CC) Entry.cpp SimpleFaceStrategy.cpp LazyStrategy.cpp $(CFLAGS) $(LIBS) $(LINK-LOCAL) $(OUTPUT)
