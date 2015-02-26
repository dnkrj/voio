CFLAGS = $(shell pkg-config --cflags opencv)
LIBS = $(shell pkg-config --libs opencv) 
LINK=  gifs/gifcv.o -lgif gifs/timestamp.o
LINKLOCAL = gifs/gifcv.o libgif.so.7 gifs/timestamp.o
OUTPUT= -o mainOutput.o
CC = g++ -std=c++0x

compile: SimpleFaceStrategy.cpp LazyStrategy.cpp Entry.cpp
	$(CC) Entry.cpp SimpleFaceStrategy.cpp LazyStrategy.cpp $(CFLAGS) $(LIBS) $(LINK) $(OUTPUT)

compile-local: SimpleFaceStrategy.cpp LazyStrategy.cpp Entry.cpp
	$(CC) Entry.cpp SimpleFaceStrategy.cpp LazyStrategy.cpp $(CFLAGS) $(LINKLOCAL) $(LIBS) $(OUTPUT)
	
	cp mainOutput.o ~/Development/mainOutput.o
