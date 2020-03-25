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

.export _finvsync

.import _scrollXC, _scrollXT, _scrollXS, _scrollYT, _scrollYS

.include "nes.inc"

_finvsync:
        ; Load the control
        lda #$88
        ora _scrollXC
        sta PPU_CTRL1

        ; And scroll
        lda _scrollXT
.repeat 4
        asl a
.endrepeat
        ora _scrollXS
        sta PPU_VRAM_ADDR1

        lda _scrollYT
.repeat 4
        asl a
.endrepeat
        ora _scrollYS
        sta PPU_VRAM_ADDR1
        rts
