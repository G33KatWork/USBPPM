TARGET = ppmhost
OBJ = main.o

CFLAGS = -std=gnu99 -Wall $(shell pkg-config libusb-1.0 --cflags) $(shell pkg-config sdl --cflags)
LDFLAGS = $(shell pkg-config libusb-1.0 --libs) $(shell pkg-config sdl --libs)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f *.o
	rm -f $(TARGET)
