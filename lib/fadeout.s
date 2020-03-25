; Copyright (c) 2019 Yahweasel
;
; Permission to use, copy, modify, and/or distribute this software for any
; purpose with or without fee is hereby granted, provided that the above
; copyright notice and this permission notice appear in all copies.
;
; THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
; WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
; MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
; ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
; WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
; ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
; OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

.export _fadeout, fadebuf, fadestate, fadestore, fadewait

.import _waitvsync, _finvsync, _fadetime

.include "zeropage.inc"
.include "nes.inc"

.segment "DATA"
fadebuf: .res $20, 0
fadestate: .res 1, 0

.segment "CODE"

_fadeout:
        jsr _waitvsync

; First load in the palette data
        ldy #$3f
        sty PPU_VRAM_ADDR2
        ldy #0
        sty PPU_VRAM_ADDR2
loadloop:
        lda PPU_VRAM_IO
        sta fadebuf, y
        iny
        cpy #$20
        bne loadloop

        jsr _finvsync

; Now do the brightness steps
        lda #$30
        sta fadestate

lumaloop:
; Darken everything
        ldy #0
darkloop:
        lda fadebuf, y
        cmp fadestate
        bcc darkloopend
; It's bright, so darken the luma component
        clc
        adc #$f0
        sta fadebuf, y
darkloopend:
        iny
        cpy #$20
        bne darkloop

; Store it back and wait
        jsr fadestore
        jsr fadewait

        lda fadestate
        clc
        adc #$f0
        sta fadestate
        bne lumaloop

; Now turn anything that's still non-black blue
        ldy #0
blueloop:
        lda fadebuf, y
        cmp #$0d
        bcs blueloopend
        lda #$0c
        sta fadebuf, y
blueloopend:
        iny
        cpy #$20
        bne blueloop

; Store it back
        jsr fadestore
        jsr fadewait

; Finally, all black
        ldy #0
        lda #$0f
blackloop:
        sta fadebuf, y
        iny
        cpy #$20
        bne blackloop

        jsr fadestore

        rts


fadestore:
        jsr _waitvsync

; Store the new values
        ldy #$3f
        sty PPU_VRAM_ADDR2
        ldy #0
        sty PPU_VRAM_ADDR2
storeloop:
        lda fadebuf, y
        sta PPU_VRAM_IO
        iny
        cpy #$20
        bne storeloop

        jsr _finvsync

        rts


fadewait:
        ldy _fadetime
        beq endfadewait

slowloop:
        jsr _waitvsync
        dey
        bne slowloop

endfadewait:
        rts
