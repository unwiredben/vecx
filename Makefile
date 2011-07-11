PalmPDK=C:\Program Files (x86)\HP webOS\PDK
CC := arm-none-linux-gnueabi-gcc
CFLAGS := -O3 -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp -Wall -Wextra -std=c99 -fgnu89-inline "-I$(PalmPDK)/include" "-I$(PalmPDK)/include/SDL" -I../SDL_gfx
LIBS := -Wl,--allow-shlib-undefined "-L$(PalmPDK)/device/lib" ../SDL_gfx/libSDL_gfx.a -lSDL
OBJECTS := e6809.o osint.o vecx.o
TARGET := vecx
CLEANFILES := $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	$(RM) $(CLEANFILES)
