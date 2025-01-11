TARGET = quad.elf
OBJS = quad.o romdisk.o
KOS_ROMDISK_DIR = romdisk

all: rm-elf $(TARGET)

include $(KOS_BASE)/Makefile.rules

KOS_CFLAGS += -O0 -I$(KOS_BASE)/utils # 03 is bad but idk why.

romdisk/dc.dt:  dc.png
	mkdir -p romdisk
	$(KOS_BASE)/utils/pvrtex/pvrtex -f RGB565 -c -i $< -o $@

$(TARGET): romdisk/dc.dt $(OBJS)
	kos-cc -o $(TARGET) $(OBJS) -lpng -ljpeg -lkmg -lz -lkosutils -lm

clean: rm-elf
	-rm -f $(OBJS)
	-rm -rf romdisk/*

rm-elf:
	-rm -f $(TARGET) romdisk.*

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

dist:
	rm -f $(OBJS) romdisk.o romdisk.img
	$(KOS_STRIP) $(TARGET)	 
