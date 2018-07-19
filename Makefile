LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf
CFLAGS = -Wall -c -std=c89
BIN = OriginalDecryptor
SOURCE = main.c draw.c general.c posix.c widgets.c explorer.c menu.c
all: $(BIN)

$(BIN):
	gcc $(SOURCE) $(LDFLAGS) -o $(BIN)

clean:
	rm $(BIN)

