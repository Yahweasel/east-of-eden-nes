; Shamelessly stolen from the NESDev wiki!

.export _readjoy, _waitjoy

.include "zeropage.inc"
.include "nes.inc"

_readjoy:
        lda #1
        sta APU_PAD1
        sta tmp1
        lsr a        ; Carry bit is what matters with this magic algo
        sta APU_PAD1

rjloop:
        lda APU_PAD1
        lsr a       ; bit 0 -> carry
        rol tmp1    ; carry -> bit 0, bit 7 -> carry
        bcc rjloop

        lda tmp1
        rts

_waitjoy:
@onloop:
        jsr _readjoy
        bne @onloop

@offloop:
        jsr _readjoy
        beq @offloop

        rts
