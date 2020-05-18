CC=g++
CFLAGS=-c -Wall -std=c++0x
LDFLAGS=
SOURCES=main.cpp array_deque.cpp lz77_encoder.cpp ByteOperations.cpp LZ77Decoder.cpp sha1.cpp files.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=lz77

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@