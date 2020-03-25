.export _ppubuf_put

.import popa, ppubuf_put

.include "zeropage.inc"
.include "nes.inc"

_ppubuf_put:
        sta tmp1
        jsr popa
        tax
        jsr popa
        tay
        lda tmp1
        jmp ppubuf_put
