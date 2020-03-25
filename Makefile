# Generic banks (i.e., banks other than 0)
# THIS MUST BE CONSISTENT IN THREE PLACES:
# Makefile
# banks.h
# banks.inc
#
# Plus, the bank count in bank0.s (in the header) must be correct
#
# For the most part, functionality associated with heavy data goes in each bank
# with said data, replicated if need be. For instance, CHR-loading code is in
# each CHR bank. Many library functions are simply duplicated between banks,
# and the remainder and those with light data are put onto bank 0. There are,
# therefore, banks for CHR, music, worlds and actions, and information on
# particular characters is kept on bank 0.
PRG_BANKS=\
	c1 \
	w1 \
	a1

# All of our banks = bank 0 + those
BANKS=0 $(PRG_BANKS)
BANK_FILES=$(addsuffix .bank,$(BANKS))

# Objects for *all* banks (for uniformity)
UNIV_OBJS=globals.o

# Functionality shared by all CHR banks
CHR_OBJS=chr/chr.o

# Functionality shared by all overworld banks
OW_OBJS=overworld.o

# General purpose library used on-demand by any bank
LIBS=lib/lib.lib

# What goes into each bank
# Note that the order can be critical for jump tables, particularly in
# overworld banks
0_OBJS=bank0.o $(UNIV_OBJS) 0/main.o 0/characters.o 0/battle.o 0/menu.o st-gen.o
c1_OBJS=$(CHR_OBJS) chr/1.o
w1_OBJS=$(OW_OBJS) overworlds/w1-gen.o
a1_OBJS=actions/act1.o actions/ja1-gen.o

CC=cl65
AS=ca65
LD=ld65
CFLAGS=-Ois


all: out.nes

out.nes: $(BANK_FILES)
	cat $(BANK_FILES) > $@

0.bank: $(0_OBJS) $(LIBS)
	$(LD) -C cfgs/0.cfg $(0_OBJS) $(LIBS) nes.lib -o $@ -m $@.map

bank0.s: bankswitch-gen-0.s

define bank_template =
$(1).bank: bank-gen-$(1).o globals.o $$($(1)_OBJS) $$(LIBS)
	$(LD) -C cfgs/prg.cfg bank-gen-$(1).o $$(UNIV_OBJS) $$($(1)_OBJS) $$(LIBS) nes.lib -o $$@ -m $$@.map
endef

$(foreach bank,$(PRG_BANKS),$(eval $(call bank_template,$(bank))))

bank-gen-%.s: bank-in.s bankswitch-gen-%.s
	sed 's/OWN_BANK/$*/' < bank-in.s > $@

bankswitch-gen-%.s: bankswitch-in.s
	sed 's/OWN_BANK/$*/' < bankswitch-in.s > $@

%.o: %.c *.h
	$(CC) $(CFLAGS) -I. -t nes -c $< -o $@

%.o: %.s *.inc
	$(AS) -t nes $<

lib/lib.lib: lib/*.c lib/*.s
	cd lib ; $(MAKE)

test: out.nes
	env MEDNAFEN_HOME="$$PWD/mednafen" mednafen out.nes

clean:
	cd lib ; $(MAKE) clean
	rm -f *.o 0/*.o chr/*.o overworlds/*.o actions/*.o bank-gen-*.s bankswitch-gen-*.s $(BANK_FILES) *.map out.nes
