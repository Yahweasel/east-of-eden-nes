.export _vram_addr

_vram_addr:
        ldy $2002
        stx $2006
        sta $2006
        rts
