all: \
	nes.char.chr \
	title.b1.chr \
	icons1.b0.chr \
	back1.b1.chr \
	ff3a.b1.chr \
	ff3b.b1.chr \
	ff3c.b1.chr \
	ff3d.b1.chr \
	spr1.spr.chr

%.b0.chr: %.b0.bmp
	bmp2chr $< $@.tmp
	dd if=$@.tmp of=$@ bs=256 count=2
	rm -f $@.tmp

%.char.chr: %.char.bmp
	bmp2chr $< $@.tmp
	dd if=$@.tmp of=$@ bs=256 skip=2 count=6
	rm -f $@.tmp

%.b1.chr: %.b1.bmp
	bmp2chr $< $@.tmp
	dd if=$@.tmp of=$@ bs=256 skip=8 count=8
	rm -f $@.tmp

%.spr.chr: %.spr.bmp
	bmp2chr $< $@

%.bmp: %.bmp.xz
	unxz -k $<
