#
# KallistiGL test program
# (c)2013-2014 Josh PH3NOM Pearson
# (c)2001 Megan Potter
#   

TARGET = gltest.elf
OBJS = gltest.o pvr-texture.o romdisk.o
KOS_ROMDISK_DIR = romdisk

all: rm-elf convert-texture $(TARGET)

convert-texture:
	mkdir -p $(KOS_ROMDISK_DIR)
	$(KOS_BASE)/utils/pvrtex/pvrtex -i assets/test.png -o $(KOS_ROMDISK_DIR)/test.pvr -f RGB565 -s

include $(KOS_BASE)/Makefile.rules

clean: rm-elf
	-rm -f $(OBJS)
	-rm -f $(KOS_ROMDISK_DIR)/*.pvr

rm-elf:
	-rm -f $(TARGET) romdisk.*

$(TARGET): $(OBJS)
	kos-cc -o $(TARGET) $(OBJS) -lGL -lpcx -lkosutils

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

dist: $(TARGET)
	-rm -f $(OBJS) romdisk.img
	$(KOS_STRIP) $(TARGET)