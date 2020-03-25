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

.include "banks-gen.inc"

_bank_switch:
        tax
        sty     tmp1
        jsr     popa
        ldy     tmp1
bank_switch:
; a = bank number
; x = function number
; Argument stack is full of arguments for the target
; First we store on the stack our own bank number
        sta     tmp1
        lda     #bank_OWN_BANK
        pha
        lda     tmp1
; Switch banks
        sta     $8000
; Load the address and call the function
        lda     $FEFA, x
        sta     ptr1
        lda     $FEFB, x
        sta     ptr1+1
        jsr     goptr1
; Then switch back to the previous bank and return
        sta     tmp1
        pla
        sta     $8000
        lda     tmp1
        rts

goptr1:
        jmp     (ptr1)
