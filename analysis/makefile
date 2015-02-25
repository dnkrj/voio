CFLAGS = $(shell pkg-config --cflags opencv)
LIBS = $(shell pkg-config --libs opencv) -lgif
LINK=gifs/gifcv.o
OUTPUT= -o mainOutput.o
CC = g++ -std=c++0x

compile: SimpleFaceStrategy.cpp LazyStrategy.cpp Entry.cpp
	$(CC) Entry.cpp SimpleFaceStrategy.cpp LazyStrategy.cpp $(CFLAGS) $(LIBS) $(LINK)
	$(OUTPUT)
