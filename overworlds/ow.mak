OWS=\
	cain_and_abel.ow \
	ff3a.ow \
	ff3b.ow \
	ff3c.ow \
	ff3d.ow

all: $(OWS)

%.ow: %.world
	./overworld-generator.js $< $@
