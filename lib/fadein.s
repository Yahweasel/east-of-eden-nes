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

.export _fadein

.import _waitvsync, _finvsync, _paletteTarget, fadebuf, fadestate, fadestore, fadewait

.include "zeropage.inc"
.include "nes.inc"

_fadein:
        jsr _waitvsync

; First step: Make anything that's not supposed to be black, blue
        ldy #$1f
blueloop:
        lda _paletteTarget, y
        cmp #$0f
        beq blueloopend
        lda #$0c
blueloopend:
        sta fadebuf, y
        dey
        bpl blueloop

        jsr fadestore
        jsr fadewait

; Now get the right color, but not brightness
        ldy #0
chromaloop:
        lda _paletteTarget, y
        and #$0f
        sta fadebuf, y
        iny
        cpy #$20
        bne chromaloop

        jsr fadestore

; Now do the brightness steps
        ldx #3

fadeloop:

        jsr fadewait

        ldy #0
brightloop:
        lda _paletteTarget, y
        sta tmp1
        lda fadebuf, y
        cmp tmp1
        beq brightloopend
        clc
        adc #$10
        sta fadebuf, y
brightloopend:
        iny
        cpy #$20
        bne brightloop

        jsr fadestore

        dex
        bne fadeloop

done:
        rts
