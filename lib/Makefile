OBJS= \
	clearsprites.o dialogue.o dupsprite.o fadein.o fadeout.o farcall.o \
	finvsync.o fullscreen.o libinit.o loadsprite.o ppubuf.o ppubuf_waitempty.o \
	ppubuf_put.o print.o readjoy.o scroll.o unloadsprite.o updatesprite.o \
	vram_addr.o vram_put.o $(FARS)

CC=cl65
AS=ca65
AR=ar65
CFLAGS=-Ois


all: lib.lib

%.o: %.c
	$(CC) $(CFLAGS) -t nes -I.. -c $< -o $@

%.o: %.s
	$(AS) -t nes $<

lib.lib: $(OBJS)
	$(AR) r $@ $(OBJS)

clean:
	rm -f $(OBJS) lib.lib
