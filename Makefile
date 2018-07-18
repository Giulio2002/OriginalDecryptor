#CPP = clang++
#C = clang
CPP = g++
C = gcc

LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf
CFLAGS = -Wall -c -std=c89
BIN = OriginalDecryptor

all: $(BIN)

$(BIN): main.o widgets.o draw.o general.o \
posix.o
	$(C) $^ $(LDFLAGS) -o $@

widgets.o: widgets.c
	$(C) $(CFLAGS) $^ -o $@

draw.o: draw.c
	$(C) $(CFLAGS) $^ -o $@

general.o: general.c
	$(C) $(CFLAGS) $^ -o $@

posix.o: posix.c
	$(C) $(CFLAGS) $^ -o $@

clean:
	rm *.o && rm $(BIN)

