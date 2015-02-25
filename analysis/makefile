CFLAGS = $(shell pkg-config --cflags opencv)
LIBS = $(shell pkg-config --libs opencv) 
LINK=gifs/gifcv.o -lgif
OUTPUT= -o mainOutput.o
CC = g++ -std=c++0x

compile: SimpleFaceStrategy.cpp LazyStrategy.cpp Entry.cpp
	$(CC) Entry.cpp SimpleFaceStrategy.cpp LazyStrategy.cpp $(CFLAGS) $(LIBS) $(LINK)
	$(OUTPUT)
