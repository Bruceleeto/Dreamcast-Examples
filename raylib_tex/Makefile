

TARGET = raylib.elf
OBJS = main.o gl_png.o romdisk.o
KOS_ROMDISK_DIR = romdisk

CFLAGS = -I${KOS_PORTS}/include/raylib

all: rm-elf convert-texture $(TARGET)

convert-texture:
	mkdir -p $(KOS_ROMDISK_DIR)
	pvrtex -i assets/texture.png -o $(KOS_ROMDISK_DIR)/texture.tex -f RGB565 -c -m
	cp -r assets/* $(KOS_ROMDISK_DIR)/

include $(KOS_BASE)/Makefile.rules

clean: rm-elf
	-rm -f $(OBJS)
	-rm -f $(KOS_ROMDISK_DIR)/*.tex

rm-elf:
	-rm -f $(TARGET) romdisk.*

$(TARGET): $(OBJS)
	kos-cc $(CFLAGS) -o $(TARGET) $(OBJS) -lraylib -lGL -lm -lkosutils 

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

dist: $(TARGET)
	-rm -f $(OBJS) romdisk.img
	$(KOS_STRIP) $(TARGET)