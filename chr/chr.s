.import popa, chrs

.include "zeropage.inc"
.include "nes.inc"

.segment "JUMPS"
.word loadChr

.segment "CODE"
; cdecl loadChr(unsigned char idx, unsigned char page)
loadChr:
        ; Get the base address
        jsr popa
        sta PPU_VRAM_ADDR2
        stx PPU_VRAM_ADDR2 ; x=0 because this is always function 0

        ; Get the index
        jsr popa

        ; Load the base address
        clc
        adc #>chrs
        sta ptr1+1
        lda #<chrs
        sta ptr1

        ; Copy the page
        ldy #0
chrloop:
        lda (ptr1), y
        sta PPU_VRAM_IO
        iny
        bne chrloop ; repeat per byte

        rts
